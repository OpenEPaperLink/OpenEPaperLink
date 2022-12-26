#ifndef _SLEEP_H_
#define _SLEEP_H_

#include <stdint.h>


void sleepForMsec(uint32_t msec);
void sleepTillInt(void);	//assumes you left only one int enabled!

#endif
