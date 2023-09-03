#include "epd.h"

#include <stdbool.h>
#include <stdint.h>
// #include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "12x20_horizontal_LSB_1.h"
#include "core_cm3.h"
#include "gpio.h"
#include "main.h"
#include "mz100_clock.h"
#include "mz100_gpio.h"
#include "mz100_pinmux.h"
#include "mz100_ssp.h"
#include "mz100_wdt.h"
#include "printf.h"
#include "stdarg.h"
#include "util.h"

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

static uint8_t EPDtempBracket = 0;

#define CHAR_WIDTH_BYTES 2
#define CHAR_HEIGHT 20
#define CHAR_WIDTH 12
#define SCREEN_WIDTH 640

uint8_t buffer[CHAR_HEIGHT][640 / 8];
uint8_t charbuffer[CHAR_HEIGHT][CHAR_WIDTH_BYTES + 1];

#define CHAR_SPACING 1
#define EMPTY_SPACING 3

struct epd_colorlutpart {
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
} __packed;

struct epd_colorlut {
    struct epd_colorlutpart part[20];
} __packed;

struct epd_vcomlutpart {
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
} __packed;

struct epd_vcomlut {
    struct epd_vcomlutpart part[20];
} __packed;

struct epd_xonlutpart {
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
} __packed;

struct epd_xonlut {
    struct epd_xonlutpart part[20];
} __packed;

void interleaveBW(uint8_t b) {
    b ^= 0xFF;
    uint8_t b_out = 0;
    for (uint8_t shift = 0; shift < 4; shift++) {
        b_out = 0;
        if ((b >> 2 * shift) & 0x01) b_out |= 0x30;  // 0x30
        if ((b >> 2 * shift) & 0x02) b_out |= 0x03;  // 0x03
        display_tx_byte(b_out);
    }
}

void interleaveColor(uint8_t b, uint8_t r) {
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
        display_tx_byte(b_out);
    }
}

void epdWrite(uint8_t reg, uint8_t len, ...) {
    va_list valist;
    va_start(valist, len);
    epd_pin_enable(1);
    GPIO_WritePinOutput(EPD_CS, GPIO_IO_LOW);
    GPIO_WritePinOutput(EPD_DC, GPIO_IO_LOW);
    SSP_SendData(SSP2_ID, reg);
    for (int i = 0; i < 0xF; ++i)
        __ISB();
    GPIO_WritePinOutput(EPD_DC, GPIO_IO_HIGH);
    for (uint8_t i = 0; i < len; i++) {
        SSP_SendData(SSP2_ID, va_arg(valist, int));
    }

    for (int j = 0; j < 0xF; ++j)
        __ISB();
    GPIO_WritePinOutput(EPD_CS, GPIO_IO_HIGH);
    epd_pin_enable(0);
    va_end(valist);
}

void epd_reset() {
    uint8_t v0 = 5;
    printf("Resetting...");
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
            printf("EPD reset failure\r\n");
            break;
        }
    }
    delay(5000);
    printf("    Reset complete\n");
}

void EPD_cmd(char a1) {
    epd_pin_enable(1);

    GPIO_WritePinOutput(EPD_CS, GPIO_IO_LOW);
    GPIO_WritePinOutput(EPD_DC, GPIO_IO_LOW);
    SSP_SendData(SSP2_ID, a1);
    for (int i = 0; i < 0xF; ++i)
        __ISB();

    epd_pin_enable(0);

    GPIO_WritePinOutput(EPD_DC, GPIO_IO_HIGH);
    GPIO_WritePinOutput(EPD_CS, GPIO_IO_HIGH);
}

void EPD_data(char a1) {
    epd_pin_enable(1);

    GPIO_WritePinOutput(EPD_CS, GPIO_IO_LOW);
    SSP_SendData(SSP2_ID, a1);
    for (int i = 0; i < 0xF; ++i)
        __ISB();

    epd_pin_enable(0);

    GPIO_WritePinOutput(EPD_CS, GPIO_IO_HIGH);
}

void spi_soft_send_byte(char a1) {
    uint8_t v2 = 0;
    do {
        if ((a1 & 0x80) != 0)
            GPIO_WritePinOutput(EPD_MOSI, GPIO_IO_HIGH);
        else
            GPIO_WritePinOutput(EPD_MOSI, GPIO_IO_LOW);
        delay_us(1);
        GPIO_WritePinOutput(EPD_CLK, GPIO_IO_HIGH);
        delay_us(1);
        GPIO_WritePinOutput(EPD_CLK, GPIO_IO_LOW);
        a1 *= 2;
        v2++;
    } while (v2 < 8);
    GPIO_WritePinOutput(EPD_MOSI, GPIO_IO_LOW);
    delay_us(1);
}

void BUSY_wait(unsigned int a1) {
    unsigned int v2 = 0;
    while (GPIO_ReadPinLevel(EPD_BUSY) == GPIO_IO_LOW) {
        delay(10000);
        v2++;
        if (v2 > a1)
            break;
        if (((v2 % 1000) / 10) == 0)
            WDT_RestartCounter();
    }
}

void spi_soft_read_buffer(char a1, uint16_t readaddress, uint8_t *target, uint16_t length) {
    char v9;
    unsigned int v10;

    GPIO_WritePinOutput(EPD_CS, GPIO_IO_HIGH);
    GPIO_WritePinOutput(EPD_HLT_CTRL, GPIO_IO_LOW);
    spi_soft_send_byte(3);
    spi_soft_send_byte(a1);
    spi_soft_send_byte(readaddress >> 8);
    spi_soft_send_byte(readaddress);
    delay_us(5);
    for (uint16_t i = 0; i < length; i++) {
        v9 = 0;
        v10 = 0;
        do {
            v9 *= 2;
            GPIO_WritePinOutput(EPD_CLK, GPIO_IO_LOW);
            delay_us(1);
            if (GPIO_ReadPinLevel(EPD_MISO))
                v9 |= 1u;
            GPIO_WritePinOutput(EPD_CLK, GPIO_IO_HIGH);
            delay_us(1);
            v10++;
        } while (v10 < 8);
        delay_us(1);
        *target++ = v9;
    }

    GPIO_WritePinOutput(EPD_CLK, GPIO_IO_LOW);
    GPIO_WritePinOutput(EPD_HLT_CTRL, GPIO_IO_HIGH);
}

int spi_soft_read_byte() {
    int v0;
    unsigned int v1;

    v0 = 0;
    GPIO_WritePinOutput(EPD_BS, GPIO_IO_HIGH);
    GPIO_WritePinOutput(EPD_CS, GPIO_IO_LOW);
    delay_us(1);
    GPIO_WritePinOutput(EPD_MOSI, GPIO_IO_HIGH);
    GPIO_WritePinOutput(EPD_CLK, GPIO_IO_HIGH);
    delay_us(1);
    GPIO_WritePinOutput(EPD_CLK, GPIO_IO_LOW);
    delay_us(1);
    GPIO_SetPinDir(EPD_MOSI, GPIO_INPUT);
    GPIO_PinMuxFun(EPD_MOSI, 0);
    delay_us(3);
    v1 = 0;
    do {
        v0 = (uint8_t)(2 * v0);
        GPIO_WritePinOutput(EPD_CLK, GPIO_IO_HIGH);
        if (GPIO_ReadPinLevel(EPD_MOSI))
            v0 |= 1u;
        GPIO_WritePinOutput(EPD_CLK, GPIO_IO_LOW);
        delay_us(1);
        v1++;
    } while (v1 < 8);
    GPIO_SetPinDir(EPD_MOSI, GPIO_OUTPUT);
    GPIO_PinMuxFun(EPD_MOSI, 0);
    GPIO_WritePinOutput(EPD_MOSI, GPIO_IO_LOW);
    delay_us(1);
    GPIO_WritePinOutput(EPD_CS, GPIO_IO_HIGH);
    delay_us(1);
    GPIO_WritePinOutput(EPD_BS, GPIO_IO_LOW);
    return v0;
}

uint8_t getTempBracket() {
    uint8_t v0;
    uint8_t v1;
    uint8_t temptable[40];

    EPD_cmd(EPD_SPI_FLASH_CONTROL);
    EPD_data(1);
    delay_us(1000);
    spi_soft_read_buffer(0, 25002, temptable, 10);
    delay_us(1000);
    delay_us(1000);
    EPD_cmd(EPD_SPI_FLASH_CONTROL);
    EPD_data(0);

    EPD_cmd(EPD_TEMP_CALIB);
    BUSY_wait(0xAu);
    v0 = spi_soft_read_byte();
    v1 = (uint8_t)(2 * v0) + ((uint8_t)spi_soft_read_byte() >> 7);

    uint8_t bracket = 0;
    for (int i = 0; i < 9; i++) {
        if ((((char)v1 - (uint8_t)temptable[i]) & 0x80) != 0) {
            bracket = i;
            break;
        }
    }
    return bracket;
}

void loadFrameRatePLL(uint8_t bracket) {
    uint8_t pllvalue;
    uint8_t plltable[12];

    EPD_cmd(EPD_SPI_FLASH_CONTROL);
    EPD_data(1);
    delay_us(1000);
    spi_soft_read_buffer(0, 25039, plltable, 10);
    delay_us(1000);
    EPD_cmd(EPD_SPI_FLASH_CONTROL);
    EPD_data(0);

    pllvalue = plltable[bracket];

    EPD_cmd(EPD_PLL_CONTROL);
    EPD_data(pllvalue);
}

extern void dump(const uint8_t *a, const uint16_t l);

void loadTempVCOMDC(uint8_t bracket) {
    uint8_t vcomvalue;
    uint8_t vcomtable[12];

    EPD_cmd(EPD_SPI_FLASH_CONTROL);
    EPD_data(1);
    delay_us(1000);
    spi_soft_read_buffer(0, 25049, vcomtable, 10);
    delay_us(1000);
    EPD_cmd(EPD_SPI_FLASH_CONTROL);
    EPD_data(0);

    vcomvalue = vcomtable[bracket];

    EPD_cmd(EPD_VCOM_DC_SET);
    EPD_data(vcomvalue);
}

uint8_t *loadLUT(uint8_t index, uint8_t bracket) {
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

    EPD_cmd(EPD_SPI_FLASH_CONTROL);
    EPD_data(1);
    delay_us(1000);
    lutBuffer = malloc(len);
    if (lutBuffer) spi_soft_read_buffer(0, adr, lutBuffer, len);
    delay_us(1000);
    EPD_cmd(EPD_SPI_FLASH_CONTROL);
    EPD_data(0);
    return lutBuffer;
}

void shiftRightByX(uint8_t *data, int X, int N) {
    if (X < 0 || X >= 8) {
        // Invalid shift value, X should be between 0 and 7 (inclusive)
        return;
    }

    // Perform the shift operation on each byte in the range
    for (int i = 0; i < N; i++) {
        data[i] = (data[i] >> X) | ((data[i + 1] & ((1 << X) - 1)) << (8 - X));
    }
}
void shiftLeftByX(uint8_t *data, int X, int N) {
    if (X < 0 || X >= 8) {
        // Invalid shift value, X should be between 0 and 7 (inclusive)
        return;
    }

    // Perform the shift operation on each byte in the range
    for (int i = N - 1; i >= 0; i--) {
        data[i] = (data[i] << X) | ((data[i - 1] >> (8 - X)) & ((1 << X) - 1));
    }
}

uint16_t loadCharacter(uint8_t currentChar, uint16_t curX, bool first) {
    currentChar -= 0x20;

    memset(charbuffer, 0, sizeof(charbuffer));
    for (uint8_t d = 0; d < CHAR_HEIGHT; d++) {
        for (uint8_t c = 0; c < CHAR_WIDTH_BYTES; c++) {
            charbuffer[d][c] = font[currentChar][c + (2 * d)];
        }
    }

    // find amount of left whitespace and compensate
    uint8_t leftShift = 0;
    for (uint8_t left = 0; left < CHAR_WIDTH; left++) {
        bool leftAdjusted = false;
        for (uint8_t height = 0; height < CHAR_HEIGHT; height++) {
            if (charbuffer[height][0] & 0x01) {
                leftAdjusted = true;
                break;
            }
        }
        if (leftAdjusted) {
            break;
        } else {
            for (uint8_t height = 0; height < CHAR_HEIGHT; height++) {
                shiftRightByX(&(charbuffer[height][0]), 1, CHAR_WIDTH_BYTES);
            }
            leftShift++;
        }
    }

    // find width for character
    uint8_t width = 0;
    for (int8_t curBit = CHAR_WIDTH + 1; curBit > 0; curBit--) {
        bool widthFound = false;
        for (uint8_t height = 0; height < CHAR_HEIGHT; height++) {
            if (charbuffer[height][curBit / 8] & (1 << (curBit % 8))) {
                widthFound = true;
                break;
            }
        }
        if (widthFound) {
            width = curBit + 1;
            break;
        }
    }

    if (!first) {
        curX += CHAR_SPACING;
    }

    for (uint8_t height = 0; height < CHAR_HEIGHT; height++) {
        shiftLeftByX(&(charbuffer[height][0]), curX % 8, CHAR_WIDTH_BYTES + 1);
    }

    for (uint8_t d = 0; d < CHAR_HEIGHT; d++) {
        for (uint8_t c = 0; c < CHAR_WIDTH_BYTES + 1; c++) {
            buffer[d][(curX / 8) + c] |= charbuffer[d][c];
        }
    }
    if (width == 0) width = EMPTY_SPACING;
    curX += width;
    return curX;
}

void dumpBuffer(uint16_t xloc, uint16_t yloc, uint16_t width) {
    xloc = SCREEN_WIDTH - xloc;

    setDisplayWindow(xloc - width, yloc, xloc, yloc + CHAR_HEIGHT);
    display_send_start(0);
    for (uint8_t curY = 0; curY < CHAR_HEIGHT; curY++) {
        for (uint16_t curX = 0; curX < width; curX += 8) {
            interleaveBW(buffer[curY][curX / 8]);
        }
    }
    display_send_stop();
}

void loadLUTSfromEEPROM(uint8_t bracket) {
    for (uint8_t c = EPD_LUT_B; c <= EPD_LUT_R3; c++) {
        struct epd_colorlut *colorlut = (struct epd_colorlut *)loadLUT(c, bracket);

        for (uint8_t d = 0; d < 8; d++) {
            colorlut->part[1].length[d] = 1;
            colorlut->part[0].length[d] = 1;
        }

        for (uint8_t part = 0; part < 4; part++) {
            if (colorlut->part[part].repeat) colorlut->part[part].repeat = 1;
        }

        for (uint8_t part = 4; part < 20; part++) {
            if (colorlut->part[part].repeat) colorlut->part[part].repeat = 0;
        }

        lutBeginTX(c);
        for (uint16_t d = 0; d < 260; d++) {
            display_tx_byte(((uint8_t *)colorlut)[d]);
        }
        lutEndTX();
        if (colorlut) free(colorlut);
    }

    struct epd_vcomlut *vcomlut = (struct epd_vcomlut *)loadLUT(EPD_LUT_VCOM, bracket);

    for (uint8_t part = 0; part < 20; part++) {
        if (vcomlut->part[part].repeat) vcomlut->part[part].repeat = 0;
    }
    lutBeginTX(EPD_LUT_VCOM);
    for (uint16_t d = 0; d < 220; d++) {
        display_tx_byte(((uint8_t *)vcomlut)[d]);
    }
    lutEndTX();
    if (vcomlut) free(vcomlut);

    struct epd_xonlut *xonlut = (struct epd_xonlut *)loadLUT(EPD_LUT_XON, bracket);

    // memset(&(xonlut->part[0].repeat), 0x00, sizeof(struct epd_xonlutpart));
    // memset(&(xonlut->part[1].repeat), 0x00, sizeof(struct epd_xonlutpart));

    for (uint8_t part = 0; part < 20; part++) {
        if (xonlut->part[part].repeat) xonlut->part[part].repeat = 0;
    }
    lutBeginTX(EPD_LUT_XON);
    for (uint16_t d = 0; d < 200; d++) {
        display_tx_byte(((uint8_t *)xonlut)[d]);
    }
    lutEndTX();
    if (xonlut) free(xonlut);
}

void init_GPIO_EPD() {
    SSP_CFG_Type v0;
    SPI_Param_Type spiParaStruct;
    GPIO_PinMuxFun(EPD_MOSI, 0);
    GPIO_SetPinDir(EPD_MOSI, GPIO_OUTPUT);
    GPIO_PinMuxFun(EPD_CLK, 0);
    GPIO_SetPinDir(EPD_CLK, GPIO_OUTPUT);
    GPIO_PinMuxFun(EPD_CS, 0);
    GPIO_SetPinDir(EPD_CS, GPIO_OUTPUT);
    GPIO_WritePinOutput(EPD_CS, GPIO_IO_HIGH);
    GPIO_PinMuxFun(EPD_BUSY, 0);
    GPIO_SetPinDir(EPD_BUSY, GPIO_INPUT);
    GPIO_PinModeConfig(EPD_BUSY, PINMODE_PULLUP);
    GPIO_PinMuxFun(EPD_RESET, 0);
    GPIO_SetPinDir(EPD_RESET, GPIO_OUTPUT);
    GPIO_WritePinOutput(EPD_RESET, GPIO_IO_HIGH);
    GPIO_PinMuxFun(EPD_DC, 0);
    GPIO_SetPinDir(EPD_DC, GPIO_OUTPUT);
    GPIO_WritePinOutput(EPD_DC, GPIO_IO_HIGH);
    GPIO_PinMuxFun(EPD_BS, 0);
    GPIO_SetPinDir(EPD_BS, GPIO_OUTPUT);
    GPIO_WritePinOutput(EPD_BS, GPIO_IO_LOW);
    GPIO_PinMuxFun(EPD_HLT_CTRL, 0);
    GPIO_SetPinDir(EPD_HLT_CTRL, GPIO_OUTPUT);
    GPIO_WritePinOutput(EPD_HLT_CTRL, GPIO_IO_HIGH);
    GPIO_PinMuxFun(EPD_MISO, 0);
    GPIO_SetPinDir(EPD_MISO, GPIO_INPUT);
    GPIO_PinModeConfig(EPD_MISO, PINMODE_DEFAULT);
    memset(&v0, 0, 9);
    v0.timeOutVal = 0;
    SSP_Init(SSP2_ID, &v0);
    spiParaStruct.spiClkPhase = SPI_SCPHA_1;
    spiParaStruct.spiClkPolarity = SPI_SCPOL_LOW;
    SPI_Config(SSP2_ID, &spiParaStruct);
    CLK_I2SClkSrc(CLK_I2S_XTAL32M);
    CLK_SSPClkSrc(CLK_SSP_ID_2, CLK_SSP_I2S);
    CLK_I2SClkDivider(1, 1);
}

void fillWindow(uint16_t x, uint16_t y, uint16_t xe, uint16_t ye, uint8_t color) {
    setDisplayWindow(x, y, xe, ye);
    display_send_start(0);

    for (uint32_t c = 0; c < (xe - x) * (ye - y) / 8; c++) {
        interleaveBW(0x00);
    }

    display_send_stop();
}

void epd_refresh_and_sleep(uint8_t lut) {
    if (lut) {
        epdWrite(EPD_PANEL_SETTING, 2, 0xC3, 0x88);  // 0xC3-0x88 // lut from register
        loadLUTSfromEEPROM(EPDtempBracket);
    } else {
        epdWrite(EPD_PANEL_SETTING, 2, 0xC3, 0x08);  // 0xC3-0x88 // lut from EEPROM
        epdWrite(EPD_UNKNOWN_1, 1, 0x03);            // load lut, probably
    }

    // epdPrintf(50,100,false,"Blaat! Dit is een test %d", 6);

    EPD_cmd(EPD_REFRESH);

    unsigned int v2 = 0;
    while (GPIO_ReadPinLevel(EPD_BUSY) == GPIO_IO_HIGH) {
        WDT_RestartCounter();
        delay(10000);
        v2++;
        if (v2 > 10)
            break;
    }
    do_sleeped_epd_refresh();
    init_GPIO_EPD();
    epd_reset();
    //    epd_reset();
    EPD_cmd(EPD_POWER_SETTING);
    EPD_data(2);
    EPD_data(0);
    EPD_data(0);
    EPD_data(0);
    delay_us(50000);
    EPD_cmd(EPD_POWER_OFF);
    delay_us(100000);
    BUSY_wait(0x32u);
    EPD_cmd(EPD_SPI_FLASH_CONTROL);
    EPD_data(1);
    GPIO_WritePinOutput(EPD_HLT_CTRL, GPIO_IO_LOW);
    spi_soft_send_byte(EPD_EEPROM_SLEEP);
    GPIO_WritePinOutput(EPD_HLT_CTRL, GPIO_IO_HIGH);
    EPD_cmd(EPD_SPI_FLASH_CONTROL);
    EPD_data(0);
    EPD_cmd(EPD_DEEP_SLEEP);
    EPD_data(0xA5);
}

void epd_pin_enable(int a1) {
    if (a1) {
        GPIO_PinMuxFun(EPD_CLK, GPIO22_SSP2_SCK);
        GPIO_PinMuxFun(EPD_MOSI, GPIO12_SSP2_TXD);
        GPIO_PinMuxFun(EPD_MISO, GPIO13_SSP2_RXD);
        SSP_Enable(SSP2_ID);
    } else {
        SSP_Disable(SSP2_ID);
        GPIO_PinMuxFun(EPD_MOSI, 0);
        GPIO_SetPinDir(EPD_MOSI, GPIO_OUTPUT);
        GPIO_PinMuxFun(EPD_CLK, 0);
        GPIO_SetPinDir(EPD_CLK, GPIO_OUTPUT);
        GPIO_PinMuxFun(EPD_MISO, 0);
        GPIO_SetPinDir(EPD_MISO, GPIO_INPUT);
        GPIO_PinModeConfig(EPD_MISO, PINMODE_DEFAULT);
    }
}

void lutBeginTX(uint8_t reg) {
    EPD_cmd(reg);
    epd_pin_enable(1);
    GPIO_WritePinOutput(EPD_CS, GPIO_IO_LOW);
}

void lutEndTX() {
    GPIO_WritePinOutput(EPD_CS, GPIO_IO_HIGH);
    epd_pin_enable(0);
}

void setDisplayWindow(uint16_t x, uint16_t y, uint16_t xe, uint16_t ye) {
    x &= 0xFFF8;             // byte boundary
    xe = (xe - 1) | 0x0007;  // byte boundary - 1
    EPD_cmd(0x91);
    epdWrite(0x90, 9, x / 256, x % 256, xe / 256, xe % 256, y / 256, y % 256, ye / 256, ye % 256, 0x01);
}

void display_tx_byte(uint8_t data) {
    SSP_SendData(SSP2_ID, data);
}

void display_send_start(uint8_t inverted) {
    EPD_cmd(EPD_START_DATA);
    epd_pin_enable(1);
    GPIO_WritePinOutput(EPD_CS, GPIO_IO_LOW);
}

void display_send_stop() {
    for (int i = 0; i < 0xF; ++i)
        __ISB();
    GPIO_WritePinOutput(EPD_CS, GPIO_IO_HIGH);
    epd_pin_enable(0);
}

void init_epd(void) {
    printf("EPD Powerup begin\n");
    init_GPIO_EPD();
    epd_reset();

    EPD_cmd(EPD_POWER_ON);
    BUSY_wait(0x32u);

    // wake the eeprom
    epdWrite(EPD_SPI_FLASH_CONTROL, 1, 0x01);
    GPIO_WritePinOutput(EPD_HLT_CTRL, GPIO_IO_LOW);
    epdWrite(EPD_WAKE_EEPROM, 0x00);
    GPIO_WritePinOutput(EPD_HLT_CTRL, GPIO_IO_HIGH);
    epdWrite(EPD_SPI_FLASH_CONTROL, 1, 0x00);

    epdWrite(EPD_POWER_SETTING, 4, 0x37, 0x00, 0x05, 0x05);  // 0x37 - 00- 05 05
    // epdWrite(EPD_POWER_SETTING, 4, 0x07, 0x00, 0x05, 0x05);  // 0x37 - 00- 05 05

    epdWrite(EPD_PANEL_SETTING, 2, 0xC3, 0x08);  // 0xC3-0x88 // lut from EEPROM

    epdWrite(EPD_POWER_OFF_SEQUENCE, 1, 0x00);
    epdWrite(EPD_BOOSTER_SOFT_START, 0x03, 199, 204, 45);

    epdWrite(EPD_PLL_CONTROL, 0x01, 60);
    epdWrite(EPD_TEMP_SELECT, 0x01, 0x00);
    epdWrite(EPD_VCOM_DATA_INTERVAL, 0x01, 119);
    epdWrite(EPD_TCON_SET, 0x01, 34);
    epdWrite(EPD_TRES, 0x04, 2, 128, 1, 128);

    EPDtempBracket = getTempBracket();
    loadFrameRatePLL(EPDtempBracket);
    loadTempVCOMDC(EPDtempBracket);
}

void epdPrintf(uint16_t x, uint16_t y, bool color, const char *c, ...) {
    // Render the text
    char out_buffer[96];

    uint16_t curX = 0;
    memset(buffer, 0, sizeof(buffer));
    memset(charbuffer, 0, sizeof(charbuffer));
    va_list lst;
    va_start(lst, c);
    vsnprintf(out_buffer, 256, c, lst);
    va_end(lst);

    curX = x % 8;

    char *text = (char *)out_buffer;
    memset(charbuffer, 0, sizeof(charbuffer));
    curX = loadCharacter(*text, curX, true);
    text++;

    while (*text != '\0') {
        memset(charbuffer, 0, sizeof(charbuffer));
        curX = loadCharacter(*text, curX, false);
        text++;
    }

    x /= 8;
    x *= 8;
    dumpBuffer(x, y, curX);
}

void drawImg(uint16_t x, uint16_t y, const uint8_t *img) {
    uint16_t width = img[0];
    uint16_t height = img[1];
    img += 2;
    setDisplayWindow(x, y, x + width, y + height);
    display_send_start(0);
    for (uint8_t curY = 0; curY < height; curY++) {
        for (uint16_t curX = 0; curX < width; curX += 8) {
            interleaveColor(*(img++), 0x00);
        }
    }
    display_send_stop();
}