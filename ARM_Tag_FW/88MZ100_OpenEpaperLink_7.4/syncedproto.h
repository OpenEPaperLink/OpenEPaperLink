#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "settings.h"

extern __attribute__((section(".aonshadow"))) uint8_t mSelfMac[];
extern __attribute__((section(".aonshadow"))) volatile uint8_t currentChannel;
extern __attribute__((section(".aonshadow"))) struct blockRequest curBlock;                                       // used by the block-requester, contains the next request that we'll send
extern __attribute__((section(".aonshadow"))) struct AvailDataInfo curDataInfo;  // last 'AvailDataInfo' we received from the AP // __attribute__((section(".aon")))


extern uint8_t APmac[];

extern __attribute__((section(".aonshadow"))) uint8_t curImgSlot;

extern void setupRadio(void);
extern void killRadio(void);

extern struct AvailDataInfo *getAvailDataInfo();
extern struct AvailDataInfo *getShortAvailDataInfo();
extern void drawImageFromEeprom(const uint8_t imgSlot);
extern bool processAvailDataInfo(struct AvailDataInfo *avail);
extern void initializeProto();
extern uint8_t detectAP(const uint8_t channel);
