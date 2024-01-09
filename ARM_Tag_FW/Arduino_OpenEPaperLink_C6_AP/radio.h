#pragma once
#include <stdbool.h>
#include <stdint.h>

extern uint8_t mSelfMac[8];

void radio_init(uint8_t ch);
bool radioTx(uint8_t *packet, bool subGhz);
void radioSetChannel(uint8_t ch);
void radioSetTxPower(uint8_t power);
int8_t commsRxUnencrypted(uint8_t *data, bool *subGhzRx);
