#ifndef _BOARDHEADER_H_
#define _BOARDHEADER_H_

#include "../../../../tag_types.h"
#include "HAL_Newton_M3.h"


// Mac fixed part
// 062E4793B899 (B29)
#define MAC_ID_0 0xB8
#define MAC_ID_1 0x90  // FIX FOR THIS TYPE!!

// AP mode definitions
#define HAS_EEPROM 1
#define HAS_SCREEN 1
#define AP_EMULATE_TAG 1

// hw type
#define HW_TYPE SOLUM_M3_BWR_60

#define SCREEN_WIDTH 600    
#define SCREEN_HEIGHT 448
#define SCREEN_XOFFSET 0
#define SCREEN_YOFFSET 0

#define EEPROM_IMG_EACH (0x11000UL)

#endif
