#ifndef _BOARD_H_
#define _BOARD_H_

#include <stdint.h>
#include "proto.h"
#include "main.h"

#define eepromByte				spiByte
#define eepromPrvSelect()		do { digitalWrite(FLASH_CS,LOW); } while(0)
#define eepromPrvDeselect()		do { digitalWrite(FLASH_CS,HIGH); } while(0)

//eeprom map
#define EEPROM_SETTINGS_AREA_START		(0x01000UL)
#define EEPROM_SETTINGS_AREA_LEN		(0x03000UL)
#define EEPROM_UPDATA_AREA_START		(0x04000UL)
#define EEPROM_UPDATE_AREA_LEN			(0x10000UL)
#define EEPROM_IMG_START				(0x14000UL)
#define EEPROM_IMG_EACH					(0x08000UL)
//till end of eeprom really. do not put anything after - it will be erased at pairing time!!!
#define EEPROM_PROGRESS_BYTES			(128)


#define HAS_EEPROM 1
#define HAS_SCREEN 1
#define AP_EMULATE_TAG 1

//hw types
#define HW_TYPE					        SOLUM_29_SSD1619_HIGH_RES

//#include "../boardCommon.h"


#endif
