#ifndef SYNCED_H
#define SYNCED_H

#include <stdint.h>


extern void initRadio();
extern void killRadio();

extern uint8_t __xdata mSelfMac[];
extern struct AvailDataInfo *__xdata getAvailDataInfo();
extern bool doDataDownload(struct AvailDataInfo *__xdata avail);
extern void initializeProto();
extern struct AvailDataInfo *__xdata getAvailDataInfo();
bool probeChannel(uint8_t channel);
#endif