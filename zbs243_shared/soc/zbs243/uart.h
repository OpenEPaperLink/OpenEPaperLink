#ifndef _UART_ZBS_H_
#define _UART_ZBS_H_

#include <stdint.h>

#pragma callee_saves uartInit
void uartInit(void);

#pragma callee_saves uartTx
void uartTx(uint8_t val);

void UART_IRQ1(void) __interrupt (0);

#pragma callee_saves uartBytesAvail
uint8_t uartBytesAvail(void);

#pragma callee_saves uartRX
uint8_t uartRx();

void UartTxWait();

#endif

