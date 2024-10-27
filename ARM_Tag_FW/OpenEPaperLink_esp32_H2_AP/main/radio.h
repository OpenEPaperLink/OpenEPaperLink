#pragma once
#include <stdbool.h>
#include <stdint.h>

#define RAW_PKT_PADDING 2
extern uint8_t mSelfMac[8];

void radio_init(uint8_t ch);
bool radioTx(uint8_t *packet);
void radioSetChannel(uint8_t ch);
void radioSetTxPower(uint8_t power);
int8_t commsRxUnencrypted(uint8_t *data);

#ifdef SUBGIG_SUPPORT
void SubGig_radio_init(uint8_t ch);
bool SubGig_radioTx(uint8_t *packet);
void SubGig_radioSetChannel(uint8_t ch);
void SubGig_radioSetTxPower(uint8_t power);
int8_t SubGig_commsRxUnencrypted(uint8_t *data);
#endif