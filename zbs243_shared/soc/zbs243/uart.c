#include "uart.h"
#include "cpu.h"

#ifdef AP_FW
#include "stdbool.h"
#include "string.h"
#endif

void uartInit(void) {
    // clock it up
    CLKEN |= 0x20;
    // configure baud rate
    UARTBRGH = 0x00;
#ifdef AP_FW
    UARTBRGL = 69;  // nice. 230400 baud
    IEN_UART0 = 1;
#else
    UARTBRGL = 0x8A;  // config for 115200
#endif
    UARTSTA = 0x12;  // also set the "empty" bit else we wait forever for it to go up
}

#ifndef AP_FW
void uartTx(uint8_t val) {
    while (!(UARTSTA & (1 << 1)))
        ;
    UARTSTA &= ~(1 << 1);
    UARTBUF = val;
}
#else
extern uint8_t __xdata blockbuffer[];

volatile uint8_t txtail = 0;
volatile uint8_t txhead = 0;
uint8_t __xdata txbuf[256] = {0};

volatile uint8_t __idata rxtail = 0;
volatile uint8_t __idata rxhead = 0;
uint8_t __xdata rxbuf[256] = {0};

void uartTx(uint8_t val) {
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
uint8_t __idata cmd[3];
volatile bool __idata serialBypassActive = false;

void checkcommand(uint8_t rx) {
    for (uint8_t c = 0; c < 2; c++) {
        cmd[c] = cmd[c + 1];
    }
    cmd[2] = rx;
    if (strncmp(cmd, ">D>", 3) == 0) {
        blockp = blockbuffer;
        serialBypassActive = true;
    }
}

void UART_IRQ1(void) __interrupt(0) {
    if (UARTSTA & 1) {  // RXC
        UARTSTA &= 0xfe;
        if (serialBypassActive) {
            *blockp++ = UARTBUF;
            if (blockp == (blockbuffer + 4100)) {
                serialBypassActive = false;
            }
        } else {
            rxbuf[rxhead] = UARTBUF;
            rxhead++;
            checkcommand(UARTBUF);
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
#endif