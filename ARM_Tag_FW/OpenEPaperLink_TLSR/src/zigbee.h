#pragma once

#include <stdint.h>
#include <stdbool.h>
extern uint8_t channelList[6];
void init_zigbee(uint_fast8_t channel);
uint8_t getRxBuffer(uint8_t *bufferout, uint8_t *rssiout);
void zigbee_tx(uint8_t *buf, uint8_t len);
void zigbee_off();
void zigbee_rx_start();

bool radioSetChannel(uint_fast8_t channel);
bool radioRxEnable(bool on);
void radioRxFlush(void);
bool radioInit(void);
int32_t radioRxDequeuePkt(uint8_t *dstBuf, uint32_t maxLen, int8_t *rssiP, uint8_t *lqiP);
bool radioTxLL(uint8_t *pkt);