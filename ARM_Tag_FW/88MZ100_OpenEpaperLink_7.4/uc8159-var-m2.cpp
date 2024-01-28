
#include "uc8159-var-m2.h"

extern "C" {

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "mz100/gpio.h"
#include "mz100/mz100_gpio.h"
#include "mz100/mz100_pinmux.h"
#include "mz100/printf.h"
#include "mz100/util.h"
}

#include "drawing.h"
#include "epd_interface.h"

#define EPD_PANEL_SETTING 0x00
#define EPD_POWER_SETTING 0x01
#define EPD_POWER_OFF 0x02
#define EPD_POWER_OFF_SEQUENCE 0x03
#define EPD_POWER_ON 0x04
#define EPD_BOOSTER_SOFT_START 0x06
#define EPD_DEEP_SLEEP 0x07
#define EPD_START_DATA 0x10
#define EPD_DATA_STOP 0x11
#define EPD_REFRESH 0x12
#define EPD_IMAGE_PROCESS 0x13
#define EPD_LUT_VCOM 0x20
#define EPD_LUT_B 0x21
#define EPD_LUT_W 0x22
#define EPD_LUT_G1 0x23
#define EPD_LUT_G2 0x24
#define EPD_LUT_R0 0x25
#define EPD_LUT_R1 0x26
#define EPD_LUT_R2 0x27
#define EPD_LUT_R3 0x28
#define EPD_LUT_XON 0x29
#define EPD_PLL_CONTROL 0x30
#define EPD_TEMP_CALIB 0x40
#define EPD_TEMP_SELECT 0x41
#define EPD_TEMP_WRITE 0x42
#define EPD_TEMP_READ 0x43
#define EPD_VCOM_DATA_INTERVAL 0x50
#define EPD_LPD 0x51
#define EPD_TCON_SET 0x60
#define EPD_TRES 0x61
#define EPD_SPI_FLASH_CONTROL 0x65
#define EPD_REVISION 0x70
#define EPD_GET_STATUS 0x71
#define EPD_AUTOMEASURE_VCOM 0x80
#define EPD_READ_VCOM 0x81
#define EPD_VCOM_DC_SET 0x82
#define EPD_SET_WINDOW 0x90

#define EPD_WAKE_EEPROM 0xAB
#define EPD_EEPROM_SLEEP 0xB9
#define EPD_UNKNOWN_1 0xE5

#define EPD_IS_BUSY false
#define EPD_IS_NOT_BUSY true

extern "C" {
extern void dump(const uint8_t *a, const uint16_t l);
}

struct __attribute__((packed)) epd_colorlutpart {
    uint8_t repeat;
    uint8_t lvl0 : 4;
    uint8_t lvl1 : 4;
    uint8_t lvl2 : 4;
    uint8_t lvl3 : 4;
    uint8_t lvl4 : 4;
    uint8_t lvl5 : 4;
    uint8_t lvl6 : 4;
    uint8_t lvl7 : 4;
    uint8_t length[8];
};

struct __attribute__((packed)) epd_colorlut {
    struct epd_colorlutpart part[20];
};

struct __attribute__((packed)) epd_vcomlutpart {
    uint8_t repeat;
    uint8_t lvl0 : 2;
    uint8_t lvl1 : 2;
    uint8_t lvl2 : 2;
    uint8_t lvl3 : 2;
    uint8_t lvl4 : 2;
    uint8_t lvl5 : 2;
    uint8_t lvl6 : 2;
    uint8_t lvl7 : 2;
    uint8_t length[8];
};

struct __attribute__((packed)) epd_vcomlut {
    struct epd_vcomlutpart part[20];
};

struct __attribute__((packed)) epd_xonlutpart {
    uint8_t repeat;
    uint8_t lvl0 : 1;
    uint8_t lvl1 : 1;
    uint8_t lvl2 : 1;
    uint8_t lvl3 : 1;
    uint8_t lvl4 : 1;
    uint8_t lvl5 : 1;
    uint8_t lvl6 : 1;
    uint8_t lvl7 : 1;
    uint8_t length[8];
};

struct __attribute__((packed)) epd_xonlut {
    struct epd_xonlutpart part[20];
};

void uc8159::epdSetup() {
    printf("EPD: Init begin\n");
    initEPDGPIO();
    this->epdReset();
    epdWrite(EPD_POWER_ON, 0);
    busyWaitUntil(EPD_IS_NOT_BUSY, 100);

    epdWrite(EPD_TRES, 0x04, 2, 128, 1, 128);

    // wake the eeprom
    epdWrite(EPD_SPI_FLASH_CONTROL, 1, 0x01);
    GPIO_WritePinOutput(EPD_HLT_CTRL, GPIO_IO_LOW);
    epdWrite(EPD_WAKE_EEPROM, 0x00);
    GPIO_WritePinOutput(EPD_HLT_CTRL, GPIO_IO_HIGH);
    epdWrite(EPD_SPI_FLASH_CONTROL, 1, 0x00);

    epdWrite(EPD_POWER_SETTING, 4, 0x37, 0x00, 0x05, 0x05);  // 0x37 - 00- 05 05 this one worked

    epdWrite(EPD_PANEL_SETTING, 2, 0xC3, 0x08);  // 0xC3-0x88 // lut from EEPROM
    epdWrite(EPD_POWER_OFF_SEQUENCE, 1, 0x00);
    epdWrite(EPD_BOOSTER_SOFT_START, 0x03, 199, 204, 45);
    epdWrite(EPD_PLL_CONTROL, 0x01, 60);
    epdWrite(EPD_TEMP_SELECT, 0x01, 0x00);
    epdWrite(EPD_VCOM_DATA_INTERVAL, 0x01, 119);
    epdWrite(EPD_TCON_SET, 0x01, 34);

    EPDtempBracket = this->getTempBracket();
    this->loadFrameRatePLL(EPDtempBracket);
    this->loadTempVCOMDC(EPDtempBracket);

    // this->loadTempVSH(EPDtempBracket);
    // epdWrite(EPD_POWER_SETTING, 4, 0x37, 0x00, this->vshc, this->vslc); // this doesn't work

    printf("EPD: Init complete\n");
}

void uc8159::selectLUT(uint8_t lut) {
    this->drawLut = lut;
}

uc8159::~uc8159() {
}

uint8_t uc8159::getTempBracket() {
    uint8_t v0;
    uint8_t v1;
    uint8_t temptable[40];

    epdWrite(EPD_SPI_FLASH_CONTROL, 1, 0x01);
    eepromReadBlock(0, 25002, temptable, 10);
    epdWrite(EPD_SPI_FLASH_CONTROL, 1, 0x00);

    epdWrite(EPD_TEMP_CALIB, 0);
    busyWaitUntil(EPD_IS_NOT_BUSY, 10);
    v0 = softSPIReadByte();
    v1 = (uint8_t)(2 * v0) + ((uint8_t)softSPIReadByte() >> 7);

    uint8_t bracket = 0;
    for (int i = 0; i < 9; i++) {
        if ((((char)v1 - (uint8_t)temptable[i]) & 0x80) != 0) {
            bracket = i;
            break;
        }
    }
    printf("EPD: Temp bracket = %d\n", bracket);
    return bracket;
}
void uc8159::loadFrameRatePLL(uint8_t bracket) {
    uint8_t pllvalue;
    uint8_t plltable[12];
    epdWrite(EPD_SPI_FLASH_CONTROL, 1, 0x01);
    eepromReadBlock(0, 25039, plltable, 10);
    epdWrite(EPD_SPI_FLASH_CONTROL, 1, 0x00);
    pllvalue = plltable[bracket];
    printf("loading pll value 0x%02X\n", pllvalue);
    pllvalue = 0x3A;  // was 0x3C
    epdWrite(EPD_PLL_CONTROL, 1, pllvalue);
}
void uc8159::loadTempVCOMDC(uint8_t bracket) {
    uint8_t vcomvalue;
    uint8_t vcomtable[12];
    epdWrite(EPD_SPI_FLASH_CONTROL, 1, 1);
    eepromReadBlock(0, 25049, vcomtable, 10);
    epdWrite(EPD_SPI_FLASH_CONTROL, 1, 0);
    vcomvalue = vcomtable[bracket];
    epdWrite(EPD_VCOM_DC_SET, 1, vcomvalue);
}
void uc8159::loadTempVSH(uint8_t bracket) {
    uint8_t vshtable[20];
    epdWrite(EPD_SPI_FLASH_CONTROL, 1, 1);
    eepromReadBlock(0, 25011, vshtable, 20);
    epdWrite(EPD_SPI_FLASH_CONTROL, 1, 0);
    this->vshc = vshtable[bracket * 2];
    this->vslc = vshtable[(bracket * 2) + 1];
}
uint8_t *uc8159::loadLUT(uint8_t index, uint8_t bracket) {
    uint16_t adr = 0;
    uint16_t len = 0;
    uint8_t *lutBuffer;
    switch (index) {
        case EPD_LUT_VCOM:
            // VCOM LUT
            adr = 20800 + (220 * bracket);
            len = 220;
            break;
        case EPD_LUT_B:
        case EPD_LUT_W:
        case EPD_LUT_G1:
        case EPD_LUT_G2:
        case EPD_LUT_R0:
        case EPD_LUT_R1:
        case EPD_LUT_R2:
        case EPD_LUT_R3:
            adr = (bracket * 2080);
            adr += (index - 0x21) * 260;
            len = 260;
            break;
        case EPD_LUT_XON:
            // XON LUT
            adr = 23000 + (200 * bracket);
            len = 200;
            break;
    }

    epdWrite(EPD_SPI_FLASH_CONTROL, 1, 1);
    lutBuffer = (uint8_t *)malloc(len);
    if (lutBuffer) eepromReadBlock(0, adr, lutBuffer, len);
    epdWrite(EPD_SPI_FLASH_CONTROL, 1, 0);
    return lutBuffer;
}

void colorLutSkip(struct epd_colorlut *colorlut, uint8_t skip) {
    for (uint8_t shift = skip; shift < 19; shift++) {
        colorlut->part[shift] = colorlut->part[shift + 1];
    }
}

void vcomLutSkip(struct epd_vcomlut *colorlut, uint8_t skip) {
    for (uint8_t shift = skip; shift < 19; shift++) {
        colorlut->part[shift] = colorlut->part[shift + 1];
    }
}

void xonLutSkip(struct epd_xonlut *colorlut, uint8_t skip) {
    for (uint8_t shift = skip; shift < 19; shift++) {
        colorlut->part[shift] = colorlut->part[shift + 1];
    }
}

void uc8159::loadLUTSfromEEPROM(uint8_t bracket, bool doRed) {
    for (uint8_t c = EPD_LUT_B; c <= EPD_LUT_R3; c++) {
        struct epd_colorlut *colorlut = (struct epd_colorlut *)loadLUT(c, bracket);

        colorLutSkip(colorlut, 0);
        colorLutSkip(colorlut, 0);
        colorlut->part[0].repeat = 1;
        colorLutSkip(colorlut, 2);
        if (!doRed) {
            colorLutSkip(colorlut, 3);
            colorLutSkip(colorlut, 3);
            colorLutSkip(colorlut, 3);
        }

        // colorLutSkip(colorlut, 3);
        //  0 0 2 3 was okay // 00034 okay reds?
        epdBlockWrite(c, (uint8_t *)colorlut, 260);
        if (colorlut) free(colorlut);
    }

    struct epd_vcomlut *vcomlut = (struct epd_vcomlut *)loadLUT(EPD_LUT_VCOM, bracket);
    vcomLutSkip(vcomlut, 0);
    vcomLutSkip(vcomlut, 0);
    vcomlut->part[0].repeat = 1;

    vcomLutSkip(vcomlut, 2);
    if (!doRed) {
        vcomLutSkip(vcomlut, 3);
        vcomLutSkip(vcomlut, 3);
        vcomLutSkip(vcomlut, 3);
    }
    epdBlockWrite(EPD_LUT_VCOM, (uint8_t *)vcomlut, 220);
    if (vcomlut) free(vcomlut);

    struct epd_xonlut *xonlut = (struct epd_xonlut *)loadLUT(EPD_LUT_XON, bracket);

    xonLutSkip(xonlut, 0);
    xonLutSkip(xonlut, 0);
    xonlut->part[0].repeat = 1;
    xonLutSkip(xonlut, 2);
    if (!doRed) {
        xonLutSkip(xonlut, 3);
        xonLutSkip(xonlut, 3);
        xonLutSkip(xonlut, 3);
    }
    epdBlockWrite(EPD_LUT_XON, (uint8_t *)xonlut, 200);
    if (xonlut) free(xonlut);
}
void uc8159::epdReset() {
    uint8_t v0 = 5;
    printf("EPD: Reset... ");
    while (1) {
        GPIO_WritePinOutput(EPD_RESET, GPIO_IO_HIGH);
        delay(100);
        GPIO_WritePinOutput(EPD_RESET, GPIO_IO_LOW);
        delay(3000);
        GPIO_WritePinOutput(EPD_RESET, GPIO_IO_HIGH);
        delay(3000);
        if (GPIO_ReadPinLevel(EPD_BUSY))
            break;
        v0--;
        if (!v0) {
            printf(" - EPD reset failure!\r\n");
            break;
        }
    }
    delay(5000);
    printf("complete.\n");
}

void uc8159::eepromReadBlock(char a1, uint16_t readaddress, uint8_t *target, uint16_t length) {
    GPIO_WritePinOutput(EPD_CS, GPIO_IO_HIGH);
    GPIO_WritePinOutput(EPD_HLT_CTRL, GPIO_IO_LOW);
    softSPIWriteByte(3);
    softSPIWriteByte(a1);
    softSPIWriteByte(readaddress >> 8);
    softSPIWriteByte(readaddress);
    delay_us(5);
    for (uint16_t i = 0; i < length; i++) {
        uint8_t readbyte = 0;
        uint8_t loopCount = 0;
        do {
            readbyte *= 2;
            GPIO_WritePinOutput(EPD_CLK, GPIO_IO_LOW);
            delay_us(1);
            if (GPIO_ReadPinLevel(EPD_MISO))
                readbyte |= 1u;
            GPIO_WritePinOutput(EPD_CLK, GPIO_IO_HIGH);
            delay_us(1);
            delay(1);
            loopCount++;
        } while (loopCount < 8);
        delay_us(1);
        *target++ = readbyte;
    }

    GPIO_WritePinOutput(EPD_CLK, GPIO_IO_LOW);
    GPIO_WritePinOutput(EPD_HLT_CTRL, GPIO_IO_HIGH);
    delay(1);
}

void uc8159::interleaveColorToBuffer(uint8_t *dst, uint8_t b, uint8_t r) {
    b ^= 0xFF;
    uint8_t b_out = 0;
    for (int8_t shift = 3; shift >= 0; shift--) {
        b_out = 0;
        if (((b >> 2 * shift) & 0x01) && ((r >> 2 * shift) & 0x01)) {
            b_out |= 0x04;  // 0x30
        } else if ((b >> 2 * shift) & 0x01) {
            b_out |= 0x03;                     // 0x30
        } else if ((r >> 2 * shift) & 0x01) {  // 4 or 5
            b_out |= 0x04;                     // 0x30
        } else {
        }

        if (((b >> 2 * shift) & 0x02) && ((r >> 2 * shift) & 0x02)) {
            b_out |= 0x40;  // 0x30
        } else if ((b >> 2 * shift) & 0x02) {
            b_out |= 0x30;  // 0x30
        } else if ((r >> 2 * shift) & 0x02) {
            b_out |= 0x40;  // 0x30
        } else {
        }
        *dst++ = b_out;
    }
}

void uc8159::epdWriteDisplayData() {
    uint8_t blocksize = 32;
    uint16_t byteWidth = this->effectiveXRes / 8;
    uint8_t screenrow_bw[byteWidth * blocksize];
    uint8_t screenrow_r[byteWidth * blocksize];
    uint8_t screenrowInterleaved[byteWidth * 4];
    // setDisplayWindow(0, 0, 640, 384);
    epdWrite(EPD_START_DATA, 0);
    enableHardSPI(true);
    GPIO_WritePinOutput(EPD_CS, GPIO_IO_LOW);
    for (uint16_t curY = 0; curY < this->effectiveYRes; curY += blocksize) {  //
        memset(screenrow_bw, 0, byteWidth * blocksize);
        memset(screenrow_r, 0, byteWidth * blocksize);

        for (uint8_t bcount = 0; bcount < blocksize; bcount++) {
            drawItem::renderDrawLine(screenrow_bw + (byteWidth * bcount), curY + bcount, 0);
        }
        for (uint8_t bcount = 0; bcount < blocksize; bcount++) {
            drawItem::renderDrawLine(screenrow_r + (byteWidth * bcount), curY + bcount, 1);
        }

        for (uint8_t bcount = 0; bcount < blocksize; bcount++) {
            for (uint16_t curX = 0; curX < (byteWidth); curX++) {
                interleaveColorToBuffer(screenrowInterleaved + (curX * 4), screenrow_bw[curX + (byteWidth * bcount)], screenrow_r[curX + (byteWidth * bcount)]);
            }
            epdBlockWrite(screenrowInterleaved, byteWidth * 4);
        }
    }
    GPIO_WritePinOutput(EPD_CS, GPIO_IO_HIGH);
    enableHardSPI(false);

    drawItem::flushDrawItems();
}

void uc8159::draw() {
    this->drawNoWait();
    this->epdWaitRdy();
}

void uc8159::drawNoWait() {
    wdt60s();
    epdWriteDisplayData();
    if (drawLut) {
        epdWrite(EPD_PANEL_SETTING, 2, 0xC3, 0x88);  // 0xC3-0x88 // lut from register
        bool doReds = true;
        if (drawLut == 2) doReds = false;
        loadLUTSfromEEPROM(EPDtempBracket, doReds);
    } else {
        epdWrite(EPD_PANEL_SETTING, 2, 0xC3, 0x08);  // 0xC3-0x88 // lut from EEPROM
        epdWrite(EPD_UNKNOWN_1, 1, 0x03);            // load lut, probably
        busyWaitUntil(EPD_IS_BUSY, 10);
        busyWaitUntil(!EPD_IS_BUSY, 100);
    }
    printf("EPD: Draw start\n");
    epdWrite(EPD_REFRESH, 0);
    busyWaitUntil(EPD_IS_BUSY, 10);
}

void uc8159::epdWaitRdy() {
    // do_sleeped_epd_refresh();
    busyWaitUntil(!EPD_IS_BUSY, 300000);
    printf("EPD: Draw done!\n");
}

void uc8159::epdEnterSleep() {
    initEPDGPIO();
    this->epdReset();
    epdWrite(EPD_POWER_SETTING, 4, 0x02, 0x00, 0x00, 0x00);
    delay_us(50000);
    epdWrite(EPD_POWER_OFF, 0);
    delay_us(100000);
    busyWaitUntil(EPD_IS_NOT_BUSY, 100000);
    epdWrite(EPD_SPI_FLASH_CONTROL, 1, 1);
    GPIO_WritePinOutput(EPD_HLT_CTRL, GPIO_IO_LOW);
    softSPIWriteByte(EPD_EEPROM_SLEEP);
    GPIO_WritePinOutput(EPD_HLT_CTRL, GPIO_IO_HIGH);
    epdWrite(EPD_SPI_FLASH_CONTROL, 1, 0);
    epdWrite(EPD_DEEP_SLEEP, 1, 0xA5);
}
