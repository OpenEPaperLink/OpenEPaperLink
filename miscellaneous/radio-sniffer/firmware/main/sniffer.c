// Passive IEEE 802.15.4 sniffer for OpenEPaperLink timing measurements (ESP32-C6 / ESP32-H2).
//
// Never transmits. Every received frame goes to the host over the native USB port (USB-Serial-JTAG)
// with the SFD timestamp, RSSI and LQI. Optionally toggles SYNC_GPIO on every frame to or from one
// MAC address, so a power profiler can put current and radio frames on one timeline.
//
// Record (device -> host), little endian:
//   0xA5 0x5A, type u8, payload length u16, payload, checksum u8 (sum of payload bytes)
//   type 1 FRAME : ts_us u64, rssi i8, lqi u8, channel u8, len u8, frame[len] (FCS included)
//   type 2 STATUS: ts_us u64, received u32, dropped_queue u32, dropped_usb u32, channel u8, sync_on u8
//   type 3 HELLO : version u8, channel u8
// Commands (host -> device), ASCII lines:
//   C<11..26>   set channel
//   M<16 hex>   toggle SYNC_GPIO on frames to/from this MAC (bytes in over-the-air order)
//   M           sync off
//   S           send a STATUS record now
//   T           wiring test: toggle SYNC_GPIO at 20 Hz for 2 s

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "driver/gpio.h"
#include "driver/usb_serial_jtag.h"
#include "esp_attr.h"
#include "esp_ieee802154.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "hal/gpio_ll.h"
#include "nvs_flash.h"

#define SNIFFER_VERSION 1
#define SYNC_GPIO       2
#define DEFAULT_CHANNEL 25
#define QUEUE_LEN       128

#define REC_FRAME  1
#define REC_STATUS 2
#define REC_HELLO  3

static const char *TAG = "sniffer";

typedef struct {
    uint64_t ts;
    int8_t rssi;
    uint8_t lqi;
    uint8_t channel;
    uint8_t len;
    uint8_t data[127];
} frame_rec_t;

static QueueHandle_t s_queue;
static volatile uint32_t s_received, s_dropped_queue, s_dropped_usb;
static volatile uint8_t s_channel = DEFAULT_CHANNEL;
static uint8_t s_sync_mac[8];
static volatile bool s_sync_on;
static uint32_t s_sync_level;

// True when the frame carries `mac` as long source or destination address.
static IRAM_ATTR bool frame_has_mac(const uint8_t *p, uint8_t len, const uint8_t *mac) {
    if (len < 3) return false;
    uint8_t dst_mode = (p[1] >> 2) & 3;
    uint8_t src_mode = (p[1] >> 6) & 3;
    bool pan_comp = (p[0] >> 6) & 1;
    uint8_t off = 3;  // FCF + sequence number
    if (dst_mode) {
        off += 2;  // destination PAN
        if (dst_mode == 3) {
            if (off + 8 <= len && memcmp(p + off, mac, 8) == 0) return true;
            off += 8;
        } else {
            off += 2;
        }
    }
    if (src_mode) {
        if (!pan_comp || !dst_mode) off += 2;  // source PAN
        if (src_mode == 3 && off + 8 <= len && memcmp(p + off, mac, 8) == 0) return true;
    }
    return false;
}

IRAM_ATTR void esp_ieee802154_receive_done(uint8_t *frame, esp_ieee802154_frame_info_t *frame_info) {
    static frame_rec_t rec;  // only used from this ISR
    uint8_t len = frame[0] > 127 ? 127 : frame[0];
    rec.ts = frame_info->timestamp;
    rec.rssi = frame_info->rssi;
    rec.lqi = frame_info->lqi;
    rec.channel = frame_info->channel;
    rec.len = len;
    memcpy(rec.data, frame + 1, len);

    if (s_sync_on && frame_has_mac(rec.data, len, s_sync_mac)) {
        s_sync_level ^= 1;
        gpio_ll_set_level(&GPIO, SYNC_GPIO, s_sync_level);
    }

    BaseType_t woken = pdFALSE;
    if (xQueueSendFromISR(s_queue, &rec, &woken) == pdTRUE) {
        s_received++;
    } else {
        s_dropped_queue++;
    }
    esp_ieee802154_receive_handle_done(frame);
    if (woken) portYIELD_FROM_ISR();
}

static void send_record(uint8_t type, const uint8_t *payload, uint16_t len) {
    static uint8_t buf[5 + 140 + 1];
    if (len > 140) return;
    buf[0] = 0xA5;
    buf[1] = 0x5A;
    buf[2] = type;
    buf[3] = len & 0xFF;
    buf[4] = len >> 8;
    memcpy(buf + 5, payload, len);
    uint8_t sum = 0;
    for (uint16_t i = 0; i < len; i++) sum += payload[i];
    buf[5 + len] = sum;
    int n = 6 + len;
    if (usb_serial_jtag_write_bytes(buf, n, pdMS_TO_TICKS(20)) != n) s_dropped_usb++;
}

static void send_status(void) {
    uint8_t p[8 + 4 + 4 + 4 + 2];
    uint64_t ts = esp_timer_get_time();
    uint32_t r = s_received, dq = s_dropped_queue, du = s_dropped_usb;
    memcpy(p, &ts, 8);
    memcpy(p + 8, &r, 4);
    memcpy(p + 12, &dq, 4);
    memcpy(p + 16, &du, 4);
    p[20] = s_channel;
    p[21] = s_sync_on;
    send_record(REC_STATUS, p, sizeof(p));
}

static void set_channel(uint8_t ch) {
    if (ch < 11 || ch > 26) return;
    s_channel = ch;
    esp_ieee802154_set_channel(ch);
    esp_ieee802154_receive();
}

static void handle_command(char *line) {
    switch (line[0]) {
        case 'C':
            set_channel((uint8_t)atoi(line + 1));
            break;
        case 'T':
            // Wiring test: toggle SYNC_GPIO 40 times at 20 Hz (2 s), independent of radio frames.
            for (int i = 0; i < 40; i++) {
                s_sync_level ^= 1;
                gpio_set_level(SYNC_GPIO, s_sync_level);
                vTaskDelay(pdMS_TO_TICKS(50));
            }
            break;
        case 'M':
            if (strlen(line + 1) >= 16) {
                for (int i = 0; i < 8; i++) {
                    char hex[3] = {line[1 + 2 * i], line[2 + 2 * i], 0};
                    s_sync_mac[i] = (uint8_t)strtoul(hex, NULL, 16);
                }
                s_sync_on = true;
            } else {
                s_sync_on = false;
            }
            break;
        default:
            break;
    }
    send_status();
}

static void tx_task(void *arg) {
    frame_rec_t rec;
    uint8_t p[8 + 4 + 127];
    int64_t last_status = 0;
    for (;;) {
        if (xQueueReceive(s_queue, &rec, pdMS_TO_TICKS(100)) == pdTRUE) {
            memcpy(p, &rec.ts, 8);
            p[8] = (uint8_t)rec.rssi;
            p[9] = rec.lqi;
            p[10] = rec.channel;
            p[11] = rec.len;
            memcpy(p + 12, rec.data, rec.len);
            send_record(REC_FRAME, p, 12 + rec.len);
        }
        int64_t now = esp_timer_get_time();
        if (now - last_status >= 1000000) {
            last_status = now;
            send_status();
        }
    }
}

static void rx_task(void *arg) {
    char line[40];
    int pos = 0;
    uint8_t c;
    for (;;) {
        if (usb_serial_jtag_read_bytes(&c, 1, portMAX_DELAY) != 1) continue;
        if (c == '\n' || c == '\r') {
            if (pos) {
                line[pos] = 0;
                handle_command(line);
                pos = 0;
            }
        } else if (pos < (int)sizeof(line) - 1) {
            line[pos++] = (char)c;
        }
    }
}

void app_main(void) {
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    usb_serial_jtag_driver_config_t usb_cfg = USB_SERIAL_JTAG_DRIVER_CONFIG_DEFAULT();
    usb_cfg.tx_buffer_size = 16384;
    usb_cfg.rx_buffer_size = 256;
    ESP_ERROR_CHECK(usb_serial_jtag_driver_install(&usb_cfg));

    gpio_reset_pin(SYNC_GPIO);
    gpio_set_direction(SYNC_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(SYNC_GPIO, 0);

    s_queue = xQueueCreate(QUEUE_LEN, sizeof(frame_rec_t));

    ESP_ERROR_CHECK(esp_ieee802154_enable());
    esp_ieee802154_set_promiscuous(true);
    esp_ieee802154_set_coordinator(false);
    esp_ieee802154_set_rx_when_idle(true);
    set_channel(DEFAULT_CHANNEL);

    xTaskCreate(tx_task, "tx", 4096, NULL, 10, NULL);
    xTaskCreate(rx_task, "rx", 3072, NULL, 5, NULL);

    uint8_t hello[2] = {SNIFFER_VERSION, s_channel};
    send_record(REC_HELLO, hello, sizeof(hello));
    ESP_LOGI(TAG, "sniffing on channel %d", s_channel);
}
