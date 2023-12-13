#ifndef _BOARDHEADER_H_
#define _BOARDHEADER_H_

#include "../../../../tag_types.h"
#include "HAL_Newton_M3.h"


#define EEPROM_IMG_EACH (0x05000UL)

// Mac fixed part
// 06F66008B7D0 (B7D)
#define MAC_ID_0 0xB7
#define MAC_ID_1 0xD0 

// AP mode definitions
#define HAS_EEPROM 1
#define HAS_SCREEN 1
#define AP_EMULATE_TAG 1

// hw type
#define HW_TYPE SOLUM_M3_BWR_43

#define SCREEN_WIDTH 152    
#define SCREEN_HEIGHT 522

#define SCREEN_XOFFSET 0
#define SCREEN_YOFFSET 0

#define EPD_MIRROR_H
#define EPD_DRAW_DIRECTION_RIGHT

#define NO_BUTTONS 1

#endif
