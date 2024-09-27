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
//#include "soc/lp_uart_reg.h"
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
void esp_ieee802154_receive_done(uint8_t *frame, esp_ieee802154_frame_info_t *frame_info) {
    ESP_EARLY_LOGI(TAG, "RX %d", frame[0]);
    BaseType_t xHigherPriorityTaskWoken;
    static uint8_t inner_rxPKT[130];
    memcpy(inner_rxPKT, &frame[0], frame[0] + 1);
    xQueueSendFromISR(packet_buffer, (void *)&inner_rxPKT, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR_ARG(xHigherPriorityTaskWoken);
    esp_ieee802154_receive_sfd_done();
}

void esp_ieee802154_transmit_failed(const uint8_t *frame, esp_ieee802154_tx_error_t error) {
    isInTransmit = 0;
    ESP_EARLY_LOGE(TAG, "TX Err: %d", error);
}

void esp_ieee802154_transmit_done(const uint8_t *frame, const uint8_t *ack, esp_ieee802154_frame_info_t *ack_frame_info) {
    isInTransmit = 0;
    ESP_EARLY_LOGI(TAG, "TX %d", frame[0]);
    esp_ieee802154_receive_sfd_done();
}
static bool zigbee_is_enabled = false;
void radio_init(uint8_t ch) {
    if (packet_buffer == NULL) packet_buffer = xQueueCreate(32, 130);

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
	// while (getMillis() - lastZbTx < 6) {
	// }
	// lastZbTx = getMillis();
	memcpy(txPKT, packet, packet[0]);
#ifdef CONFIG_OEPL_SUBGIG_SUPPORT
	struct MacFrameNormal  *txHeader = (struct MacFrameNormal *) (packet + 1);

	if(txHeader->pan == PROTO_PAN_ID_SUBGHZ) {
		return SubGig_radioTx(packet);
	}
#endif
	while (isInTransmit) {
	}
	isInTransmit = 1;
	esp_ieee802154_transmit(txPKT, false);
	return true;
}

void radioSetChannel(uint8_t ch) {
    radio_init(ch);
}

void radioSetTxPower(uint8_t power) {}

int8_t commsRxUnencrypted(uint8_t *data) {
    static uint8_t inner_rxPKT_out[130];
    if (xQueueReceive(packet_buffer, (void *)&inner_rxPKT_out, pdMS_TO_TICKS(100)) == pdTRUE) {
        memcpy(data, &inner_rxPKT_out[1], inner_rxPKT_out[0] + 1);
        return inner_rxPKT_out[0] - 2;
    }
#ifdef CONFIG_OEPL_SUBGIG_SUPPORT
    if(gSubGigData.Enabled) {
       int8_t Ret = SubGig_commsRxUnencrypted(data);
       if(Ret > 0) {
          return Ret;
        }
    }
#endif
    return 0;
}
