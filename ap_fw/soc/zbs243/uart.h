#ifndef _UART_ZBS_H_
#define _UART_ZBS_H_

#include <stdint.h>

//pre-configured for 115200 8n1
//RX can be done but i do not need it

#pragma callee_saves uartInit
void uartInit(void);

#pragma callee_saves uartTx
void uartTx(uint8_t val);

void UART_IRQ1(void) __interrupt (0);

#pragma callee_saves uartBytesAvail
uint8_t uartBytesAvail(void);

#pragma callee_saves uartRX
uint8_t uartRx();


#endif

