#include "main.h"

#include "uc8176-var-m2.h"

extern "C" {

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "mz100/gpio.h"
#include "mz100/mz100_gpio.h"
#include "mz100/mz100_pinmux.h"
#include "mz100/printf.h"
#include "mz100/util.h"
}

#include "settings.h"
#include "drawing.h"
#include "epd_interface.h"

#define EPD_PANEL_SETTING 0x00
#define EPD_POWER_SETTING 0x01
#define EPD_POWER_OFF 0x02
#define EPD_POWER_OFF_SEQUENCE 0x03
#define EPD_POWER_ON 0x04
#define EPD_BOOSTER_SOFT_START 0x06
#define EPD_DEEP_SLEEP 0x07
#define EPD_START_DTM1 0x10
#define EPD_DATA_STOP 0x11
#define EPD_REFRESH 0x12
#define EPD_START_DTM2 0x13
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

void uc8176::epdSetup() {
#ifdef DEBUG_EPD
    printf("EPD: Init begin\n");
#endif
    initEPDGPIO();
    this->epdReset();
    epdWrite(0x01, 2, 0x03, 0x00);
    epdWrite(0x06, 3, 0x17, 0x17, 0x17);
    //epdWrite(0x30, 1, 0x39);  // 4hz Framerate < no setup for this in stock
    epdWrite(0x60,1, 0x11); // tcon non-overlap
    epdWrite(EPD_POWER_ON, 0);
    busyWaitUntil(EPD_IS_NOT_BUSY, 100);
    delay(10000);
    epdWrite(0x61, 4, 0x01, 0x90, 0x01, 0x2C);
    epdWrite(0x50, 1, 0x87);  // 0x87 correct for polarity and border -  0x8C = 5 interval
    epdWrite(0x00, 1, 0x0F);  // was 0x07
    epdWrite(0x41, 1, 0x0F);
    epdWrite(0x40, 0);
    busyWaitUntil(EPD_IS_NOT_BUSY, 10000);
    delay(1000);

#ifdef DEBUG_EPD
    printf("EPD: Init complete\n");
#endif
}

void uc8176::selectLUT(uint8_t lut) {
    this->drawLut = lut;
}

uc8176::~uc8176() {
}

void uc8176::epdReset() {
    uint8_t v0 = 5;
#ifdef DEBUG_EPD
    printf("EPD: Reset... ");
#endif
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
#ifdef DEBUG_EPD
    printf("complete.\n");
#endif
}

void uc8176::epdWriteDisplayData() {
    uint16_t byteWidth = this->effectiveXRes / 8;
    uint8_t screenrow[byteWidth];

    epdWrite(EPD_START_DTM1, 0);
    enableHardSPI(true);
    GPIO_WritePinOutput(EPD_CS, GPIO_IO_LOW);
    for (uint16_t curY = 0; curY < this->effectiveYRes; curY += 1) {  //
        memset(screenrow, 0, byteWidth);
        drawItem::renderDrawLine(screenrow, curY, 0);
        epdBlockWrite(screenrow, byteWidth);
    }
    GPIO_WritePinOutput(EPD_CS, GPIO_IO_HIGH);
    enableHardSPI(false);

    epdWrite(EPD_START_DTM2, 0);
    enableHardSPI(true);
    GPIO_WritePinOutput(EPD_CS, GPIO_IO_LOW);
    for (uint16_t curY = 0; curY < this->effectiveYRes; curY += 1) {  //
        memset(screenrow, 0, byteWidth);
        drawItem::renderDrawLine(screenrow, curY, 1);
        epdBlockWrite(screenrow, byteWidth);
    }
    GPIO_WritePinOutput(EPD_CS, GPIO_IO_HIGH);
    enableHardSPI(false);

    drawItem::flushDrawItems();
}

void uc8176::draw() {
    this->drawNoWait();
    this->epdWaitRdy();
}

void uc8176::drawNoWait() {
#ifdef DEBUG_EPD
    printf("EPD: Draw start\n");
#endif
    epdWriteDisplayData();

    wdt10s();
#ifdef DEBUG_EPD
    printf("EPD: Refresh start\n");
#endif
    epdWrite(EPD_REFRESH, 0);
    busyWaitUntil(EPD_IS_BUSY, 10);
}

void uc8176::epdWaitRdy() {
    delay(15000);
    do_sleeped_epd_refresh();
    //   delay(150000);
    // busyWaitUntil(!EPD_IS_BUSY, 300000);
#ifdef DEBUG_EPD
    printf("EPD: Draw done!\n");
#endif
    delay(1000);
}

void uc8176::epdEnterSleep() {
    initEPDGPIO();
    this->epdReset();
    epdWrite(EPD_POWER_OFF, 0);
    delay_us(100000);
    busyWaitUntil(EPD_IS_NOT_BUSY, 100000);
    epdWrite(EPD_DEEP_SLEEP, 1, 0xA5);
}
