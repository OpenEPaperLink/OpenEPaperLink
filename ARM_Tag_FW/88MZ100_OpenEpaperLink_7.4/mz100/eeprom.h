#ifndef _EEPROM_H_
#define _EEPROM_H_

#include <stdint.h>

#define EEPROM_WRITE_PAGE_SZ		256		//max write size & alignment
#define EEPROM_ERZ_SECTOR_SZ		4096	//erase size and alignment
//pages are 4K in size
//an update can be stored in any 2 image slots

#define EEPROM_SETTINGS_AREA_START		(0x7F000UL)
#define EEPROM_SETTINGS_SIZE    		(0x01000UL)

#define EEPROM_TOTAL_SIZE               (0x80000UL)

#define EEPROM_MAC_INFO_START		(0x6c000UL)	//not same as stock
#define EEPROM_MAC_INFO_LEN			(0x01000UL)

#define EEPROM_IMG_VALID			(0x494d4721)

//#define EEPROM_PIECE_SZ				(88)

#include "../../common/eeprom_struct.h"

#endif
