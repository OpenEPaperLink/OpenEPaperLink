#ifndef _SPI_ZBS_H_
#define _SPI_ZBS_H_

#include <stdint.h>

//pre-configured for 4mhz mode 0, but can be changed

#pragma callee_saves spiInit
void spiInit(void);

#pragma callee_saves spiByte
uint8_t spiByte(uint8_t val);



#endif
