#ifndef SYNCED_H
#define SYNCED_H

#include <stdint.h>
//#include "settings.h"

extern uint8_t __xdata mSelfMac[];
extern uint8_t __xdata currentChannel;
extern uint8_t __xdata APmac[];

extern uint8_t __xdata curImgSlot;

extern void setupRadio(void);
extern void killRadio(void);


extern struct AvailDataInfo *__xdata getAvailDataInfo();
extern struct AvailDataInfo *__xdata getShortAvailDataInfo();
extern void drawImageFromEeprom(const uint8_t imgSlot);
extern bool processAvailDataInfo(struct AvailDataInfo *__xdata avail);
extern void initializeProto();
extern uint8_t detectAP(const uint8_t channel);


extern uint8_t __xdata eventDataID;
extern bool __xdata eventModeActive;
struct eventData *__xdata getEventData();
void eventAPMode();

#endif