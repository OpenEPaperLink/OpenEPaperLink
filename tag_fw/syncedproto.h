#ifndef SYNCED_H
#define SYNCED_H

#include <stdint.h>
//#include "settings.h"

extern uint8_t __xdata mSelfMac[];
extern uint8_t __xdata currentChannel;
extern uint8_t __xdata APmac[];

extern void setupRadio(void);
extern void killRadio(void);


extern struct AvailDataInfo *__xdata getAvailDataInfo();
extern struct AvailDataInfo *__xdata getShortAvailDataInfo();
extern bool doDataDownload(struct AvailDataInfo *__xdata avail);
extern void initializeProto();
extern uint8_t detectAP(uint8_t channel);

#endif