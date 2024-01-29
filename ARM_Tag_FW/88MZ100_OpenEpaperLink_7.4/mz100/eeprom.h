#ifndef _EEPROM_H_
#define _EEPROM_H_

#include <stdint.h>

#define EEPROM_WRITE_PAGE_SZ		256		//max write size & alignment
#define EEPROM_ERZ_SECTOR_SZ		4096	//erase size and alignment
//pages are 4K in size
//an update can be stored in any 2 image slots

#define EEPROM_PAGE_SIZE			(0x01000UL)

#define EEPROM_OS_START				(0x00000UL)
#define EEPROM_OS_LEN				(0x1FFFFUL)	//0xE820 of image, rounded up to 4K

#define EEPROM_IMG_START			(0x20000UL)
#define EEPROM_IMG_EACH				(0x1F000UL)
#define EEPROM_IMG_LEN				(EEPROM_IMG_START + 0x13FFFUL)

#define EEPROM_UPDATE_START			(0x20000UL)	//same header as images
#define EEPROM_UPDATE_LEN			(0x1FFFFUL)

#define EEPROM_SETTINGS_AREA_START		(0x14000UL)
#define EEPROM_SETTINGS_AREA_LEN		(0x03000UL)

#define EEPROM_MAC_INFO_START		(0x6c000UL)	//not same as stock
#define EEPROM_MAC_INFO_LEN			(0x01000UL)


#define EEPROM_IMG_INPROGRESS		(0x7fffffff)
#define EEPROM_IMG_VALID			(0x494d4721)


//#define EEPROM_PIECE_SZ				(88)

#include "../../common/eeprom_struct.h"

#endif
