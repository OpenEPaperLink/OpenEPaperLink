#ifndef _BOARD_H_
#define _BOARD_H_

#include <stdint.h>
#include "proto.h"
#include "spi.h"

#define eepromByte				spiByte
#define eepromPrvSelect()		do { __asm__("nop\nnop\nnop\n"); P1_1 = 0; __asm__("nop\nnop\nnop\n"); } while(0)
#define eepromPrvDeselect()		do { __asm__("nop\nnop\nnop\n"); P1_1 = 1; __asm__("nop\nnop\nnop\n"); } while(0)

//eeprom map
#define EEPROM_SETTINGS_AREA_START		(0x01000UL)
#define EEPROM_SETTINGS_AREA_LEN		(0x03000UL)
#define EEPROM_UPDATA_AREA_START		(0x04000UL)
#define EEPROM_UPDATE_AREA_LEN			(0x10000UL)
#define EEPROM_IMG_START				(0x14000UL)
#define EEPROM_IMG_EACH					(0x04000UL)
//till end of eeprom really. do not put anything after - it will be erased at pairing time!!!
#define EEPROM_PROGRESS_BYTES			(128)

#define HAS_EEPROM 1
#define HAS_SCREEN 1
#define NFC_TYPE 1
#define AP_EMULATE_TAG 1

//hw types
#define HW_TYPE					        SOLUM_29_SSD1619

#include "../boardCommon.h"


#endif
