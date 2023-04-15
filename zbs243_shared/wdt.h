#ifndef _WDT_H_
#define _WDT_H_

#include <stdint.h>


#pragma callee_saves wdtOn
void wdtOn(void);

#pragma callee_saves wdtOff
void wdtOff(void);

#pragma callee_saves wdtPet
void wdtPet(void);

#pragma callee_saves wdtSetResetVal
void wdtSetResetVal(uint32_t val);		//speed is CPU-specific. On ZBS it is 62KHz or so

#pragma callee_saves wdtDeviceReset
void wdtDeviceReset(void);

#define wdt10s()                    \
    do {                            \
        wdtSetResetVal(0xFFF68A1F); \
        wdtOn();                    \
    } while (0)

#define wdt30s()                    \
    do {                            \
        wdtSetResetVal(0xFFE39E5F); \
        wdtOn();                    \
    } while (0)

#define wdt60s()                    \
    do {                            \
        wdtSetResetVal(0xFFC73CBF); \
        wdtOn();                    \
    } while (0)


#define wdt120s()                    \
    do {                            \
        wdtSetResetVal(0xFF8E797F); \
        wdtOn();                    \
    } while (0)

#endif
