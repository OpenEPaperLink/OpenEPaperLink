#ifndef _BOARD_H_
#define _BOARD_H_

#include <stdint.h>
#include "proto.h"
#include "main.h"

//eeprom map
#define EEPROM_SETTINGS_AREA_START		(0x40000UL)
#define EEPROM_SETTINGS_AREA_LEN		(0x03000UL)

#define EEPROM_UPDATA_AREA_START		(0x43000UL)
#define EEPROM_UPDATE_AREA_LEN			(0x20000UL)

#define EEPROM_IMG_START				(0x43000UL)
#define EEPROM_IMG_EACH					(0x7000UL)
#define EEPROM_IMG_LEN					(0x20000UL)

#define EEPROM_PAGE_SIZE			(0x01000)
//till end of eeprom really. do not put anything after - it will be erased at pairing time!!!
#define EEPROM_PROGRESS_BYTES			(128)


#define HAS_EEPROM 1
#define HAS_SCREEN 1
#define AP_EMULATE_TAG 1

//hw types
#define HW_TYPE					        0x60

#endif
