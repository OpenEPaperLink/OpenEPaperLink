#ifndef SYNCED_H
#define SYNCED_H

#include <stdint.h>


extern uint8_t __xdata mSelfMac[];
extern struct AvailDataInfo *__xdata getAvailDataInfo();
extern bool doDataDownload(struct AvailDataInfo *__xdata avail);
extern void initializeProto();


#endif