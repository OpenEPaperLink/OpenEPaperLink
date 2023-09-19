#include <Arduino.h>
#include "eeprom.h"
#include "comms.h"
#include "powermgt.h"
#include "proto.h"
#include "syncedproto.h"
#include "hal.h"

// *****************************************************************************
// when using HW SPI, this file requires the following to be present in the HAL
// uint8_t eepromSPIByte(uint8_t data);
// void eepromSPIBlockRead(uint8_t* dst, uint16_t len);
// void eepromSPIBlockWrite(uint8_t* src, uint16_t len);

#define EEPROM_USE_HW_SPI

#ifdef EEPROM_USE_HW_SPI
#define eepromByte eepromSPIByte
#endif

#ifndef EEPROM_USE_HW_SPI
uint8_t eepromByte(uint8_t data) {
    uint8_t temp = 0;
    for (uint8_t i = 0; i < 8; i++) {
        if (data & 0x80)
            digitalWrite(FLASH_MOSI, HIGH);
        else
            digitalWrite(FLASH_MOSI, LOW);
        data = (data << 1);
        digitalWrite(FLASH_CLK, HIGH);
        temp = (temp << 1);
        if (digitalRead(FLASH_MISO))
            temp |= 1;
        digitalWrite(FLASH_CLK, LOW);
    }
    return temp;
}
#endif

static uint32_t mEepromSize;
static uint8_t mOpcodeErz4K = 0, mOpcodeErz32K = 0, mOpcodeErz64K = 0;

// extern uint8_t * tempBuffer;
uint8_t tempBufferE[320] = {0};

uint32_t eepromGetSize(void) {
    return mEepromSize;
}

void eepromReadStart(uint32_t addr) {
    eepromPrvSelect();
    eepromByte(0x03);
    eepromByte(addr >> 16);
    eepromByte(addr >> 8);
    eepromByte(addr & 0xff);
}

void eepromRead(uint32_t addr, void *dstP, uint16_t len) {
    uint8_t *dst = (uint8_t *)dstP;

    eepromPrvSelect();
    eepromByte(0x03);
    eepromByte(addr >> 16);
    eepromByte(addr >> 8);
    eepromByte(addr & 0xff);
#ifdef EEPROM_USE_HW_SPI
    eepromSPIBlockRead(dst, len);
#else
    while (len--)
        *dst++ = eepromByte(0);
#endif
    eepromPrvDeselect();
}

static void eepromPrvSimpleCmd(uint8_t cmd) {
    eepromPrvSelect();
    eepromByte(cmd);
    eepromPrvDeselect();
}

static bool eepromPrvBusyWait(void) {
    uint8_t val;

    eepromPrvSelect();
    eepromByte(0x05);
    while ((val = eepromByte(0x00)) & 1)
        ;
    eepromPrvDeselect();

    return true;
}

static bool eepromWriteLL(uint32_t addr, const void *srcP, uint16_t len) {
    const uint8_t *src = (const uint8_t *)srcP;

    eepromPrvSimpleCmd(0x06);

    eepromPrvSelect();
    eepromByte(0x02);
    eepromByte(addr >> 16);
    eepromByte(addr >> 8);
    eepromByte(addr & 0xff);

#ifdef EEPROM_USE_HW_SPI
    eepromSPIBlockWrite((uint8_t *)src, len);
#else
    while (len--)
        eepromByte(*src++);
#endif
    eepromPrvDeselect();

    return eepromPrvBusyWait();
}

void eepromDeepPowerDown(void) {
    eepromPrvSimpleCmd(0xb9);
}

static void eepromPrvWakeFromPowerdown(void) {
    eepromPrvSimpleCmd(0xab);
}

static void eepromPrvSfdpRead(uint16_t ofst, uint8_t *dst, uint8_t len) {
    eepromPrvSelect();
    eepromByte(0x5a);  // cmd
    eepromByte(0);     // addr
    eepromByte(ofst >> 8);
    eepromByte(ofst);
    eepromByte(0x00);  // dummy
    while (len--)
        *dst++ = eepromByte(0);
    eepromPrvDeselect();
}

boolean eepromInit(void) {
    uint8_t buf[8];
    uint8_t i, nParamHdrs;

    eepromPrvWakeFromPowerdown();

    // process SFDP

    eepromPrvSfdpRead(0, buf, 8);
    if (buf[0] != 0x53 || buf[1] != 0x46 || buf[2] != 0x44 || buf[3] != 0x50 || buf[7] != 0xff) {
        printf("SFDP: header not found\n");

        boolean valid = false;

        // try manual ID for chips we know of
        eepromPrvSelect();
        eepromByte(0x90);
        eepromByte(0x00);
        eepromByte(0x00);
        eepromByte(0x00);
        if (eepromByte(0) == 0xc2) {  // old macronix chips
            valid = true;
            mOpcodeErz4K = 0x20;
            switch (eepromByte(0)) {
                case 0x05:  // MX25V512
                    mEepromSize = 0x00010000ul;
                    break;

                case 0x12:  // MX25V4005
                    mEepromSize = 0x00080000ul;
                    break;

                default:
                    valid = false;
                    break;
            }
        }
        eepromPrvDeselect();

        return valid;
    }
    if (buf[5] != 0x01) {
        printf("SFDP: version wrong: %u.%d\n", buf[5], buf[4]);
        return false;
    }
    nParamHdrs = buf[6];
    if (nParamHdrs == 0xff)  // that case is very unlikely and we just do not care
        nParamHdrs--;

    // now we need to find the JEDEC parameter table header
    for (i = 0; i <= nParamHdrs; i++) {
        eepromPrvSfdpRead((i * 8) + 8, buf, 8);
        if (buf[0] == 0x00 && buf[2] == 0x01 && buf[3] >= 9) {
            uint8_t j;

            eepromPrvSfdpRead(*(uint16_t *)(buf + 4), tempBufferE, 9 * 4);
            if ((tempBufferE[0] & 3) != 1) {
                printf("SFDP: no 4K ERZ\n");
                break;
            }
            if (!(tempBufferE[0] & 0x04)) {
                printf("SFDP: no large write buf\n");
                break;
            }
            if ((tempBufferE[2] & 0x06)) {
                printf("SFDP: addr.len != 3\n");
                break;
            }

            if (!tempBufferE[1] || tempBufferE[1] == 0xff) {
                printf("SFDP: 4K ERZ opcode invalid\n");
                break;
            }
            mOpcodeErz4K = tempBufferE[1];

            if (tempBufferE[7] & 0x80) {
                printf("SFDP: device too big\n");
                break;
            } else {
                uint8_t t;

                if ((t = tempBufferE[7]))
                    mEepromSize = 0x00200000UL;
                else if ((t = tempBufferE[6]))
                    mEepromSize = 0x00002000UL;
                else if ((t = tempBufferE[5]))
                    mEepromSize = 0x00000020UL;
                else {
                    printf("SFDP: device so small?!\n");
                    break;
                }

                while (t) {
                    mEepromSize <<= 1;
                    t >>= 1;
                }
            }

            // get erase opcodes
            for (j = 0x1c; j < 0x24; j += 2) {
                uint8_t instr = tempBufferE[j + 1];

                if (!instr || instr == 0xff)
                    continue;

                switch (tempBufferE[j]) {
                    case 0x0c:
                        if (mOpcodeErz4K != instr) {
                            printf("4K ERZ opcode disagreement\n");
                            return false;
                        }
                        break;

                    case 0x0f:  // 32K erase
                        mOpcodeErz32K = instr;
                        break;

                    case 0x10:  // 64K erase
                        mOpcodeErz64K = instr;
                        break;
                }
            }

            /*
            printf("EEPROM accepted\n");
            printf(" ERZ opcodes: \n");
            if (mOpcodeErz4K)
                printf(" 4K:  %02xh\n", mOpcodeErz4K);
            if (mOpcodeErz32K)
                printf(" 32K: %02xh\n", mOpcodeErz32K);
            if (mOpcodeErz64K)
                printf(" 64K: %02xh\n", mOpcodeErz64K);
            printf(" Size: 0x%*08lx\n", (uint16_t)&mEepromSize);
            */
            return true;
        }
    }

    printf("SFDP: no JEDEC table of expected version found\n");
    return false;
}

bool eepromWrite(uint32_t addr, const void *srcP, uint16_t len) {
    const uint8_t *src = (const uint8_t *)srcP;

    while (len) {
        uint16_t lenNow = EEPROM_WRITE_PAGE_SZ - (addr & (EEPROM_WRITE_PAGE_SZ - 1));

        if (lenNow > len)
            lenNow = len;

        if (!eepromWriteLL(addr, src, lenNow))
            return false;

        addr += lenNow;
        src += lenNow;
        len -= lenNow;
    }
    return true;
}

bool eepromErase(uint32_t addr, uint16_t nSec) {
    uint8_t now;

    if (((uint16_t)addr) & 0x0fff)
        return false;

    for (; nSec; nSec -= now) {
        eepromPrvSimpleCmd(0x06);
        eepromPrvSelect();

        if (nSec >= 16 && !(uint16_t)addr && mOpcodeErz64K) {  // erase 64K

            eepromByte(mOpcodeErz64K);
            now = 16;
        } else if (nSec >= 8 && !(((uint16_t)addr) & 0x7fff) && mOpcodeErz32K) {  // erase 32K

            eepromByte(mOpcodeErz32K);
            now = 8;
        } else {  // erase 4K

            eepromByte(mOpcodeErz4K);
            now = 1;
        }

        eepromByte(addr >> 16);
        eepromByte(addr >> 8);
        eepromByte(addr);
        eepromPrvDeselect();

        if (!eepromPrvBusyWait())
            return false;

        addr += (EEPROM_ERZ_SECTOR_SZ * now);
    }

    return true;
}

void eepromOtpModeEnter(void) {
    eepromPrvSimpleCmd(0xb1);
}

void eepromOtpModeExit(void) {
    eepromPrvSimpleCmd(0xc1);
}