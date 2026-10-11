#include "system.h"

#include <Arduino.h>
#include <FS.h>
#include <Preferences.h>
#include <esp_netif.h>
#include <esp_sntp.h>

#include <algorithm>
#include <map>
#if CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH && CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF
#include <esp_core_dump.h>
#include <esp_flash.h>
#endif

#include "storage.h"
#include "tag_db.h"
#include "wifimanager.h"

static time_t lastNtpSync = 0;
static bool crashDumpFound = false;

void timeSyncCallback(struct timeval* tv) {
    lastNtpSync = tv->tv_sec;
    Serial.println("time succesfully synced");
}

time_t lastTimeSync() {
    return lastNtpSync;
}

String ntpServerName() {
    const char* name = sntp_getservername(0);
    if (name != nullptr && name[0] != '\0') return String(name);
    const ip_addr_t* server = sntp_getserver(0);
    if (server != nullptr && !ip_addr_isany(server)) return String(ipaddr_ntoa(server));
    return String();
}

bool crashedLastBoot() {
    return crashDumpFound;
}

void enableNtpFromDhcp() {
#if LWIP_DHCP_GET_NTP_SRV
    // has to be enabled before the DHCP lease, to use the NTP server option of the DHCP server. It goes
    // through the lwIP thread, so start the network stack first (safe to call again later).
    esp_netif_init();
    esp_sntp_servermode_dhcp(true);
#endif
}

void initTime(void* parameter) {
    if (!(WiFi.status() == WL_CONNECTED || wm.wifiStatus == ETHERNET)) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    sntp_set_time_sync_notification_cb(timeSyncCallback);
    sntp_set_sync_interval(300 * 1000);
    // A single NTP server: the one from DHCP when the network provides it, else time.cloudflare.com (anycast,
    // one address; a cached pool.ntp.org answer can point to a server that doesn't respond for minutes).
    // With several servers, lwIP moves on to the next one without any delay when a name doesn't resolve,
    // which flooded the DNS server when there was no internet (#167). With one server it backs off.
    if (sntp_enabled()) sntp_stop();
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    const ip_addr_t* dhcpServer = sntp_getserver(0);
    if (dhcpServer == nullptr || ip_addr_isany(dhcpServer)) {
        sntp_setservername(0, "time.cloudflare.com");
        Serial.println("NTP server: time.cloudflare.com");
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

const char* resetReasonName() {
    switch (esp_reset_reason()) {
        case ESP_RST_POWERON:
            return "Power-on";
        case ESP_RST_EXT:
            return "External";
        case ESP_RST_SW:
            return "Software";
        case ESP_RST_PANIC:
            return "Panic";
        case ESP_RST_INT_WDT:
            return "Watchdog";
        case ESP_RST_TASK_WDT:
            return "Task Watchdog";
        case ESP_RST_WDT:
            return "Other Watchdog";
        case ESP_RST_DEEPSLEEP:
            return "Deep Sleep";
        case ESP_RST_BROWNOUT:
            return "Brownout";
        case ESP_RST_SDIO:
            return "SDIO";
        default:
            return "Unknown";
    }
}

void logStartUp() {
    logLine(String("Reboot. Reason: ") + resetReasonName());
}

/// @brief A reset caused by a crash, which writes a core dump
static bool resetByCrash() {
    switch (esp_reset_reason()) {
        case ESP_RST_PANIC:
        case ESP_RST_INT_WDT:
        case ESP_RST_TASK_WDT:
        case ESP_RST_WDT:
            return true;
        default:
            return false;
    }
}

/// @brief Write a summary of the core dump of a crash (stored in flash by a panic or watchdog) to the log
/// @note The dump stays in flash, for download via /coredump, until the next crash overwrites it.
/// Decode the backtrace with: xtensa-esp32s3-elf-addr2line -pfiaC -e firmware.elf <addresses>
void logCoreDump() {
#if CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH && CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF
    // a dump without a crash reset is one that was logged before
    if (!resetByCrash() || esp_core_dump_image_check() != ESP_OK) return;
    crashDumpFound = true;

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
#endif
}

size_t coreDumpSize() {
#if CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH && CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF
    size_t address, size;
    if (esp_core_dump_image_check() == ESP_OK && esp_core_dump_image_get(&address, &size) == ESP_OK) return size;
#endif
    return 0;
}

size_t readCoreDump(uint8_t* buffer, size_t offset, size_t len) {
#if CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH && CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF
    size_t address, size;
    if (esp_core_dump_image_get(&address, &size) != ESP_OK || offset >= size) return 0;
    len = std::min(len, size - offset);
    if (esp_flash_read(NULL, buffer, address + offset, len) == ESP_OK) return len;
#endif
    return 0;
}

bool eraseCoreDump() {
#if CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH && CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF
    return esp_core_dump_image_erase() == ESP_OK;
#else
    return false;
#endif
}
