#ifndef SYNCED_H
#define SYNCED_H

#include <stdint.h>


extern uint8_t __xdata mSelfMac[8];
extern uint8_t __xdata currentChannel;
extern uint8_t __xdata APmac[];

extern void initRadio();
extern void killRadio();


extern struct AvailDataInfo *__xdata getAvailDataInfo();
extern bool doDataDownload(struct AvailDataInfo *__xdata avail);
extern void initializeProto();
extern struct AvailDataInfo *__xdata getAvailDataInfo();
uint8_t detectAP(uint8_t channel);

#endif