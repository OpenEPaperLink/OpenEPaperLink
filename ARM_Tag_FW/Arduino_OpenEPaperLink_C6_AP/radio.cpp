#include "radio.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_ieee802154.h"
#include "esp_phy_init.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "led.h"
#include "subGhz.h"
#include "proto.h"
#include "sdkconfig.h"
// if you get an error about soc/lp_uart_reg.h not being found,
// you didn't choose the right build target. :-)
#include "soc/lp_uart_reg.h"
#include "soc/uart_struct.h"
#include <esp_mac.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>
#include <Arduino.h>

bool has_sub_ghz = false;

uint8_t mSelfMac[8];
volatile uint8_t isInTransmit = 0;
QueueHandle_t packet_buffer = NULL;

void esp_ieee802154_receive_done(uint8_t *frame, esp_ieee802154_frame_info_t *frame_info) {
  Serial.printf("RADIO info RX %d\r\n", frame[0]);
  BaseType_t xHigherPriorityTaskWoken;
  static uint8_t inner_rxPKT[130];
  memcpy(inner_rxPKT, &frame[0], frame[0] + 1);
  xQueueSendFromISR(packet_buffer, (void *)&inner_rxPKT, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR_ARG(xHigherPriorityTaskWoken);
}

void esp_ieee802154_transmit_failed(const uint8_t *frame, esp_ieee802154_tx_error_t error) {
  isInTransmit = 0;
  Serial.printf("RADIO err TX Err: %d\r\n", error);
}

void esp_ieee802154_transmit_done(const uint8_t *frame, const uint8_t *ack, esp_ieee802154_frame_info_t *ack_frame_info) {
  isInTransmit = 0;
  Serial.printf("RADIO info TX %d\r\n", frame[0]);
}

void radio_init(uint8_t ch) {
  if (packet_buffer == NULL) packet_buffer = xQueueCreate(32, 130);

  // this will trigger a "IEEE802154 MAC sleep init failed" when called a second time, but it works
  esp_ieee802154_enable();
  esp_ieee802154_set_channel(ch);
  // esp_ieee802154_set_txpower(int8_t power);
  esp_ieee802154_set_panid(PROTO_PAN_ID);
  esp_ieee802154_set_promiscuous(false);
  esp_ieee802154_set_coordinator(false);
  esp_ieee802154_set_pending_mode(ESP_IEEE802154_AUTO_PENDING_ZIGBEE);

  // esp_ieee802154_set_extended_address needs the MAC in reversed byte order
  esp_read_mac(mSelfMac, ESP_MAC_IEEE802154);
  uint8_t eui64_rev[8] = { 0 };
  for (int i = 0; i < 8; i++) {
    eui64_rev[7 - i] = mSelfMac[i];
  }
  esp_ieee802154_set_extended_address(eui64_rev);
  esp_ieee802154_get_extended_address(mSelfMac);

  esp_ieee802154_set_short_address(0xFFFE);
  esp_ieee802154_set_rx_when_idle(true);
  esp_ieee802154_receive();

  led_flash(1);
  delay(100);
  led_flash(0);
  delay(100);
  led_flash(1);
  delay(100);
  led_flash(0);

  Serial.printf("RADIO Receiver ready, panId=0x%04x, channel=%d, long=%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x, short=%04x\r\n",
                esp_ieee802154_get_panid(), esp_ieee802154_get_channel(),
                mSelfMac[0], mSelfMac[1], mSelfMac[2], mSelfMac[3],
                mSelfMac[4], mSelfMac[5], mSelfMac[6], mSelfMac[7],
                esp_ieee802154_get_short_address());


  // Lets here take care of the SubGhz Init
  if (!init_subGhz())
    Serial.printf("Sub-GHz radio init failed\r\n");
  else if (!tiRadioSetChannel(ch))
    Serial.printf("SubGHz radio channel fail\r\n");
  else
    has_sub_ghz = true;

  Serial.printf("SubGhz %s\r\n", has_sub_ghz ? "Active" : "Not Found");
  if (has_sub_ghz) {
    tiRadioRxFilterCfg(mSelfMac, SHORT_MAC_UNUSED, PROTO_PAN_ID, true);
    tiRadioTxConfigure(mSelfMac, SHORT_MAC_UNUSED, PROTO_PAN_ID);
    tiRadioRxEnable(true, false);
  }
}

// uint32_t lastZbTx = 0;
bool radioTx(uint8_t *packet, bool subGhz) {
  led_flash(1);
  if (has_sub_ghz && subGhz) {
    tiRadioTxLL(packet);
  } else {
    static uint8_t txPKT[130];
    while (isInTransmit) {
    }
    // while (millis() - lastZbTx < 6) {
    // }
    // lastZbTx = millis();
    memcpy(txPKT, packet, packet[0]);
    isInTransmit = 1;
    esp_ieee802154_transmit(txPKT, false);
    return true;
  }
}

void radioSetChannel(uint8_t ch) {
  radio_init(ch);
  if (has_sub_ghz)
    tiRadioSetChannel(ch);
}

void radioSetTxPower(uint8_t power) {}

int8_t commsRxUnencrypted(uint8_t *data, bool *subGhzRx) {
  int8_t rssi_sub_rx = 0;
  uint8_t lqi_sub_rx = 0;

  static uint8_t inner_rxPKT_out[135];
  if (xQueueReceive(packet_buffer, (void *)&inner_rxPKT_out, pdMS_TO_TICKS(100)) == pdTRUE) {
    memcpy(data, &inner_rxPKT_out[1], inner_rxPKT_out[0] + 1);
    *subGhzRx = false; // This is Normal data
    return inner_rxPKT_out[0] - 2;
  }
  if (has_sub_ghz) {
    int32_t ret_sub_rx_len = tiRadioRxDequeuePkt(inner_rxPKT_out, sizeof(inner_rxPKT_out), &rssi_sub_rx, &lqi_sub_rx);
    if (ret_sub_rx_len > 0)
    {
      //Serial.printf("Got Sub Ghz Len %i data: %i %u\r\n", ret_sub_rx, rssi_sub_rx, lqi_sub_rx);
      memcpy(data, inner_rxPKT_out, ret_sub_rx_len);
      *subGhzRx = true; // This is SubGHz data
      return ret_sub_rx_len;
    }
  }
  return 0;
}
