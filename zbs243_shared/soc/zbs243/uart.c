#include "uart.h"

#include "cpu.h"

#include "stdbool.h"
// #include "string.h"

void uartInit(void) {
    // clock it up
    CLKEN |= 0x20;
    // configure baud rate
    UARTBRGH = 0x00;

    // UARTBRGL = 69;  // nice. 230400 baud
    // UARTBRGL = 70;  // 79 == 200k
    // UARTSTA = 0x12;  // also set the "empty" bit else we wait forever for it to go up
    UARTBRGL = 0x8A;  // config for 115200
    UARTSTA = 0x10;   // clear the register, don't trigger the interrupt just yet
    IEN_UART0 = 1;
}

extern uint8_t __xdata blockbuffer[];

volatile uint8_t __xdata txtail = 0;
volatile uint8_t __xdata txhead = 0;
uint8_t __xdata txbuf[256] = {0};

volatile uint8_t __idata rxtail = 0;
volatile uint8_t __idata rxhead = 0;
uint8_t __xdata rxbuf[256] = {0};

void UartTxWait() {
    while (txhead != txtail) {
    }
}

void uartTx(uint8_t val) {
   while(txhead+1 == txtail){

    }
    __critical {
        txbuf[txhead] = val;
        if (txhead == txtail) {
            UARTBUF = val;
        }
        txhead++;
    }
}

uint8_t uartRx() {
    if (rxhead == rxtail) {
        return 0;
    } else {
        uint8_t ret = rxbuf[rxtail];
        rxtail++;
        return ret;
    }
}

uint8_t uartBytesAvail() {
    return rxhead - rxtail;
}

uint8_t* __idata blockp;
volatile bool __idata serialBypassActive = false;

void UART_IRQ1(void) __interrupt(0) {
    if (UARTSTA & 1) {  // RXC
        UARTSTA &= 0xfe;
        if (serialBypassActive) {
            *blockp++ = 0xAA ^ UARTBUF;
            if (blockp == (blockbuffer + 4100)) {
                serialBypassActive = false;
                blockp = blockbuffer;
            }
        } else {
            rxbuf[rxhead] = UARTBUF;
            rxhead++;
        }
    }
    if (UARTSTA & 2) {  // TXC
        UARTSTA &= 0xfd;
        txtail++;
        if (txhead != txtail) {
            UARTBUF = txbuf[txtail];
        }
    }
}
