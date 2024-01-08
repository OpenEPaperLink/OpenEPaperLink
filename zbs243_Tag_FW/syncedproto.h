#ifndef SYNCED_H
#define SYNCED_H

#include <stdint.h>
#include "settings.h"

extern uint8_t __xdata mSelfMac[];
extern uint8_t __xdata currentChannel;
extern uint8_t __xdata APmac[];

extern uint8_t __xdata curImgSlot;
extern bool __xdata fastNextCheckin;


//extern void setupRadio(void);
//extern void killRadio(void);


#ifdef ENABLE_RETURN_DATA
extern bool sendTagReturnData(uint8_t __xdata *data, uint8_t len, uint8_t type);
#endif

extern void dump(const uint8_t *__xdata a, const uint16_t __xdata l);
extern bool checkCRC(const void *p, const uint8_t len);
extern bool validateBlockData();

extern uint8_t __xdata findSlotDataTypeArg(uint8_t arg) __reentrant;
extern uint8_t __xdata findNextSlideshowImage(uint8_t start) __reentrant;
extern uint8_t getEepromImageDataArgument(const uint8_t slot);

extern struct AvailDataInfo *__xdata getAvailDataInfo();
extern struct AvailDataInfo *__xdata getShortAvailDataInfo();

extern void drawImageFromEeprom(const uint8_t imgSlot, uint8_t lut);
extern void eraseImageBlocks();
extern bool processAvailDataInfo(struct AvailDataInfo *__xdata avail);
extern void initializeProto();
extern uint8_t detectAP(const uint8_t channel);

extern bool validateMD5(uint32_t __xdata addr, uint16_t __xdata len);
extern bool validateFWMagic();

#endif