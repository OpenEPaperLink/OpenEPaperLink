#pragma once

extern uint8_t channelList[6];

void radioPrvDisable(void);
bool radioInit(void);
bool radioRxEnable(bool on);
void send_broadcast(uint8_t msg[], int len);
void send_msg(uint8_t dest_address[], uint8_t msg[], int len);

int32_t radioRxDequeuePkt(void *dstBuf, uint32_t maxLen, int8_t *rssiP, uint8_t *lqiP);
void radioRxFlush(void);
bool radioTxLL(const void *pkt);
bool radioSetChannel(uint_fast8_t channel);