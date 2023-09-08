// data / _command: 2.2
// _select          1.7
// busy             2.1
// reset            2.0
// spi.clk          0.0
// spi.mosi         0.1

#include "i2cdevices.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "cpu.h"
#include "i2c.h"
#include "printf.h"
#include "timer.h"

#define __packed
#include "../oepl-definitions.h"
#include "../oepl-proto.h"

extern void dump(uint8_t* __xdata a, uint16_t __xdata l);  // remove me when done

extern uint8_t __xdata blockXferBuffer[];
__xdata uint8_t i2cbuffer[18];

bool supportsNFCWake() {
    P1PULL |= (1 << 3);
    timerDelay(33300);  // wait 25 ms
    uint32_t pcount = 0;
    P1PULL &= ~(1 << 3);
    while (P1_3 && pcount < 10000) {
        pcount++;
    }
    if (pcount < 10000) {
        // P1_3 (Field Detect) dropped to 'low' pretty fast, this means the load on this pin is high
        pr("NFC: This tag currently does not support NFC wake, load on the FD pin (P1.3) is pretty high.\nOn some boards, a pull-up resistor backpowers the NFC IC. Consider removing it!\n");
        return false;
    } else {
        // No reason to believe this pin is currently loaded down severely
        return true;
    }
}

void loadRawNTag(uint16_t blocksize) {
    struct I2cTransaction __xdata i2ctrans;
    if (blocksize > 864) blocksize = 864;

    uint8_t trcount = (uint8_t)(blocksize / 16);
    if (blocksize % 16) trcount++;

    for (uint8_t c = 0; c < trcount; c++) {
        i2ctrans.numBytes = 17;
        i2ctrans.deviceAddr = (uint8_t)0x55 << 1;
        i2ctrans.bytes = i2cbuffer;
        i2cbuffer[0] = c + 1;
        memcpy(i2cbuffer + 1, sizeof(struct blockData) + blockXferBuffer + (c * 16), 16);
        uint8_t res = i2cTransact(&i2ctrans, 1);
        timerDelay(133300);
    }
}

void loadURLtoNTag() {
    // https://learn.adafruit.com/adafruit-pn532-rfid-nfc/ndef << very helpful

    uint8_t __xdata i2cbuffer[18];
    __xdata uint8_t* tempbuffer = blockXferBuffer + 2048;

    strncpy(tempbuffer + 7, blockXferBuffer + sizeof(struct blockData), 245);
    uint8_t __xdata len = strlen(tempbuffer + 7);
    struct I2cTransaction __xdata i2ctrans;

    // TLV
    tempbuffer[0] = 0x03;  // NDEF message (TLV type)
    tempbuffer[1] = 4 + len + 1;

    // ndef record
    tempbuffer[2] = 0xD1;
    tempbuffer[3] = 0x01;     // well known record type
    tempbuffer[4] = len + 1;  // payload length
    tempbuffer[5] = 0x55;     // payload type (URI record)
    tempbuffer[6] = 0x00;     // URI identifier code (no prepending)

    len = 7 + len;

    tempbuffer[len] = 0xFE;

    uint8_t trcount = len / 16;
    if (len % 16) trcount++;

    for (uint8_t c = 0; c < trcount; c++) {
        i2ctrans.numBytes = 17;
        i2ctrans.deviceAddr = (uint8_t)0x55 << 1;
        i2ctrans.bytes = i2cbuffer;
        i2cbuffer[0] = c + 1;
        memcpy(i2cbuffer + 1, tempbuffer + (c * 16), 16);
        uint8_t res = i2cTransact(&i2ctrans, 1);
        timerDelay(133300);
    }
}

void i2cBusScan() {
    struct I2cTransaction __xdata iictest;
    iictest.numBytes = 0;
    iictest.bytes = NULL;
    pr("Starting I2C scan...\n");
    for (uint8_t address = 0x00; address <= 0x7F; address++) {
        iictest.deviceAddr = address << 1;
        uint8_t res = i2cTransact(&iictest, 1);
        if (res == 0) {
            pr(" - Found i2c device at %02X\n", address);
        }
        timerDelay(13330);
    }
    pr("I2C scan complete\n");
}

bool i2cCheckDevice(uint8_t address) {
    struct I2cTransaction __xdata iictest;
    iictest.numBytes = 0;
    iictest.deviceAddr = address << 1;
    uint8_t res = i2cTransact(&iictest, 1);
    if (res == 0) {
        pr("I2C: Device found at 0x%02X\n", address);
        return true;
    }
    return false;
}