#include "ssd1619.h"

#include <Arduino.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lut.h"
#include "settings.h"
#include "hal.h"
#include "wdt.h"

#include "drawing.h"

#define CMD_DRV_OUTPUT_CTRL 0x01
#define CMD_SOFT_START_CTRL 0x0C
#define CMD_ENTER_SLEEP 0x10
#define CMD_DATA_ENTRY_MODE 0x11
#define CMD_SOFT_RESET 0x12
#define CMD_SOFT_RESET2 0x13
#define CMD_SETUP_VOLT_DETECT 0x15
#define CMD_TEMP_SENSOR_CONTROL 0x18
#define CMD_ACTIVATION 0x20
#define CMD_DISP_UPDATE_CTRL 0x21
#define CMD_DISP_UPDATE_CTRL2 0x22
#define CMD_WRITE_FB_BW 0x24
#define CMD_WRITE_FB_RED 0x26
#define CMD_VCOM_GLITCH_CTRL 0x2B
#define CMD_LOAD_OTP_LUT 0x31
#define CMD_WRITE_LUT 0x32
#define CMD_BORDER_WAVEFORM_CTRL 0x3C
#define CMD_WINDOW_X_SIZE 0x44
#define CMD_WINDOW_Y_SIZE 0x45
#define CMD_WRITE_PATTERN_RED 0x46
#define CMD_WRITE_PATTERN_BW 0x47
#define CMD_XSTART_POS 0x4E
#define CMD_YSTART_POS 0x4F
#define CMD_ANALOG_BLK_CTRL 0x74
#define CMD_DIGITAL_BLK_CTRL 0x7E

#define SCREEN_CMD_CLOCK_ON 0x80
#define SCREEN_CMD_CLOCK_OFF 0x01
#define SCREEN_CMD_ANALOG_ON 0x40
#define SCREEN_CMD_ANALOG_OFF 0x02
#define SCREEN_CMD_LATCH_TEMPERATURE_VAL 0x20
#define SCREEN_CMD_LOAD_LUT 0x10
#define SCREEN_CMD_USE_MODE_2 0x08  // modified commands 0x10 and 0x04
#define SCREEN_CMD_REFRESH 0xC7

#define commandEnd()                \
    do {                            \
        digitalWrite(EPD_CS, HIGH); \
    } while (0)

#define markCommand()              \
    do {                           \
        digitalWrite(EPD_DC, LOW); \
    } while (0)

#define markData()                  \
    do {                            \
        digitalWrite(EPD_DC, HIGH); \
    } while (0)

extern void dump(const uint8_t *a, const uint16_t l);  // remove me when done

static uint8_t currentLut = 0;
uint8_t dispLutSize = 0;  // we'll need to expose this in the 'capabilities' flag

static bool isInited = false;

#define LUT_BUFFER_SIZE 256
static uint8_t waveformbuffer[LUT_BUFFER_SIZE];
uint8_t customLUT[LUT_BUFFER_SIZE] = {0};

struct waveform10 *waveform10 = (struct waveform10 *)waveformbuffer;  // holds the LUT/waveform
struct waveform *waveform7 = (struct waveform *)waveformbuffer;       // holds the LUT/waveform
struct waveform12 *waveform12 = (struct waveform12 *)waveformbuffer;  // holds the LUT/waveform

static void commandReadBegin(uint8_t cmd) {
    epdSelect();
    markCommand();
    spi_write(cmd);
    pinMode(EPD_MOSI, INPUT);
    markData();
}
static void commandReadEnd() {
    // set up pins for spi (0.0,0.1,0.2)
    pinMode(EPD_MOSI, OUTPUT);
    epdDeselect();
}
static uint8_t epdReadByte() {
    uint8_t val = 0, i;

    for (i = 0; i < 8; i++) {
        digitalWrite(EPD_CLK, HIGH);
        delayMicroseconds(1);
        val <<= 1;
        if (digitalRead(EPD_MOSI))
            val++;
        digitalWrite(EPD_CLK, LOW);
        delayMicroseconds(1);
    }

    return val;
}
static void shortCommand(uint8_t cmd) {
    epdSelect();
    markCommand();
    spi_write(cmd);
    epdDeselect();
}
static void shortCommand1(uint8_t cmd, uint8_t arg) {
    epdSelect();
    markCommand();
    spi_write(cmd);
    markData();
    spi_write(arg);
    epdDeselect();
}
static void shortCommand2(uint8_t cmd, uint8_t arg1, uint8_t arg2) {
    epdSelect();
    markCommand();
    spi_write(cmd);
    markData();
    spi_write(arg1);
    spi_write(arg2);
    epdDeselect();
}
static void commandBegin(uint8_t cmd) {
    epdSelect();
    markCommand();
    spi_write(cmd);
    markData();
}

void setWindowX(uint16_t start, uint16_t end) {
    epdWrite(CMD_WINDOW_X_SIZE, 2, start / 8, end / 8 - 1);
}
void setWindowY(uint16_t start, uint16_t end) {
    epdWrite(CMD_WINDOW_Y_SIZE, 4, (start)&0xff, (start) >> 8, (end - 1) & 0xff, (end - 1) >> 8);
}
void setPosXY(uint16_t x, uint16_t y) {
    epdWrite(CMD_XSTART_POS, 1, (uint8_t)(x / 8));
    epdWrite(CMD_YSTART_POS, 2, (y)&0xff, (y) >> 8);
}

void epdEnterSleep() {
    digitalWrite(EPD_RST, LOW);
    delay(10);
    digitalWrite(EPD_RST, HIGH);
    delay(50);
    shortCommand(CMD_SOFT_RESET2);
    epdBusyWaitFalling(15);
    shortCommand1(CMD_ENTER_SLEEP, 0x03);
    isInited = false;
}
void epdSetup() {
    epdReset();
    shortCommand(CMD_SOFT_RESET);  // software reset
    delay(10);
    shortCommand(CMD_SOFT_RESET2);
    delay(10);
    epdWrite(CMD_ANALOG_BLK_CTRL, 1, 0x54);
    epdWrite(CMD_DIGITAL_BLK_CTRL, 1, 0x3B);
    epdWrite(CMD_VCOM_GLITCH_CTRL, 2, 0x04, 0x63);
    epdWrite(CMD_DRV_OUTPUT_CTRL, 3, (SCREEN_HEIGHT - 1) & 0xff, (SCREEN_HEIGHT - 1) >> 8, 0x00);
    epdWrite(CMD_DISP_UPDATE_CTRL, 2, 0x08, 0x00);
    epdWrite(CMD_BORDER_WAVEFORM_CTRL, 1, 0x01);
    epdWrite(CMD_TEMP_SENSOR_CONTROL, 1, 0x80);
    epdWrite(CMD_DISP_UPDATE_CTRL2, 1, 0xB1);
    epdWrite(CMD_ACTIVATION, 0);
    epdBusyWaitFalling(10000);
    isInited = true;
    currentLut = EPD_LUT_DEFAULT;
}
static uint8_t epdGetStatus() {
    uint8_t sta;
    commandReadBegin(0x2F);
    sta = epdReadByte();
    commandReadEnd();
    return sta;
}

void loadFixedTempOTPLUT() {
    shortCommand1(0x18, 0x48);                   // external temp sensor
    shortCommand2(0x1A, 0x05, 0x00);             // < temp register
    shortCommand1(CMD_DISP_UPDATE_CTRL2, 0xB1);  // mode 1 (i2C)
    shortCommand(CMD_ACTIVATION);
    epdBusyWaitFalling(1000);
}
static void writeLut() {
    commandBegin(CMD_WRITE_LUT);
    if (dispLutSize == 12) {
        for (uint8_t i = 0; i < 153; i++)
            epdSend(waveformbuffer[i]);
    } else {
        for (uint8_t i = 0; i < (dispLutSize * 10); i++)
            epdSend(waveformbuffer[i]);
    }
    commandEnd();
}
static void readLut() {
    commandReadBegin(0x33);
    for (uint16_t c = 0; c < LUT_BUFFER_SIZE; c++) {
        waveformbuffer[c] = epdReadByte();
    }
    commandReadEnd();
}
static uint8_t getLutSize() {
    uint8_t ref = 0;
    for (uint8_t c = (LUT_BUFFER_SIZE - 4); c > 16; c--) {
        uint8_t check = waveformbuffer[c];
        for (uint8_t d = 1; d < 4; d++) {
            if (waveformbuffer[c + d] != check) {
                ref = c;
                goto end;
            }
        }
    }
end:;
    return ref + 1;
}
static void lutGroupDisable(uint8_t group) {
    switch (dispLutSize) {
        case 7:
            memset(&(waveform7->group[group]), 0x00, 5);
            break;
        case 10:
            memset(&(waveform10->group[group]), 0x00, 5);
            break;
        case 12:
            memset(&(waveform12->group[group]), 0x00, sizeof(waveform12->group[0]));
            break;
    }
}
static void lutGroupSpeedup(uint8_t group, uint8_t speed) {
    switch (dispLutSize) {
        case 7:
            for (uint8_t i = 0; i < 4; i++) {
                waveform7->group[group].phaselength[i] = 1 + (waveform7->group[group].phaselength[i] / speed);
            }
            break;
        case 10:
            for (uint8_t i = 0; i < 4; i++) {
                waveform10->group[group].phaselength[i] = 1 + (waveform10->group[group].phaselength[i] / speed);
            }
            break;
        case 12:
            waveform12->group[group].tp0a = 1 + (waveform12->group[group].tp0a / speed);
            waveform12->group[group].tp0b = 1 + (waveform12->group[group].tp0b / speed);
            waveform12->group[group].tp0c = 1 + (waveform12->group[group].tp0c / speed);
            waveform12->group[group].tp0d = 1 + (waveform12->group[group].tp0d / speed);
            break;
    }
}
static void lutGroupRepeat(uint8_t group, uint8_t repeat) {
    switch (dispLutSize) {
        case 7:
            waveform7->group[group].repeat = repeat;
            break;
        case 10:
            waveform10->group[group].repeat = repeat;
            break;
        case 12:
            waveform12->group[group].repeat = repeat;
            break;
    }
}
static void lutGroupRepeatReduce(uint8_t group, uint8_t factor) {
    switch (dispLutSize) {
        case 7:
            waveform7->group[group].repeat = waveform7->group[group].repeat / factor;
            break;
        case 10:
            waveform10->group[group].repeat = waveform10->group[group].repeat / factor;
            break;
        case 12:
            waveform12->group[group].repeat = waveform12->group[group].repeat / factor;
            break;
    }
}
void selectLUT(uint8_t lut) {
    if (currentLut == lut) {
        // return;
    }

    // Handling if we received an OTA LUT
    if (lut == EPD_LUT_OTA) {
        memcpy(waveformbuffer, customLUT, dispLutSize * 10);
        writeLut();
        currentLut = lut;
        return;
    }

    if (currentLut != EPD_LUT_DEFAULT) {
        // load the 'default' LUT for the current temperature in the EPD lut register
        shortCommand1(CMD_DISP_UPDATE_CTRL2, 0xB1);  // mode 1?
        shortCommand(CMD_ACTIVATION);
        epdBusyWaitFalling(1000);
    }

    currentLut = lut;

    // if we're going to be using the default LUT, we're done here.
    if (lut == EPD_LUT_DEFAULT) {
        return;
    }

    // download the current LUT from the waveform buffer
    readLut();

    if (dispLutSize == 0) {
        dispLutSize = getLutSize();
        dispLutSize /= 10;
        printf("lut size = %d\n", dispLutSize);
        dispLutSize = 12;
#ifdef PRINT_LUT
        dump(waveformbuffer, LUT_BUFFER_SIZE);
#endif
        memcpy(customLUT, waveformbuffer, LUT_BUFFER_SIZE);
    }

    switch (lut) {
        default:
        case EPD_LUT_NO_REPEATS:
            lutGroupDisable(LUTGROUP_NEGATIVE);
            lutGroupDisable(LUTGROUP_FASTBLINK);
            lutGroupRepeat(LUTGROUP_SLOWBLINK, 0);
            lutGroupSpeedup(LUTGROUP_SET, 2);
            lutGroupSpeedup(LUTGROUP_IMPROVE_SHARPNESS, 2);
            lutGroupRepeatReduce(LUTGROUP_IMPROVE_SHARPNESS, 2);
            lutGroupSpeedup(LUTGROUP_IMPROVE_REDS, 2);
            lutGroupRepeatReduce(LUTGROUP_IMPROVE_REDS, 2);
            lutGroupDisable(LUTGROUP_UNUSED);
            break;
        case EPD_LUT_FAST_NO_REDS:
            lutGroupDisable(LUTGROUP_NEGATIVE);
            lutGroupDisable(LUTGROUP_FASTBLINK);
            lutGroupDisable(LUTGROUP_SLOWBLINK);
            lutGroupSpeedup(LUTGROUP_SET, 2);
            lutGroupDisable(LUTGROUP_IMPROVE_REDS);
            lutGroupDisable(LUTGROUP_IMPROVE_SHARPNESS);
            lutGroupDisable(LUTGROUP_UNUSED);
            break;
        case EPD_LUT_FAST:
            lutGroupDisable(LUTGROUP_NEGATIVE);
            lutGroupDisable(LUTGROUP_FASTBLINK);
            lutGroupDisable(LUTGROUP_SLOWBLINK);
            lutGroupRepeat(LUTGROUP_SET, 1);
            lutGroupSpeedup(LUTGROUP_SET, 2);
            lutGroupDisable(LUTGROUP_IMPROVE_SHARPNESS);
            lutGroupDisable(LUTGROUP_IMPROVE_REDS);
            lutGroupDisable(LUTGROUP_UNUSED);
            break;
    }

    if (dispLutSize == 10) {
        lutGroupDisable(LUTGROUP_UNUSED);
        lutGroupDisable(LUTGROUP_UNKNOWN);
        lutGroupDisable(LUTGROUP_UNUSED3);
        lutGroupDisable(LUTGROUP_UNUSED4);
    }
    writeLut();
}

void epdWriteDisplayData() {
    setWindowX(SCREEN_XOFFSET, SCREEN_WIDTH + SCREEN_XOFFSET);
    setPosXY(SCREEN_XOFFSET, 0);
    // epdWrite(CMD_DISP_UPDATE_CTRL, 1, 0x08);

    // this display expects two entire framebuffers worth of data to be written, one for b/w and one for red
    uint8_t *buf[2] = {0, 0};  // this will hold pointers to odd/even data lines
    for (uint8_t c = 0; c < 2; c++) {
        if (c == 0) epd_cmd(CMD_WRITE_FB_BW);
        if (c == 1) epd_cmd(CMD_WRITE_FB_RED);
        markData();
        epdSelect();
        for (uint16_t curY = 0; curY < SCREEN_HEIGHT; curY += 2) {
            // Get 'even' screen line
            buf[0] = (uint8_t *)calloc(SCREEN_WIDTH / 8, 1);
            drawItem::renderDrawLine(buf[0], curY, c);

            // on the first pass, the second (buf[1]) buffer is unused, so we don't have to wait for it to flush to the display / free it
            if (buf[1]) {
                // wait for 'odd' display line to finish writing to the screen
                epdSPIWait();
                free(buf[1]);
            }

            // start transfer of even data line to the screen
            epdSPIAsyncWrite(buf[0], (SCREEN_WIDTH / 8));

            // Get 'odd' screen display line
            buf[1] = (uint8_t *)calloc(SCREEN_WIDTH / 8, 1);
            drawItem::renderDrawLine(buf[1], curY + 1, c);

            // wait until the 'even' data has finished writing
            epdSPIWait();
            free(buf[0]);

            // start transfer of the 'odd' data line
            epdSPIAsyncWrite(buf[1], (SCREEN_WIDTH / 8));
        }
        // check if this was the first pass. If it was, we'll need to wait until the last display line finished writing
        if (c == 0) {
            epdSPIWait();
            epdDeselect();
            free(buf[1]);
            buf[1] = nullptr;
        }
    }
    // flush the draw list, make sure items don't appear on subsequent screens
    drawItem::flushDrawItems();

    // wait until the last line of display has finished writing and clean our stuff up
    epdSPIWait();
    epdDeselect();
    if (buf[1]) free(buf[1]);
}

void draw() {
    drawNoWait();
    getVoltage();
    epdBusyWaitFalling(120000);
}
void drawNoWait() {
    epdWriteDisplayData();
    epdWrite(0x22, 1, 0xF7);
    epdWrite(0x20, 0);
}
void epdWaitRdy() {
    epdBusyWaitFalling(120000);
}