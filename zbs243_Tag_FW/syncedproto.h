#ifndef SYNCED_H
#define SYNCED_H

#include <stdint.h>
#include "settings.h"

extern uint8_t __xdata mSelfMac[];
extern uint8_t __xdata currentChannel;
extern uint8_t __xdata APmac[];

extern uint8_t __xdata curImgSlot;

extern void setupRadio(void);
extern void killRadio(void);


#ifdef ENABLE_RETURN_DATA
extern bool sendTagReturnData(uint8_t __xdata *data, uint8_t len, uint8_t type);
#endif

void dump(const uint8_t *__xdata a, const uint16_t __xdata l);
extern struct AvailDataInfo *__xdata getAvailDataInfo();
extern struct AvailDataInfo *__xdata getShortAvailDataInfo();
extern void drawImageFromEeprom(const uint8_t imgSlot);
extern bool processAvailDataInfo(struct AvailDataInfo *__xdata avail);
extern void initializeProto();
extern uint8_t detectAP(const uint8_t channel);

#endif