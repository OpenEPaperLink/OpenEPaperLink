#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define RAW_PKT_PADDING 2
extern uint8_t mSelfMac[8];
extern TaskHandle_t radioWakeTask;
extern int64_t lastRxAgeUs;
extern int64_t lastRxSfdUs;
extern int8_t lastRxRssi;
extern uint8_t lastRxLqi;
extern uint32_t radioRxFrames;
extern uint32_t radioTxFrames;

void radio_init(uint8_t ch);
bool radioTx(uint8_t *packet);
void radioSetChannel(uint8_t ch);
void radioSetTxPower(uint8_t power);
bool radioTxBusy(void);
int8_t commsRxUnencrypted(uint8_t *data);

#ifdef SUBGIG_SUPPORT
void SubGig_radio_init(uint8_t ch);
bool SubGig_radioTx(uint8_t *packet);
void SubGig_radioSetChannel(uint8_t ch);
void SubGig_radioSetTxPower(uint8_t power);
int8_t SubGig_commsRxUnencrypted(uint8_t *data);
#endif