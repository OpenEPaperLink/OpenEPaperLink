#ifndef _BOARD_COMMON_H_
#define _BOARD_COMMON_H_

#include <stdint.h>

#pragma callee_saves powerPortsDownForSleep
void powerPortsDownForSleep(void);

#pragma callee_saves boardInit
void boardInit(void);

//late, after eeprom
#pragma callee_saves boardInit
__bit boardGetOwnMac(uint8_t __xdata *mac);


#ifndef AP_FW
//some sanity checks
#include "eeprom.h"

#endif

#endif