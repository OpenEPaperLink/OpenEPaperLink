#ifndef _BOARD_COMMON_H_
#define _BOARD_COMMON_H_

#include <stdint.h>

#pragma callee_saves powerPortsDownForSleep
void powerPortsDownForSleep(void);

#pragma callee_saves boardInit
void boardInit(void);

//early - before most things
#pragma callee_saves boardInitStage2
void boardInitStage2(void);

//late, after eeprom
#pragma callee_saves boardInit
__bit boardGetOwnMac(uint8_t __xdata *mac);


//some sanity checks
#include "eeprom.h"


#if !EEPROM_SETTINGS_AREA_START
	#error "settings cannot be at address 0"
#endif

#if (EEPROM_SETTINGS_AREA_LEN % EEPROM_ERZ_SECTOR_SZ) != 0
	#error "settings area must be an integer number of eeprom blocks"
#endif

#if (EEPROM_SETTINGS_AREA_START % EEPROM_ERZ_SECTOR_SZ) != 0
	#error "settings must begin at an integer number of eeprom blocks"
#endif

#if (EEPROM_IMG_EACH % EEPROM_ERZ_SECTOR_SZ) != 0
	#error "each image must be an integer number of eeprom blocks"
#endif

#if (EEPROM_IMG_START % EEPROM_ERZ_SECTOR_SZ) != 0
	#error "images must begin at an integer number of eeprom blocks"
#endif

#if (EEPROM_UPDATE_AREA_LEN % EEPROM_ERZ_SECTOR_SZ) != 0
	#error "update must be an integer number of eeprom blocks"
#endif

#if (EEPROM_UPDATA_AREA_START % EEPROM_ERZ_SECTOR_SZ) != 0
	#error "images must begin at an integer number of eeprom blocks"
#endif






#endif
