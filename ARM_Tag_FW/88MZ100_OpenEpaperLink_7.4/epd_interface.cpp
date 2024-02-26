#include "drawing.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "epd_interface.h"
#include "uc8159-var-m2.h"
#include "uc8176-var-m2.h"

#include "mz100/printf.h"
#include "mz100/eeprom.h"

#include "main.h"

extern "C" {
#include "mz100/mz100_clock.h"

#include "mz100/mz100_gpio.h"
#include "mz100/mz100_pinmux.h"
#include "mz100/mz100_ssp.h"
#include "mz100/mz100_wdt.h"
#include "mz100/util.h"
}

#include "settings.h"

#include "../../oepl-definitions.h"
#include "../../oepl-proto.h"

__attribute__((section(".aonshadow"))) epdInterface *epd;
__attribute__((section(".aonshadow"))) tagSpecs tag;
epdInterface::~epdInterface() {
}

void epdSetup() {
    switch (tagProfile.controllerType) {
        case 0:
            epd = new uc8159;
            break;
        case 1:
            epd = new uc8176;
            break;
    }

    epd->effectiveXRes = tagProfile.xRes;
    epd->effectiveYRes = tagProfile.yRes;
    epd->Xres = tagProfile.xRes;
    epd->Yres = tagProfile.yRes;
    epd->bpp = tagProfile.bpp;
    epd->epdSetup();
}

void epdEnterSleep() {
    epd->epdEnterSleep();
    delete epd;
}
void draw() {
    epd->draw();
}
void drawNoWait() {
    epd->drawNoWait();
}

void epdWaitRdy() {
    epd->epdWaitRdy();
}

void selectLUT(uint8_t sel) {
    epd->selectLUT(sel);
}

static void busyWaitUntilHigh(uint32_t timeout) {
    uint32_t v2 = 0;
    while (GPIO_ReadPinLevel(EPD_BUSY) == GPIO_IO_LOW) {
        delay(50);
        v2++;
        if (v2 > timeout)
            break;
        if (((v2 % 1000) / 10) == 0)
            WDT_RestartCounter();
    }
}

static void busyWaitUntilLow(uint32_t timeout) {
    uint32_t v2 = 0;
    while (GPIO_ReadPinLevel(EPD_BUSY) == GPIO_IO_HIGH) {
        delay(50);
        v2++;
        if (v2 > timeout)
            break;
        if (((v2 % 1000) / 10) == 0)
            WDT_RestartCounter();
    }
}

void busyWaitUntil(bool high, uint32_t timeout) {
    if (high)
        busyWaitUntilHigh(timeout);
    else
        busyWaitUntilLow(timeout);
}

void softSPIWriteByte(char byteOut) {
    uint8_t loopCount = 0;
    do {
        if ((byteOut & 0x80) != 0)
            GPIO_WritePinOutput(EPD_MOSI, GPIO_IO_HIGH);
        else
            GPIO_WritePinOutput(EPD_MOSI, GPIO_IO_LOW);
        delay_us(1);
        GPIO_WritePinOutput(EPD_CLK, GPIO_IO_HIGH);
        delay_us(1);
        GPIO_WritePinOutput(EPD_CLK, GPIO_IO_LOW);
        byteOut *= 2;
        loopCount++;
        delay_us(1);
    } while (loopCount < 8);
    GPIO_WritePinOutput(EPD_MOSI, GPIO_IO_LOW);
    delay_us(1);
}

uint8_t softSPIReadByte() {
    uint8_t readByte = 0;
    uint8_t loopCount = 0;
    GPIO_WritePinOutput(EPD_BS, GPIO_IO_HIGH);
    GPIO_WritePinOutput(EPD_CS, GPIO_IO_LOW);
    delay_us(1);
    GPIO_WritePinOutput(EPD_MOSI, GPIO_IO_HIGH);
    GPIO_WritePinOutput(EPD_CLK, GPIO_IO_HIGH);
    delay_us(1);
    GPIO_WritePinOutput(EPD_CLK, GPIO_IO_LOW);
    delay_us(1);
    GPIO_SetPinDir(EPD_MOSI, GPIO_INPUT);
    GPIO_PinMuxFun(EPD_MOSI, PINMUX_FUNCTION_0);
    delay_us(3);
    do {
        readByte *= 2;
        GPIO_WritePinOutput(EPD_CLK, GPIO_IO_HIGH);
        if (GPIO_ReadPinLevel(EPD_MOSI))
            readByte |= 1u;
        GPIO_WritePinOutput(EPD_CLK, GPIO_IO_LOW);
        delay_us(1);
        // delay(1);
        loopCount++;
    } while (loopCount < 8);
    GPIO_SetPinDir(EPD_MOSI, GPIO_OUTPUT);
    GPIO_PinMuxFun(EPD_MOSI, PINMUX_FUNCTION_0);
    GPIO_WritePinOutput(EPD_MOSI, GPIO_IO_LOW);
    delay_us(1);
    GPIO_WritePinOutput(EPD_CS, GPIO_IO_HIGH);
    delay_us(1);
    GPIO_WritePinOutput(EPD_BS, GPIO_IO_LOW);
    // delay(1);
    delay_us(1);

    return readByte;
}

void enableHardSPI(bool enable) {
    if (enable) {
        GPIO_PinMuxFun(EPD_CLK, GPIO22_SSP2_SCK);
        GPIO_PinMuxFun(EPD_MOSI, GPIO12_SSP2_TXD);
        GPIO_PinMuxFun(EPD_MISO, GPIO13_SSP2_RXD);
        SSP_Enable(SSP2_ID);
    } else {
        SSP_Disable(SSP2_ID);
        GPIO_PinMuxFun(EPD_MOSI, PINMUX_FUNCTION_0);
        GPIO_SetPinDir(EPD_MOSI, GPIO_OUTPUT);
        GPIO_PinMuxFun(EPD_CLK, PINMUX_FUNCTION_0);
        GPIO_SetPinDir(EPD_CLK, GPIO_OUTPUT);
        GPIO_PinMuxFun(EPD_MISO, PINMUX_FUNCTION_0);
        GPIO_SetPinDir(EPD_MISO, GPIO_INPUT);
        GPIO_PinModeConfig(EPD_MISO, PINMODE_DEFAULT);
    }
}

void initEPDGPIO() {
    SSP_CFG_Type v0;
    SPI_Param_Type spiParaStruct;
    GPIO_PinMuxFun(EPD_MOSI, PINMUX_FUNCTION_0);
    GPIO_SetPinDir(EPD_MOSI, GPIO_OUTPUT);
    GPIO_PinMuxFun(EPD_CLK, PINMUX_FUNCTION_0);
    GPIO_SetPinDir(EPD_CLK, GPIO_OUTPUT);
    GPIO_PinMuxFun(EPD_CS, PINMUX_FUNCTION_0);
    GPIO_SetPinDir(EPD_CS, GPIO_OUTPUT);
    GPIO_WritePinOutput(EPD_CS, GPIO_IO_HIGH);
    GPIO_PinMuxFun(EPD_BUSY, PINMUX_FUNCTION_0);
    GPIO_SetPinDir(EPD_BUSY, GPIO_INPUT);
    GPIO_PinModeConfig(EPD_BUSY, PINMODE_PULLUP);
    GPIO_PinMuxFun(EPD_RESET, PINMUX_FUNCTION_0);
    GPIO_SetPinDir(EPD_RESET, GPIO_OUTPUT);
    GPIO_WritePinOutput(EPD_RESET, GPIO_IO_HIGH);
    GPIO_PinMuxFun(EPD_DC, PINMUX_FUNCTION_0);
    GPIO_SetPinDir(EPD_DC, GPIO_OUTPUT);
    GPIO_WritePinOutput(EPD_DC, GPIO_IO_HIGH);
    GPIO_PinMuxFun(EPD_BS, PINMUX_FUNCTION_0);
    GPIO_SetPinDir(EPD_BS, GPIO_OUTPUT);
    GPIO_WritePinOutput(EPD_BS, GPIO_IO_LOW);
    GPIO_PinMuxFun(EPD_HLT_CTRL, PINMUX_FUNCTION_0);
    GPIO_SetPinDir(EPD_HLT_CTRL, GPIO_OUTPUT);
    GPIO_WritePinOutput(EPD_HLT_CTRL, GPIO_IO_HIGH);
    GPIO_PinMuxFun(EPD_MISO, PINMUX_FUNCTION_0);
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

void epdWrite(uint8_t reg, uint8_t len, ...) {
    va_list valist;
    va_start(valist, len);
    enableHardSPI(true);
    GPIO_WritePinOutput(EPD_CS, GPIO_IO_LOW);
    GPIO_WritePinOutput(EPD_DC, GPIO_IO_LOW);

    SSP_SendData(SSP2_ID, reg);
    while (SSP_GetTxFifoLevel(SSP2_ID))
        ;

    GPIO_WritePinOutput(EPD_DC, GPIO_IO_HIGH);

    for (uint8_t i = 0; i < len; i++) {
        SSP_SendData(SSP2_ID, va_arg(valist, int));
    }
    while (SSP_GetTxFifoLevel(SSP2_ID))
        ;
    delay_us(1);
    GPIO_WritePinOutput(EPD_CS, GPIO_IO_HIGH);
    enableHardSPI(false);
    va_end(valist);
}
void epdBlockWrite(uint8_t reg, uint8_t *buffer, uint16_t len) {
    enableHardSPI(true);
    GPIO_WritePinOutput(EPD_CS, GPIO_IO_LOW);
    GPIO_WritePinOutput(EPD_DC, GPIO_IO_LOW);

    SSP_SendData(SSP2_ID, reg);
    while (SSP_GetTxFifoLevel(SSP2_ID))
        ;

    GPIO_WritePinOutput(EPD_DC, GPIO_IO_HIGH);

    for (uint16_t i = 0; i < len; i++) {
        SSP_SendData(SSP2_ID, buffer[i]);
        while (SSP_GetTxFifoLevel(SSP2_ID) > 8)
            ;
    }
    while (SSP_GetTxFifoLevel(SSP2_ID))
        ;
    delay_us(1);
    GPIO_WritePinOutput(EPD_CS, GPIO_IO_HIGH);
    enableHardSPI(false);
}
void epdBlockWrite(uint8_t *buffer, uint16_t len) {
    enableHardSPI(true);
    GPIO_WritePinOutput(EPD_CS, GPIO_IO_LOW);
    GPIO_WritePinOutput(EPD_DC, GPIO_IO_HIGH);

    for (uint16_t i = 0; i < len; i++) {
        SSP_SendData(SSP2_ID, buffer[i]);
        while (SSP_GetTxFifoLevel(SSP2_ID) > 8)
            ;
    }
    while (SSP_GetTxFifoLevel(SSP2_ID))
        ;
    delay_us(1);
    GPIO_WritePinOutput(EPD_CS, GPIO_IO_HIGH);
    enableHardSPI(false);
}
