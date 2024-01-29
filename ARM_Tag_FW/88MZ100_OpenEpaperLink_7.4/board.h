#ifndef _BOARD_H_
#define _BOARD_H_

#include <stdint.h>
#include "proto.h"
#include "main.h"
#include "mz100/gpio.h"

#define eepromByte				spiByte
#define eepromPrvSelect()		do { /*digitalWrite(FLASH_CS,LOW);*/ } while(0)
#define eepromPrvDeselect()		do { /*digitalWrite(FLASH_CS,HIGH);*/ } while(0)

//till end of eeprom really. do not put anything after - it will be erased at pairing time!!!
#define EEPROM_PROGRESS_BYTES			(128)


#define HAS_EEPROM 1
#define HAS_SCREEN 1
#define AP_EMULATE_TAG 1

//hw types
#define HW_TYPE					        5

//#include "../boardCommon.h"


#endif
