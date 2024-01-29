#include "ssd1619.h"

#include <stdbool.h>
#include <string.h>

#include "asmUtil.h"
#include "barcode.h"
#include "board.h"
#include "cpu.h"
#include "font.h"
#include "lut.h"
#include "printf.h"
#include "screen.h"
// #include "settings.h"
#include "sleep.h"
#include "spi.h"
#include "timer.h"
#include "wdt.h"

#include <stdlib.h>

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
#define CMD_UNKNOWN_1 0x2B
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
// added for OTA LUT-support
#define CMD_GATE_LEVEL 0x03
#define CMD_SOURCE_LEVEL 0x04
#define CMD_DUMMY_PERIOD 0x3A
#define CMD_GATE_LINE_WIDTH 0x3B

#define SCREEN_CMD_CLOCK_ON 0x80
#define SCREEN_CMD_CLOCK_OFF 0x01
#define SCREEN_CMD_ANALOG_ON 0x40
#define SCREEN_CMD_ANALOG_OFF 0x02
#define SCREEN_CMD_LATCH_TEMPERATURE_VAL 0x20
#define SCREEN_CMD_LOAD_LUT 0x10
#define SCREEN_CMD_USE_MODE_2 0x08  // modified commands 0x10 and 0x04
#define SCREEN_CMD_REFRESH 0xC7

#define commandEnd() \
    do {             \
        P1_7 = 1;    \
    } while (0)

#define markCommand() \
    do {              \
        P2_2 = 0;     \
    } while (0)

#define markData() \
    do {           \
        P2_2 = 1;  \
    } while (0)

static uint8_t __xdata epdCharSize = 1;   // character size, 1 or 2 (doubled)
static bool __xdata directionY = true;    // print direction, X or Y (true)
static uint8_t __xdata rbuffer[32];       // used to rotate bits around
static uint16_t __xdata fontCurXpos = 0;  // current X value we're working with
static uint16_t __xdata fontCurYpos = 0;  // current Y value we're working with
static uint8_t __xdata currentLut = 0;
uint8_t __xdata dispLutSize = 0;  // we'll need to expose this in the 'capabilities' flag

static bool __xdata isInited = false;

bool __xdata epdGPIOActive = false;

#define LUT_BUFFER_SIZE 128
uint8_t __xdata customLUT[LUT_BUFFER_SIZE] = {0};

static uint8_t* waveformbuffer;
static struct waveform10* __xdata waveform10;
static struct waveform* __xdata waveform7;

#pragma callee_saves epdBusySleep
#pragma callee_saves epdBusyWait
static void epdBusySleep(uint32_t timeout) {
    uint8_t tmp_P2FUNC = P2FUNC;
    uint8_t tmp_P2DIR = P2DIR;
    uint8_t tmp_P2PULL = P2PULL;
    uint8_t tmp_P2LVLSEL = P2LVLSEL;
    P2FUNC &= 0xfd;
    P2DIR |= 2;
    P2PULL |= 2;
    P2LVLSEL |= 2;

    P2CHSTA &= 0xfd;
    P2INTEN |= 2;
    P2CHSTA &= 0xfd;
    sleepForMsec(timeout);
    wdtOn();
    P2CHSTA &= 0xfd;
    P2INTEN &= 0xfd;

    P2FUNC = tmp_P2FUNC;
    P2DIR = tmp_P2DIR;
    P2PULL = tmp_P2PULL;
    P2LVLSEL = tmp_P2LVLSEL;
    eepromPrvDeselect();
}
static void epdBusyWait(uint32_t timeout) {
    uint32_t __xdata start = timerGet();

    while (timerGet() - start < timeout) {
        if (!P2_1)
            return;
    }
    pr("screen timeout %lu ticks :(\n", timerGet() - start);
    while (1)
        ;
}
static void commandReadBegin(uint8_t cmd) {
    epdSelect();
    markCommand();
    spiByte(cmd);  // dump LUT

    P0DIR = (P0DIR & ~(1 << 0)) | (1 << 1);
    P0 &= ~(1 << 0);
    P0FUNC &= ~((1 << 0) | (1 << 1));
    P2_2 = 1;
}
static void commandReadEnd() {
    // set up pins for spi (0.0,0.1,0.2)
    P0FUNC |= (1 << 0) | (1 << 1);
    epdDeselect();
}
#pragma callee_saves epdReadByte
static uint8_t epdReadByte() {
    uint8_t val = 0, i;

    for (i = 0; i < 8; i++) {
        P0_0 = 1;
        __asm__("nop\nnop\nnop\nnop\nnop\nnop\n");
        val <<= 1;
        if (P0_1)
            val++;
        P0_0 = 0;
        __asm__("nop\nnop\nnop\nnop\nnop\nnop\n");
    }

    return val;
}
static void shortCommand(uint8_t cmd) {
    epdSelect();
    markCommand();
    spiTXByte(cmd);
    epdDeselect();
}
static void shortCommand1(uint8_t cmd, uint8_t arg) {
    epdSelect();
    markCommand();
    spiTXByte(cmd);
    markData();
    spiTXByte(arg);
    epdDeselect();
}
static void shortCommand2(uint8_t cmd, uint8_t arg1, uint8_t arg2) {
    epdSelect();
    markCommand();
    spiTXByte(cmd);
    markData();
    spiTXByte(arg1);
    spiTXByte(arg2);
    epdDeselect();
}
static void commandBegin(uint8_t cmd) {
    epdSelect();
    markCommand();
    spiTXByte(cmd);
    markData();
}
static void epdReset() {
    timerDelay(TIMER_TICKS_PER_SECOND / 1000);
    P2_0 = 0;
    timerDelay(TIMER_TICKS_PER_SECOND / 1000);
    P2_0 = 1;
    timerDelay(TIMER_TICKS_PER_SECOND / 1000);

    shortCommand(CMD_SOFT_RESET);  // software reset
    timerDelay(TIMER_TICKS_PER_SECOND / 500);
    shortCommand(CMD_SOFT_RESET2);
    timerDelay(TIMER_TICKS_PER_SECOND / 500);
}
void epdConfigGPIO(bool setup) {
    // data / _command: 2.2
    // busy             2.1
    // reset            2.0
    // _select          1.7
    // bs1              1.2

    // GENERIC SPI BUS PINS
    // spi.clk          0.0
    // spi.mosi         0.1
    if (epdGPIOActive == setup) return;
    if (setup) {
        P2DIR |= (1 << 1);                // busy as input
        P2DIR &= ~((1 << 2) | (1 << 0));  // D/C and Reset as output
        P1DIR &= ~((1 << 7) | (1 << 2));  // select and bs1 as output
        P1_2 = 0;                         // select 4-wire SPI / BS1 = low
        P1_7 = 1;                         // deselect EPD
    } else {
        P2DIR |= ((1 << 2) | (1 << 0));  // DC and Reset as input
        P2 &= ~((1 << 2) | (1 << 0));
        P1DIR |= ((1 << 7) | (1 << 2));  // Select and BS1 as input
        P2 &= ~((1 << 7));
    }
    epdGPIOActive = setup;
}
void epdEnterSleep() {
    P2_0 = 0;
    timerDelay(10);
    P2_0 = 1;
    timerDelay(50);
    shortCommand(CMD_SOFT_RESET2);
    epdBusyWait(TIMER_TICKS_PER_MS * 150);
    shortCommand1(CMD_ENTER_SLEEP, 0x03);
    isInited = false;
}
void epdSetup() {
        epdReset();
    #ifdef VAR_BW16
    commandBegin(CMD_DRV_OUTPUT_CTRL);
    epdSend((SCREEN_HEIGHT - 1) & 0xff);
    epdSend((SCREEN_HEIGHT - 1) >> 8);
    epdSend(0x00);
    commandEnd();
    shortCommand1(CMD_BORDER_WAVEFORM_CTRL, 0x0B); // stock says 0x33
    shortCommand1(CMD_DUMMY_PERIOD, 0x1B);
    shortCommand1(CMD_GATE_LINE_WIDTH, 0x0B);
    shortCommand1(CMD_TEMP_SENSOR_CONTROL, 0x80);
    shortCommand1(CMD_DISP_UPDATE_CTRL, 0x40);
    #else 
    shortCommand1(CMD_ANALOG_BLK_CTRL, 0x54);
    shortCommand1(CMD_DIGITAL_BLK_CTRL, 0x3B);
    shortCommand2(CMD_UNKNOWN_1, 0x04, 0x63);
    commandBegin(CMD_SOFT_START_CTRL);
    epdSend(0x8f);
    epdSend(0x8f);
    epdSend(0x8f);
    epdSend(0x3f);
    commandEnd();
    commandBegin(CMD_DRV_OUTPUT_CTRL);
    epdSend((SCREEN_HEIGHT - 1) & 0xff);
    epdSend((SCREEN_HEIGHT - 1) >> 8);
    epdSend(0x00);
    commandEnd();
    shortCommand1(CMD_BORDER_WAVEFORM_CTRL, 0x01);
    shortCommand1(CMD_TEMP_SENSOR_CONTROL, 0x80);
    shortCommand1(CMD_DISP_UPDATE_CTRL2, 0xB1);  // mode 1 (i2C)
    shortCommand(CMD_ACTIVATION);
    epdBusyWait(TIMER_TICKS_PER_SECOND);
    #endif
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
uint16_t epdGetBattery(void) {
    uint16_t voltage = 2600;
    uint8_t val;

    timerDelay(50);
    P2_0 = 0;
    timerDelay(50);
    P2_0 = 1;
    timerDelay(50);

    shortCommand(CMD_SOFT_RESET);  // software reset
    epdBusyWait(TIMER_TICKS_PER_MS * 30);
    shortCommand(CMD_SOFT_RESET2);
    epdBusyWait(TIMER_TICKS_PER_MS * 30);

    shortCommand1(CMD_DISP_UPDATE_CTRL2, SCREEN_CMD_CLOCK_ON | SCREEN_CMD_ANALOG_ON);
    shortCommand(CMD_ACTIVATION);
    epdBusyWait(TIMER_TICKS_PER_MS * 150);

    for (val = 3; val < 8; val++) {
        shortCommand1(CMD_SETUP_VOLT_DETECT, val);
        epdBusyWait(TIMER_TICKS_PER_MS * 150);
        if (epdGetStatus() & 0x10) {  // set if voltage is less than threshold ( == 1.9 + val / 10)
            voltage = 1850 + mathPrvMul8x8(val, 100);
            break;
        }
    }

    shortCommand(CMD_SOFT_RESET2);
    epdBusyWait(TIMER_TICKS_PER_MS * 15);
    shortCommand1(CMD_ENTER_SLEEP, 0x03);

    return voltage;
}

static void writeLut() {
    commandBegin(CMD_WRITE_LUT);
    for (uint8_t i = 0; i < (dispLutSize * 10); i++)
        epdSend(waveformbuffer[i]);
    commandEnd();
}
static void readLut() {
    commandReadBegin(0x33);
    uint16_t checksum = 0;
    uint16_t ident = 0;
    uint16_t shortl = 0;
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
    if (dispLutSize == 7) {
        memset(&(waveform7->group[group]), 0x00, 5);
    } else {
        memset(&(waveform10->group[group]), 0x00, 5);
    }
}
static void lutGroupSpeedup(uint8_t group, uint8_t speed) {
    if (dispLutSize == 7) {
        for (uint8_t i = 0; i < 4; i++) {
            waveform7->group[group].phaselength[i] = 1 + (waveform7->group[group].phaselength[i] / speed);
        }
    } else {
        for (uint8_t i = 0; i < 4; i++) {
            waveform10->group[group].phaselength[i] = 1 + (waveform10->group[group].phaselength[i] / speed);
        }
    }
}
static void lutGroupRepeat(uint8_t group, uint8_t repeat) {
    if (dispLutSize == 7) {
        waveform7->group[group].repeat = repeat;
    } else {
        waveform10->group[group].repeat = repeat;
    }
}
static void lutGroupRepeatReduce(uint8_t group, uint8_t factor) {
    if (dispLutSize == 7) {
        waveform7->group[group].repeat = waveform7->group[group].repeat / factor;
    } else {
        waveform10->group[group].repeat = waveform10->group[group].repeat / factor;
    }
}
void selectLUT(uint8_t lut) {
    #ifdef CUSTOMLUTS
    if (currentLut == lut) {
        return;
    }

    if (currentLut != EPD_LUT_DEFAULT) {
        // load the 'default' LUT for the current temperature in the EPD lut register
        shortCommand1(CMD_DISP_UPDATE_CTRL2, 0xB1);  // mode 1?
        shortCommand(CMD_ACTIVATION);
        epdBusyWait(TIMER_TICKS_PER_SECOND);
    }

    currentLut = lut;

    // if we're going to be using the default LUT, we're done here.
    if (lut == EPD_LUT_DEFAULT) {
        return;
    }

    waveformbuffer = malloc(150);
    waveform10 = (struct waveform10*)waveformbuffer;  // holds the LUT/waveform
    waveform7 = (struct waveform*)waveformbuffer;     // holds the LUT/waveform

    // download the current LUT from the waveform buffer
    readLut();

    if (dispLutSize == 0) {
        dispLutSize = getLutSize();
        dispLutSize /= 10;
        #ifdef DEBUGEPD
        pr("lut size = %d\n", dispLutSize);
        #endif
#ifdef PRINT_LUT
        dump(waveformbuffer, LUT_BUFFER_SIZE);
#endif
        memcpy(customLUT, waveformbuffer, dispLutSize * 10);
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

    // Handling if we received an OTA LUT
    if (lut == EPD_LUT_OTA) {
        memcpy(waveformbuffer, customLUT, dispLutSize * 10);
        writeLut();
        shortCommand1(CMD_GATE_LEVEL, customLUT[70]);
        commandBegin(CMD_SOURCE_LEVEL);
        epdSend(customLUT[71]);
        epdSend(customLUT[72]);
        epdSend(customLUT[73]);
        commandEnd();
        shortCommand1(CMD_DUMMY_PERIOD, customLUT[74]);
        shortCommand1(CMD_GATE_LINE_WIDTH, customLUT[75]);
        currentLut = lut;
        free(waveformbuffer);
        return;
    }

    if (dispLutSize == 10) {
        lutGroupDisable(LUTGROUP_UNUSED);
        lutGroupDisable(LUTGROUP_UNKNOWN);
        lutGroupDisable(LUTGROUP_UNUSED3);
        lutGroupDisable(LUTGROUP_UNUSED4);
    }
    writeLut();
    free(waveformbuffer);
    #endif
}

void setWindowX(uint16_t start, uint16_t end) {
    shortCommand2(CMD_WINDOW_X_SIZE, start / 8, end / 8 - 1);
}
void setWindowY(uint16_t start, uint16_t end) {
    commandBegin(CMD_WINDOW_Y_SIZE);
    epdSend((start) & 0xff);
    epdSend((start) >> 8);
    epdSend((end - 1) & 0xff);
    epdSend((end - 1) >> 8);
    commandEnd();
}
void setPosXY(uint16_t x, uint16_t y) {
    shortCommand1(CMD_XSTART_POS, (uint8_t)(x / 8));
    commandBegin(CMD_YSTART_POS);
    epdSend((y) & 0xff);
    epdSend((y) >> 8);
    commandEnd();
}
void setColorMode(uint8_t red, uint8_t bw) {
    shortCommand1(CMD_DISP_UPDATE_CTRL, (red << 4) | bw);
}
void fillWindowWithPattern(bool color) {
    if (color == EPD_COLOR_RED) {
        shortCommand1(CMD_WRITE_PATTERN_RED, 0x00);
    } else {
        shortCommand1(CMD_WRITE_PATTERN_BW, 0x00);
    }
}
void clearWindow(bool color) {
    if (color == EPD_COLOR_RED) {
        shortCommand1(CMD_WRITE_PATTERN_RED, 0x66);
    } else {
        shortCommand1(CMD_WRITE_PATTERN_BW, 0x66);
    }
}
void clearScreen() {
    setWindowX(0, SCREEN_WIDTH);
    setWindowY(0, SCREEN_HEIGHT);
    setPosXY(0, 0);
    shortCommand1(CMD_DATA_ENTRY_MODE, 3);  // was 3
    shortCommand1(CMD_WRITE_PATTERN_BW, 0x66);
    epdBusyWait(TIMER_TICKS_PER_MS * 100);
    shortCommand1(CMD_WRITE_PATTERN_RED, 0x66);
    epdBusyWait(TIMER_TICKS_PER_MS * 100);
}
void draw() {
    #ifdef VAR_BW16
    shortCommand1(0x22, 0xF7);
    #else 
    shortCommand1(0x22, 0xCF);
    #endif
    // shortCommand1(0x22, SCREEN_CMD_REFRESH);
    shortCommand(0x20);
    epdBusyWait(TIMER_TICKS_PER_SECOND * 120);
}
void drawNoWait() {
    shortCommand1(0x22, 0xCF);
    // shortCommand1(0x22, SCREEN_CMD_REFRESH);
    shortCommand(0x20);
}
void drawWithSleep() {
    shortCommand1(0x22, 0xCF);
    // shortCommand1(0x22, SCREEN_CMD_REFRESH);
    shortCommand(0x20);
    uint8_t tmp_P2FUNC = P2FUNC;
    uint8_t tmp_P2DIR = P2DIR;
    uint8_t tmp_P2PULL = P2PULL;
    uint8_t tmp_P2LVLSEL = P2LVLSEL;
    P2FUNC &= 0xfd;
    P2DIR |= 2;
    P2PULL |= 2;
    P2LVLSEL |= 2;

    P2CHSTA &= 0xfd;
    P2INTEN |= 2;
    P2CHSTA &= 0xfd;
    sleepForMsec(TIMER_TICKS_PER_SECOND * 120);
    wdtOn();
    P2CHSTA &= 0xfd;
    P2INTEN &= 0xfd;

    P2FUNC = tmp_P2FUNC;
    P2DIR = tmp_P2DIR;
    P2PULL = tmp_P2PULL;
    P2LVLSEL = tmp_P2LVLSEL;
    eepromPrvDeselect();
}
void epdWaitRdy() {
    epdBusyWait(TIMER_TICKS_PER_SECOND * 120);
}
void drawLineHorizontal(bool color, uint16_t x1, uint16_t x2, uint16_t y) {
    setWindowX(x1, x2);
    setWindowY(y, y + 1);
    if (color) {
        shortCommand1(CMD_WRITE_PATTERN_RED, 0xE6);
    } else {
        shortCommand1(CMD_WRITE_PATTERN_BW, 0xE6);
    }
    epdBusyWait(TIMER_TICKS_PER_MS * 100);
}
void drawLineVertical(bool color, uint16_t x, uint16_t y1, uint16_t y2) {
    setWindowY(y1, y2);
    setWindowX(x, x + 8);
    shortCommand1(CMD_DATA_ENTRY_MODE, 3);
    setPosXY(x, y1);
    if (color) {
        commandBegin(CMD_WRITE_FB_RED);
    } else {
        commandBegin(CMD_WRITE_FB_BW);
    }
    uint8_t __xdata c = 0x80;
    c >>= (x % 8);
    for (; y1 < y2; y1++) {
        epdSend(c);
    }
    commandEnd();
}
void beginFullscreenImage() {
    setColorMode(EPD_MODE_NORMAL, EPD_MODE_INVERT);
    setWindowX(0, SCREEN_WIDTH);
    setWindowY(0, SCREEN_HEIGHT);
    shortCommand1(CMD_DATA_ENTRY_MODE, 3);
    setPosXY(0, 0);
}
void beginWriteFramebuffer(bool color) {
    if (color == EPD_COLOR_RED) {
        commandBegin(CMD_WRITE_FB_RED);
    } else {
        commandBegin(CMD_WRITE_FB_BW);
    }
    epdDeselect();
}
void endWriteFramebuffer() {
    commandEnd();
}
void loadRawBitmap(uint8_t* bmp, uint16_t x, uint16_t y, bool color) {
    uint16_t xsize = bmp[0] / 8;
    if (bmp[0] % 8) xsize++;
    uint16_t size = xsize * bmp[1];
    setWindowX(x, x + (xsize * 8));
    setWindowY(y, bmp[1] + y);
    setPosXY(x, y);
    shortCommand1(CMD_DATA_ENTRY_MODE, 3);
    if (color) {
        commandBegin(CMD_WRITE_FB_RED);
    } else {
        commandBegin(CMD_WRITE_FB_BW);
    }
    bmp += 2;
    while (size--) {
        epdSend(*(bmp++));
    }
    commandEnd();
}
void printBarcode(const uint8_t* string, uint16_t x, uint16_t y) {
    setWindowY(y, 1);
    setWindowX(x, x + 8);
    setPosXY(x, y);
    shortCommand1(CMD_DATA_ENTRY_MODE, 1);
    commandBegin(CMD_WRITE_FB_BW);
    struct BarcodeInfo __xdata bci = {
        .str = string,
    };
    while (!barcodeIsDone(&bci)) {
        if (barcodeNextBar(&bci)) {
            epdSend(0xFF);
        } else {
            epdSend(0x00);
        }
    }
    commandEnd();
}
// stuff for printing text
static void pushXFontBytesToEPD(uint8_t byte1, uint8_t byte2) {
    if (epdCharSize == 1) {
        uint8_t offset = 7 - (fontCurXpos % 8);
        for (uint8_t c = 0; c < 8; c++) {
            if (byte2 & (1 << (7 - c))) rbuffer[c] |= (1 << offset);
        }
        for (uint8_t c = 0; c < 8; c++) {
            if (byte1 & (1 << (7 - c))) rbuffer[8 + c] |= (1 << offset);
        }
        fontCurXpos++;
    } else {
        uint8_t offset = 6 - (fontCurXpos % 8);
        // double font size
        for (uint8_t c = 0; c < 8; c++) {
            if (byte2 & (1 << (7 - c))) {
                rbuffer[c * 2] |= (3 << offset);
                rbuffer[(c * 2) + 1] |= (3 << offset);
            }
        }
        for (uint8_t c = 0; c < 8; c++) {
            if (byte1 & (1 << (7 - c))) {
                rbuffer[(c * 2) + 16] |= (3 << offset);
                rbuffer[(c * 2) + 17] |= (3 << offset);
            }
        }
        fontCurXpos += 2;
    }
    if (fontCurXpos % 8 == 0) {
        // next byte, flush current byte to EPD
        for (uint8_t i = 0; i < (16 * epdCharSize); i++) {
            epdSend(rbuffer[i]);
        }
        memset(rbuffer, 0, 32);
    }
}
static void bufferByteShift(uint8_t byte) {
    /*
                rbuffer[0] = 0;   // previous value
                rbuffer[1] = y%8; // offset
                rbuffer[2] = 0;   // current byte counter;
                rbuffer[3] = 1+(epdCharsize*2);
    */

    if (rbuffer[1] == 0) {
        epdSend(byte);
    } else {
        uint8_t offset = rbuffer[1];
        rbuffer[0] |= (byte >> offset);
        epdSend(rbuffer[0]);
        // epdSend(byte);
        rbuffer[0] = (byte << (8 - offset));
        rbuffer[2]++;
        if (rbuffer[2] == rbuffer[3]) {
            epdSend(rbuffer[0]);
            rbuffer[0] = 0;
            rbuffer[2] = 0;
        }
    }
}
static void pushYFontBytesToEPD(uint8_t byte1, uint8_t byte2) {
    if (epdCharSize == 2) {
        for (uint8_t j = 0; j < 2; j++) {
            uint8_t c = 0;
            for (uint8_t i = 7; i != 255; i--) {
                if (byte1 & (1 << i)) c |= (0x03 << ((i % 4) * 2));
                if ((i % 4) == 0) {
                    bufferByteShift(c);
                    c = 0;
                }
            }
            for (uint8_t i = 7; i != 255; i--) {
                if (byte2 & (1 << i)) c |= (0x03 << ((i % 4) * 2));
                if ((i % 4) == 0) {
                    bufferByteShift(c);
                    c = 0;
                }
            }
        }
    } else {
        bufferByteShift(byte1);
        bufferByteShift(byte2);
    }
}
void writeCharEPD(uint8_t c) {
    c-=0x20;

    // Writes a single character to the framebuffer
    bool empty = true;
    for (uint8_t i = 0; i < 20; i++) {
        if (font[c][i]) empty = false;
    }
    if (empty) {
        for (uint8_t i = 0; i < 8; i++) {
            if (directionY) {
                pushYFontBytesToEPD(0x00, 0x00);
            } else {
                pushXFontBytesToEPD(0x00, 0x00);
            }
        }
        return;
    }

    uint8_t begin = 0;
    while (font[c][begin] == 0x00 && font[c][begin + 1] == 0x00) {
        begin += 2;
    }

    uint8_t end = 20;
    while (font[c][end - 1] == 0x00 && font[c][end - 2] == 0x00) {
        end -= 2;
    }

    for (uint8_t pos = begin; pos < end; pos += 2) {
        if (directionY) {
            pushYFontBytesToEPD(font[c][pos + 1], font[c][pos]);
        } else {
            pushXFontBytesToEPD(font[c][pos], font[c][pos + 1]);
        }
    }

    // spacing between characters
    if (directionY) {
        pushYFontBytesToEPD(0x00, 0x00);
    } else {
        pushXFontBytesToEPD(0x00, 0x00);
    }
}

// Print text to the EPD. Origin is top-left
void epdPrintBegin(uint16_t x, uint16_t y, bool direction, bool fontsize, bool color) {
    directionY = direction;
    epdCharSize = 1 + fontsize;
    if (directionY) {
        uint8_t extra = 0;

        // provisions for dealing with font in Y direction, byte-unaligned
        if (x % 8) {
            extra = 8;
            rbuffer[0] = 0;      // previous value
            rbuffer[1] = x % 8;  // offset
            rbuffer[2] = 0;      // current byte counter;
            rbuffer[3] = (epdCharSize * 2);
        } else {
            rbuffer[1] = 0;
        }

        setWindowY(y, 1);
        if (epdCharSize == 2) {
            setWindowX(x, x + 32 + extra);
            setPosXY(x, y);
        } else {
            setWindowX(x, x + 16 + extra);
            setPosXY(x, y);
        }
        shortCommand1(CMD_DATA_ENTRY_MODE, 1);  // was 3
    } else {
        if (epdCharSize == 2) {
            x /= 2;
            x *= 2;
            setWindowY(y, y + 32);
        } else {
            setWindowY(y, y + 16);
        }
        setPosXY(x, y);
        fontCurXpos = x;
        setWindowX(x, SCREEN_WIDTH);
        shortCommand1(CMD_DATA_ENTRY_MODE, 7);
        memset(rbuffer, 0, 32);
    }

    if (color) {
        commandBegin(CMD_WRITE_FB_RED);
    } else {
        commandBegin(CMD_WRITE_FB_BW);
    }
}
void epdPrintEnd() {
    if (!directionY && ((fontCurXpos % 8) != 0)) {
        for (uint8_t i = 0; i < (16 * epdCharSize); i++) {
            epdSend(rbuffer[i]);
        }
    }
    commandEnd();
}