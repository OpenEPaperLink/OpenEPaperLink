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
#include "soc/lp_uart_reg.h"
#include "soc/uart_struct.h"
#include "utils.h"
#include <esp_mac.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>

static const char *TAG = "RADIO";

uint8_t mSelfMac[8];
volatile uint8_t isInTransmit = 0;
QueueHandle_t packet_buffer = NULL;

void esp_ieee802154_receive_done(uint8_t *frame, esp_ieee802154_frame_info_t *frame_info) {
    ESP_EARLY_LOGI(TAG, "RX <- : %d", frame[0]);
    BaseType_t xHigherPriorityTaskWoken;
    static uint8_t inner_rxPKT[130];
    memcpy(inner_rxPKT, &frame[0], frame[0] + 1);
    xQueueSendFromISR(packet_buffer, (void *)&inner_rxPKT, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR_ARG(xHigherPriorityTaskWoken);
}

void esp_ieee802154_transmit_failed(const uint8_t *frame, esp_ieee802154_tx_error_t error) {
    isInTransmit = 0;
    ESP_EARLY_LOGE(TAG, "TX Err: %d", error);
}

void esp_ieee802154_transmit_done(const uint8_t *frame, const uint8_t *ack, esp_ieee802154_frame_info_t *ack_frame_info) {
    isInTransmit = 0;
    ESP_EARLY_LOGI(TAG, "TX -> : %d", frame[0]);
}

void radio_init() {
    packet_buffer = xQueueCreate(32, 130);
    esp_ieee802154_enable();
    radioSetChannel(11);
    esp_ieee802154_set_panid(PROTO_PAN_ID);
    esp_ieee802154_set_promiscuous(false); // Filter for our mac and PAN
    esp_ieee802154_set_coordinator(false);
    esp_ieee802154_set_pending_mode(ESP_IEEE802154_AUTO_PENDING_ZIGBEE);
    esp_read_mac(mSelfMac, ESP_MAC_IEEE802154);
    esp_ieee802154_set_extended_address(mSelfMac);
    esp_ieee802154_set_short_address(0xFFFE);
    esp_ieee802154_set_rx_when_idle(true);
    esp_ieee802154_receive();
}

uint32_t lastZbTx = 0;
bool radioTx(uint8_t *packet) {
    static uint8_t txPKT[130];
    while (isInTransmit) {
    }
    while (getMillis() - lastZbTx < 6) {
    }
    led_flash(1);
    memcpy(txPKT, packet, packet[0]);
    isInTransmit = 1;
    lastZbTx = getMillis();
    esp_ieee802154_transmit(txPKT, false);
    return true;
}

void radioSetChannel(uint8_t ch) { esp_ieee802154_set_channel(ch); }

void radioSetTxPower(uint8_t power) {}

int8_t commsRxUnencrypted(uint8_t *data) {
    static uint8_t inner_rxPKT_out[130];
    if (xQueueReceive(packet_buffer, (void *)&inner_rxPKT_out, pdMS_TO_TICKS(100)) == pdTRUE) {
        memcpy(data, &inner_rxPKT_out[1], inner_rxPKT_out[0] + 1);
        return inner_rxPKT_out[0] - 2;
    }
    return 0;
}
