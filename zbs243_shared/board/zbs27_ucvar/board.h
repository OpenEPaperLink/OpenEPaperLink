#ifndef _BOARD_H_
#define _BOARD_H_

#include <stdint.h>
#include "spi.h"

#define __packed
#include "../oepl-definitions.h"


#define eepromByte				spiByte
#define eepromPrvSelect()		do { __asm__("nop\nnop\nnop\n"); P1_1 = 0; __asm__("nop\nnop\nnop\n"); } while(0)
#define eepromPrvDeselect()		do { __asm__("nop\nnop\nnop\n"); P1_1 = 1; __asm__("nop\nnop\nnop\n"); } while(0)

//eeprom map
#define EEPROM_SETTINGS_AREA_START		(0x00000UL)
#define EEPROM_SETTINGS_AREA_LEN		(0x01000UL)
#define EEPROM_IMG_START				(0x01000UL)
#define EEPROM_IMG_EACH					(0x03000UL)

#define HAS_EEPROM 1
#define HAS_SCREEN 1
#define NFC_TYPE 1
#define AP_EMULATE_TAG 1


//hw types
#define HW_TYPE					        SOLUM_M2_BWR_27

#include "../boardCommon.h"


#endif
