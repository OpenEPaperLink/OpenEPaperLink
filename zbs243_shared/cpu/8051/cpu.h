#ifndef _CPUi_H_
#define _CPUi_H_

#include <stdint.h>


typedef uint16_t uintptr_near_t;

#define VERSIONMARKER	__at (0x008b)

#define irqsOn()		IEN_EA = 1


#include "soc.h"



#endif
