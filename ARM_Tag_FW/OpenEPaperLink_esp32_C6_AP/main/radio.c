#include <stddef.h>
#include "radio.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_ieee802154.h"
#include "esp_log.h"
#include "esp_phy_init.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "led.h"
#include "main.h"
#include "proto.h"
#include "sdkconfig.h"
// if you get an error about soc/lp_uart_reg.h not being found,
// you didn't choose the right build target. :-)
#ifdef CONFIG_IDF_TARGET_ESP32C6
#include "soc/lp_uart_reg.h"
#endif
#include "soc/uart_struct.h"
#include "utils.h"
#include <esp_mac.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>
#include "SubGigRadio.h"


static const char *TAG = "RADIO";

uint8_t mSelfMac[8];
volatile uint8_t isInTransmit = 0;
QueueHandle_t packet_buffer = NULL;
TaskHandle_t radioWakeTask = NULL;  // main loop task; woken by radio frames and serial data
int64_t lastRxAgeUs = 0;            // age of the frame commsRxUnencrypted() returned last, from its SFD
int64_t lastRxSfdUs = 0;            // its SFD time (esp_timer)
int8_t lastRxRssi = 0;              // its RSSI and LQI as measured here (0 for sub-GHz)
uint8_t lastRxLqi = 0;
uint32_t radioRxFrames = 0;
uint32_t radioTxFrames = 0;

struct rxItem {
    int64_t sfdTime;  // esp_timer time of the frame's start-of-frame delimiter
    int8_t rssi;
    uint8_t lqi;
    uint8_t frame[130];
};
// No logging per frame in these callbacks: they run in the radio interrupt, and an early log
// waits for the 115200 baud console, long enough to miss the tags' reply windows (2-6 ms).
void esp_ieee802154_receive_done(uint8_t *frame, esp_ieee802154_frame_info_t *frame_info) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    static struct rxItem item;
    item.sfdTime = frame_info->timestamp;
    item.rssi = frame_info->rssi;
    item.lqi = frame_info->lqi;
    memcpy(item.frame, &frame[0], frame[0] + 1);
    xQueueSendFromISR(packet_buffer, (void *)&item, &xHigherPriorityTaskWoken);
    if (radioWakeTask) vTaskNotifyGiveFromISR(radioWakeTask, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR_ARG(xHigherPriorityTaskWoken);
    if(esp_ieee802154_receive_handle_done(frame)) {
        ESP_EARLY_LOGI(TAG, "esp_ieee802154_receive_handle_done() failed");
    }
}

// Both TX callbacks wake the main loop: it sends a block burst one frame at a time
static void txFinished(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    isInTransmit = 0;
    if (radioWakeTask) vTaskNotifyGiveFromISR(radioWakeTask, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR_ARG(xHigherPriorityTaskWoken);
}

void esp_ieee802154_transmit_failed(const uint8_t *frame, esp_ieee802154_tx_error_t error) {
    txFinished();
    ESP_EARLY_LOGE(TAG, "TX Err: %d", error);
}

void esp_ieee802154_transmit_done(const uint8_t *frame, const uint8_t *ack, esp_ieee802154_frame_info_t *ack_frame_info) {
    txFinished();
    if(ack != NULL) {
       if(esp_ieee802154_receive_handle_done(ack)) {
          ESP_EARLY_LOGI(TAG, "esp_ieee802154_receive_handle_done() failed");
       }
    }
}
static bool zigbee_is_enabled = false;
void radio_init(uint8_t ch) {
    if (packet_buffer == NULL) packet_buffer = xQueueCreate(32, sizeof(struct rxItem));

    // this will trigger a "IEEE802154 MAC sleep init failed" when called a second time, but it works
    if(zigbee_is_enabled)
    {
        zigbee_is_enabled = false;
        esp_ieee802154_disable();
    }
    zigbee_is_enabled = true;
    esp_ieee802154_enable();
    esp_ieee802154_set_channel(ch);
    // esp_ieee802154_set_txpower(int8_t power);
    esp_ieee802154_set_panid(PROTO_PAN_ID);
    esp_ieee802154_set_promiscuous(false);
    esp_ieee802154_set_coordinator(false);
    esp_ieee802154_set_pending_mode(ESP_IEEE802154_AUTO_PENDING_ZIGBEE);

    // esp_ieee802154_set_extended_address needs the MAC in reversed byte order
    esp_read_mac(mSelfMac, ESP_MAC_IEEE802154);
    uint8_t eui64_rev[8] = {0};
    for (int i = 0; i < 8; i++) {
        eui64_rev[7 - i] = mSelfMac[i];
    }
    esp_ieee802154_set_extended_address(eui64_rev);
    esp_ieee802154_get_extended_address(mSelfMac);

    esp_ieee802154_set_short_address(0xFFFE);
    esp_ieee802154_set_rx_when_idle(true);
    esp_ieee802154_receive();

    led_flash(1);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    led_flash(0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    led_flash(1);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    led_flash(0);

    ESP_LOGI(TAG, "Receiver ready, panId=0x%04x, channel=%d, long=%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x, short=%04x",
             esp_ieee802154_get_panid(), esp_ieee802154_get_channel(),
             mSelfMac[0], mSelfMac[1], mSelfMac[2], mSelfMac[3],
             mSelfMac[4], mSelfMac[5], mSelfMac[6], mSelfMac[7],
             esp_ieee802154_get_short_address());
}

// uint32_t lastZbTx = 0;
bool radioTx(uint8_t *packet) {
#ifdef CONFIG_OEPL_SUBGIG_SUPPORT
// The subghz driver uses DMA 
    static DMA_ATTR uint8_t txPKT[130];
#else
    static uint8_t txPKT[130];
#endif
    led_flash(1);
    radioTxFrames++;
#ifdef CONFIG_OEPL_SUBGIG_SUPPORT
	struct MacFrameNormal  *txHeader = (struct MacFrameNormal *) (packet + 1);

	if(txHeader->pan == PROTO_PAN_ID_SUBGHZ) {
		return SubGig_radioTx(packet);
	}
#endif
	while (isInTransmit) {
	}
	// Only now: the radio reads txPKT while it sends the previous frame. Copying before the wait
	// put the next frame's content into the one still on the air, so a full block never carried
	// part 0 and sent the last part twice.
	memcpy(txPKT, packet, packet[0]);
	isInTransmit = 1;
	esp_ieee802154_transmit(txPKT, false);
	return true;
}

void radioSetChannel(uint8_t ch) {
    // Only the channel: a full radio_init() disabled and re-enabled the radio and blinked the LEDs
    // for 400 ms, during which tags were not answered. Receiving again applies the new channel.
    while (isInTransmit) {
    }
    esp_ieee802154_set_channel(ch);
    esp_ieee802154_receive();
    ESP_LOGW(TAG, "channel %d", ch);
}

void radioSetTxPower(uint8_t power) {}

bool radioTxBusy(void) { return isInTransmit != 0; }

int8_t commsRxUnencrypted(uint8_t *data) {
    static struct rxItem item;
    // Don't block here: the main loop sleeps in ulTaskNotifyTake() and is woken by frames and serial data
    if (xQueueReceive(packet_buffer, (void *)&item, 0) == pdTRUE) {
        lastRxAgeUs = esp_timer_get_time() - item.sfdTime;
        lastRxSfdUs = item.sfdTime;
        lastRxRssi = item.rssi;
        lastRxLqi = item.lqi;
        radioRxFrames++;
        memcpy(data, &item.frame[1], item.frame[0] + 1);
        return item.frame[0] - 2;
    }
#ifdef CONFIG_OEPL_SUBGIG_SUPPORT
    if(gSubGigData.Enabled) {
       int8_t Ret = SubGig_commsRxUnencrypted(data);
       if(Ret > 0) {
          // No SFD timestamp from the CC1101; the packet is read right after its interrupt woke us.
          // Without this, the age of the last 802.15.4 frame was used to skip sub-GHz requests.
          lastRxAgeUs = 0;
          lastRxSfdUs = esp_timer_get_time();
          lastRxRssi = 0;
          lastRxLqi = 0;
          radioRxFrames++;
          return Ret;
        }
    }
#endif
    return 0;
}
