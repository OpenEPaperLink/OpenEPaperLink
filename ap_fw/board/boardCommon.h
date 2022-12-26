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

#endif
