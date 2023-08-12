#pragma once
#include <stdlib.h>

extern uint8_t channelList[6];

void ZIGBEE_set_filter(uint16_t dest_pan, uint16_t source_addr, uint32_t own_mac[], uint8_t rx_filter);
void radioSetTxPower(char result);
void RF_init(uint8_t channel);
uint8_t Zigbee_tx_buffer(uint8_t *tx_buffer, int len);

void radioInit();
void radioSetChannel(uint8_t channel);
void radioRxEnable(uint8_t channel);
void radioRxFlush();

struct zigbeeCalibDataStruct {
    uint16_t len;
    bool isValid;
    uint32_t data[30];
};
extern __attribute__((section(".aon"))) volatile struct zigbeeCalibDataStruct zigbeeCalibData;
