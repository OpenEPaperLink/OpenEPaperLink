#ifndef _UART_ZBS_H_
#define _UART_ZBS_H_

#include <stdint.h>

//pre-configured for 115200 8n1
//RX can be done but i do not need it

#pragma callee_saves uartInit
void uartInit(void);

#pragma callee_saves uartTx
void uartTx(uint8_t val);



#endif

