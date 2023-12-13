#pragma once

#include <stdint.h>
#include "settings.h"

extern uint8_t mSelfMac[];
extern uint8_t currentChannel;
extern uint8_t APmac[];

extern uint8_t curImgSlot;

extern void setupRadio(void);
extern void killRadio(void);

extern uint8_t findSlotDataTypeArg(uint8_t arg);
extern uint8_t findNextSlideshowImage(uint8_t start);
extern uint8_t getEepromImageDataArgument(const uint8_t slot);

extern struct AvailDataInfo *getAvailDataInfo();
extern struct AvailDataInfo *getShortAvailDataInfo();

extern void drawImageFromEeprom(const uint8_t imgSlot, uint8_t lut);
extern void eraseImageBlocks();
extern bool processAvailDataInfo(struct AvailDataInfo *avail);
extern void initializeProto();
extern uint8_t detectAP(const uint8_t channel);
