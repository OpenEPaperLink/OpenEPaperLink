#include "system.h"

#include <Arduino.h>
#include <FS.h>
#include <Preferences.h>
#include <esp_sntp.h>

#include <map>
#if CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH && CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF
#include <esp_core_dump.h>
#endif

#include "storage.h"
#include "tag_db.h"
#include "wifimanager.h"

void timeSyncCallback(struct timeval* tv) {
    Serial.println("time succesfully synced");
}

void enableNtpFromDhcp() {
#if LWIP_DHCP_GET_NTP_SRV
    // has to be enabled before the DHCP lease, to use the NTP server option of the DHCP server
    esp_sntp_servermode_dhcp(true);
#endif
}

void initTime(void* parameter) {
    if (!(WiFi.status() == WL_CONNECTED || wm.wifiStatus == ETHERNET)) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    sntp_set_time_sync_notification_cb(timeSyncCallback);
    sntp_set_sync_interval(300 * 1000);
    // A single NTP server: the one from DHCP when the network provides it, else pool.ntp.org. With several
    // servers, lwIP moves on to the next one without any delay when a name doesn't resolve, which flooded
    // the DNS server when there was no internet (#167). With one server it backs off between retries.
    if (sntp_enabled()) sntp_stop();
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    const ip_addr_t* dhcpServer = sntp_getserver(0);
    if (dhcpServer == nullptr || ip_addr_isany(dhcpServer)) {
        sntp_setservername(0, "pool.ntp.org");
        Serial.println("NTP server: pool.ntp.org");
    } else {
        Serial.printf("NTP server from DHCP: %s\r\n", ipaddr_ntoa(dhcpServer));
    }
    sntp_init();
    setenv("TZ", config.timeZone, 1);
    tzset();
    struct tm timeinfo;
    while (millis() < 30000) {
        if (!getLocalTime(&timeinfo)) {
            Serial.println("Waiting for valid time from NTP-server");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        } else {
            break;
        }
    }
    // log after the time sync, so the log entries get the right timestamp
    logStartUp();
    logCoreDump();
    if (config.runStatus == RUNSTATUS_INIT) {
        config.runStatus = RUNSTATUS_RUN;
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
    vTaskDelete(NULL);
}

void logLine(const char* buffer) {
    logLine(String(buffer));
}

void logLine(const String& text) {
    time_t now;
    time(&now);

    char timeStr[24];
    const char* format = (now < (time_t)1672531200) ? "           %H:%M:%S " : "%Y-%m-%d %H:%M:%S ";
    strftime(timeStr, sizeof(timeStr), format, localtime(&now));

    xSemaphoreTake(fsMutex, portMAX_DELAY);
    File logFile = contentFS->open("/log.txt", "a");
    if (logFile) {
        if (logFile.size() >= 10 * 1024) {
            logFile.close();
            contentFS->remove("/logold.txt");
            contentFS->rename("/log.txt", "/logold.txt");
            logFile = contentFS->open("/log.txt", "a");
            if (!logFile) {
                xSemaphoreGive(fsMutex);
                return;
            }
        }

        logFile.print(timeStr);
        logFile.println(text);
        logFile.close();
    }
    xSemaphoreGive(fsMutex);
}

/// @brief Log a line at most once per interval for the same key (log.txt is only 10 KB)
void logLineRateLimited(const String& key, const String& text, const uint32_t intervalSeconds) {
    static std::map<String, uint32_t> lastLogged;
    static SemaphoreHandle_t mutex = xSemaphoreCreateMutex();
    const uint32_t now = millis() / 1000;
    xSemaphoreTake(mutex, portMAX_DELAY);
    auto it = lastLogged.find(key);
    const bool log = it == lastLogged.end() || now - it->second >= intervalSeconds;
    if (log) {
        if (lastLogged.size() > 50) lastLogged.clear();  // bound the memory use
        lastLogged[key] = now;
    }
    xSemaphoreGive(mutex);
    if (log) logLine(text);
}

/// @brief Url without query string (which can hold api keys), for the log
String urlForLog(const String& url) {
    const int query = url.indexOf('?');
    return query == -1 ? url : url.substring(0, query);
}

void logStartUp() {
    esp_reset_reason_t resetReason = esp_reset_reason();

    String logEntry = "Reboot. Reason: ";
    switch (resetReason) {
        case ESP_RST_POWERON:
            logEntry += "Power-on";
            break;
        case ESP_RST_EXT:
            logEntry += "External";
            break;
        case ESP_RST_SW:
            logEntry += "Software";
            break;
        case ESP_RST_PANIC:
            logEntry += "Panic";
            break;
        case ESP_RST_INT_WDT:
            logEntry += "Watchdog";
            break;
        case ESP_RST_TASK_WDT:
            logEntry += "Task Watchdog";
            break;
        case ESP_RST_WDT:
            logEntry += "Other Watchdog";
            break;
        case ESP_RST_DEEPSLEEP:
            logEntry += "Deep Sleep";
            break;
        case ESP_RST_BROWNOUT:
            logEntry += "Brownout";
            break;
        case ESP_RST_SDIO:
            logEntry += "SDIO";
            break;
        default:
            logEntry += "Unknown";
            break;
    }

    logLine(logEntry);
}

/// @brief Write a summary of a crash core dump (stored in flash by a panic or watchdog) to the log, then erase it
/// @note Decode the backtrace with: xtensa-esp32s3-elf-addr2line -pfiaC -e firmware.elf <addresses>
void logCoreDump() {
#if CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH && CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF
    if (esp_core_dump_image_check() != ESP_OK) return;

    esp_core_dump_summary_t* summary = static_cast<esp_core_dump_summary_t*>(malloc(sizeof(esp_core_dump_summary_t)));
    if (summary == nullptr) return;
    if (esp_core_dump_get_summary(summary) == ESP_OK) {
        char buffer[160];
#if CONFIG_IDF_TARGET_ARCH_XTENSA
        snprintf(buffer, sizeof(buffer), "Crash in task %.16s, PC 0x%08lx, cause %lu, vaddr 0x%08lx, elf %.8s",
                 summary->exc_task, (unsigned long)summary->exc_pc, (unsigned long)summary->ex_info.exc_cause,
                 (unsigned long)summary->ex_info.exc_vaddr, (const char*)summary->app_elf_sha256);
#else
        snprintf(buffer, sizeof(buffer), "Crash in task %.16s, PC 0x%08lx, elf %.8s",
                 summary->exc_task, (unsigned long)summary->exc_pc, (const char*)summary->app_elf_sha256);
#endif
        logLine(buffer);

        String backtrace = "Backtrace:";
        for (uint32_t i = 0; i < summary->exc_bt_info.depth && i < 16; i++) {
            snprintf(buffer, sizeof(buffer), " 0x%08lx", (unsigned long)summary->exc_bt_info.bt[i]);
            backtrace += buffer;
        }
        if (summary->exc_bt_info.corrupted) backtrace += " |<-CORRUPTED";
        logLine(backtrace);
    }
    free(summary);
    esp_core_dump_image_erase();
#endif
}
