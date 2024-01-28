#include "uc8151.h"

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
#include "settings.h"
#include "sleep.h"
#include "spi.h"
#include "timer.h"
#include "wdt.h"
#include "stdarg.h"

#define CMD_PANEL_SETTING 0x00
#define CMD_POWER_SETTING 0x01
#define CMD_POWER_OFF 0x02
#define CMD_POWER_OFF_SEQUENCE 0x03
#define CMD_POWER_ON 0x04
#define CMD_POWER_ON_MEASURE 0x05
#define CMD_BOOSTER_SOFT_START 0x06
#define CMD_DEEP_SLEEP 0x07
#define CMD_DISPLAY_START_TRANSMISSION_DTM1 0x10
#define CMD_DATA_STOP 0x11
#define CMD_DISPLAY_REFRESH 0x12
#define CMD_DISPLAY_START_TRANSMISSION_DTM2 0x13
#define CMD_PLL_CONTROL 0x30
#define CMD_TEMPERATURE_CALIB 0x40
#define CMD_TEMPERATURE_SELECT 0x41
#define CMD_TEMPERATURE_WRITE 0x42
#define CMD_TEMPERATURE_READ 0x43
#define CMD_VCOM_INTERVAL 0x50
#define CMD_LOWER_POWER_DETECT 0x51
#define CMD_TCON_SETTING 0x60
#define CMD_RESOLUTION_SETING 0x61
#define CMD_REVISION 0x70
#define CMD_STATUS 0x71
#define CMD_AUTO_MEASUREMENT_VCOM 0x80
#define CMD_READ_VCOM 0x81
#define CMD_VCOM_DC_SETTING 0x82
#define CMD_PARTIAL_WINDOW 0x90
#define CMD_PARTIAL_IN 0x91
#define CMD_PARTIAL_OUT 0x92
#define CMD_PROGRAM_MODE 0xA0
#define CMD_ACTIVE_PROGRAM 0xA1
#define CMD_READ_OTP 0xA2
#define CMD_CASCADE_SET 0xE0
#define CMD_POWER_SAVING 0xE3
#define CMD_FORCE_TEMPERATURE 0xE5

enum PSR_FLAGS {
    RES_96x230 = 0b00000000,
    RES_96x252 = 0b01000000,
    RES_128x296 = 0b10000000,
    RES_160x296 = 0b11000000,

    LUT_OTP = 0b00000000,
    LUT_REG = 0b00100000,

    FORMAT_BWR = 0b00000000,
    FORMAT_BW = 0b00010000,

    SCAN_DOWN = 0b00000000,
    SCAN_UP = 0b00001000,

    SHIFT_LEFT = 0b00000000,
    SHIFT_RIGHT = 0b00000100,

    BOOSTER_OFF = 0b00000000,
    BOOSTER_ON = 0b00000010,

    RESET_SOFT = 0b00000000,
    RESET_NONE = 0b00000001
};

enum PWR_FLAGS_1 {
    VDS_EXTERNAL = 0b00000000,
    VDS_INTERNAL = 0b00000010,

    VDG_EXTERNAL = 0b00000000,
    VDG_INTERNAL = 0b00000001
};

enum PWR_FLAGS_2 {
    VCOM_VD = 0b00000000,
    VCOM_VG = 0b00000100,

    VGHL_16V = 0b00000000,
    VGHL_15V = 0b00000001,
    VGHL_14V = 0b00000010,
    VGHL_13V = 0b00000011
};

enum BOOSTER_FLAGS {
    START_10MS = 0b00000000,
    START_20MS = 0b01000000,
    START_30MS = 0b10000000,
    START_40MS = 0b11000000,

    STRENGTH_1 = 0b00000000,
    STRENGTH_2 = 0b00001000,
    STRENGTH_3 = 0b00010000,
    STRENGTH_4 = 0b00011000,
    STRENGTH_5 = 0b00100000,
    STRENGTH_6 = 0b00101000,
    STRENGTH_7 = 0b00110000,
    STRENGTH_8 = 0b00111000,

    OFF_0_27US = 0b00000000,
    OFF_0_34US = 0b00000001,
    OFF_0_40US = 0b00000010,
    OFF_0_54US = 0b00000011,
    OFF_0_80US = 0b00000100,
    OFF_1_54US = 0b00000101,
    OFF_3_34US = 0b00000110,
    OFF_6_58US = 0b00000111
};

enum PFS_FLAGS {
    FRAMES_1 = 0b00000000,
    FRAMES_2 = 0b00010000,
    FRAMES_3 = 0b00100000,
    FRAMES_4 = 0b00110000
};

enum TSE_FLAGS {
    TEMP_INTERNAL = 0b00000000,
    TEMP_EXTERNAL = 0b10000000,

    OFFSET_0 = 0b00000000,
    OFFSET_1 = 0b00000001,
    OFFSET_2 = 0b00000010,
    OFFSET_3 = 0b00000011,
    OFFSET_4 = 0b00000100,
    OFFSET_5 = 0b00000101,
    OFFSET_6 = 0b00000110,
    OFFSET_7 = 0b00000111,

    OFFSET_MIN_8 = 0b00001000,
    OFFSET_MIN_7 = 0b00001001,
    OFFSET_MIN_6 = 0b00001010,
    OFFSET_MIN_5 = 0b00001011,
    OFFSET_MIN_4 = 0b00001100,
    OFFSET_MIN_3 = 0b00001101,
    OFFSET_MIN_2 = 0b00001110,
    OFFSET_MIN_1 = 0b00001111
};

enum PLL_FLAGS {
    // other frequency options exist but there doesn't seem to be much
    // point in including them - this is a fair range of options...
    HZ_29 = 0b00111111,
    HZ_33 = 0b00111110,
    HZ_40 = 0b00111101,
    HZ_50 = 0b00111100,
    HZ_67 = 0b00111011,
    HZ_100 = 0b00111010,
    HZ_200 = 0b00111001
};

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

extern void dump(uint8_t* __xdata a, uint16_t __xdata l);  // remove me when done

static uint8_t __xdata epdCharSize = 1;   // character size, 1 or 2 (doubled)
static bool __xdata directionY = true;    // print direction, X or Y (true)
static uint8_t __xdata rbuffer[32];       // used to rotate bits around
static uint16_t __xdata fontCurXpos = 0;  // current X value we're working with
static uint16_t __xdata fontCurYpos = 0;  // current Y value we're working with
static uint8_t __xdata currentLut = 0;
static uint8_t __xdata dispLutSize = 0;
static bool __xdata drawDirection = false;
static bool __xdata isInited = false;

bool __xdata epdGPIOActive = false;

#define LUT_BUFFER_SIZE 128
uint8_t waveformbuffer[LUT_BUFFER_SIZE];
struct waveform10* __xdata waveform10 = (struct waveform10*)waveformbuffer;  // holds the LUT/waveform
struct waveform* __xdata waveform7 = (struct waveform*)waveformbuffer;       // holds the LUT/waveform

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
    P2LVLSEL &= ~(2);

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
        if (P2_1)
            return;
    }
#ifdef DEBUGEPD
    pr("screen timeout %lu ticks :(\n", timerGet() - start);
#endif
    while (1)
        ;
}

static void epdWaitUntilBusy(uint32_t timeout) {
    uint32_t __xdata start = timerGet();

    while (timerGet() - start < timeout) {
        if (!P2_1)
            return;
    }
#ifdef DEBUGEPD
    pr("Waited until the EPD would start doing anything, timeout %lu ticks :(\n", timerGet() - start);
#endif
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
    epdBusyWait(133300);
    timerDelay(TIMER_TICKS_PER_SECOND / 1000);
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
        pr("EPD: configuring GPIO\n");
        P2DIR |= (1 << 1);                // busy as input
        P2DIR &= ~((1 << 2) | (1 << 0));  // D/C and Reset as output
        P1DIR &= ~((1 << 7) | (1 << 2));  // select and bs1 as output
        P1_2 = 0;                         // select 4-wire SPI / BS1 = low
        P1_7 = 1;                         // deselect EPD
    } else {
        pr("EPD: Deconfiguring GPIO\n");
        P2DIR |= ((1 << 2) | (1 << 0));  // DC and Reset as input
        P2 &= ~((1 << 2) | (1 << 0));
        P1DIR |= ((1 << 7) | (1 << 2));  // Select and BS1 as input
        P2 &= ~((1 << 7));
    }
    epdGPIOActive = setup;
}
void epdEnterSleep() {
    pr("EPD Entering sleep\n");
    timerDelay(20 * TIMER_TICKS_PER_MS);
    epdReset();
    timerDelay(20 * TIMER_TICKS_PER_MS);
    shortCommand1(CMD_DEEP_SLEEP, 0xA5);
    isInited = false;
}

void epdWrite(uint8_t reg, uint8_t len, ...) {
    va_list valist;
    va_start(valist, len);
    markCommand();
    epdSelect();
    epdSend(reg);
    epdDeselect();
    markData();
    for (uint8_t i = 0; i < len; i++) {
        epdSelect();
        epdSend(va_arg(valist, int));
        epdDeselect();
    }
    va_end(valist);
}

static void epdDrawDirection(bool direction) {
    if (direction == drawDirection) return;

    drawDirection = direction;

    uint8_t psr_setting = FORMAT_BWR | BOOSTER_ON | RESET_NONE | LUT_OTP | SHIFT_RIGHT;
    if (drawDirection) {
        psr_setting |= SCAN_DOWN;
    } else {
        psr_setting |= SCAN_UP;
    }
    epdWrite(CMD_PANEL_SETTING, 1, psr_setting);
}

void epdSetup() {
    pr("EPD: Start setup\n");
    epdReset();

    epdWrite(0x06, 3, 0x07, 0x07, 0x17);
    epdWrite(0xF8, 2, 0x60, 0xA5);
    epdWrite(0xF8, 2, 0x89, 0xA5);
    epdWrite(0xF8, 2, 0x90, 0x00);
    epdWrite(0xF8, 2, 0x93, 0x2A);
    epdWrite(0xF8, 2, 0xA0, 0xA5);
    epdWrite(0xF8, 2, 0xA1, 0x00);
    epdWrite(0xF8, 2, 0x73, 0x41);
    // epdWrite(0x00, 1, 0x07); <<- stock init
    drawDirection = true;
    epdDrawDirection(false);
    epdWrite(0x16, 1, 0x00);
    epdWrite(0x50, 1, 0x87);
    // epdWrite(0x50, 1, 0x57); <<- this is the stock init

    shortCommand(CMD_POWER_ON);
    epdWaitUntilBusy(133300);
    epdWaitRdy();
    timerDelay(1333 * 5);
    pr("EPD: setup complete\n");
}
static uint8_t epdGetStatus() {
    uint8_t sta;
    commandReadBegin(0x2F);
    sta = epdReadByte();
    commandReadEnd();
    return sta;
}
uint16_t epdGetBattery(void) {
    uint8_t sta;
    commandReadBegin(0x51);
    sta = epdReadByte();
    commandReadEnd();
    if (sta) return 3000;
    return 2100;
}

void selectLUT(uint8_t lut) {
    // implement alternative LUTs here. Currently just reset the watchdog to two minutes,
    // to ensure it doesn't reset during the much longer bootup procedure
    lut += 1;                    // make the compiler a happy camper
    wdtSetResetVal(0xFF8E797F);  // 120 s
    wdtOn();
    return;
}

static uint16_t __xdata xWindowStart = 0;
static uint16_t __xdata xWidth = 0;
static uint16_t __xdata yWindowStart = 0;
static uint16_t __xdata yHeight = 0;

void setWindowXY(uint16_t xstart, uint16_t xend, uint16_t ystart, uint16_t yend) {
    xstart /= 8;
    xstart *= 8;

    xWindowStart = xstart;
    xWidth = xend - xstart;
    yWindowStart = ystart;
    yHeight = yend - ystart;

    xWidth /= 8;
    xWidth *= 8;

    return;
}

void startWindow(bool color) {
    if (color) {
        shortCommand(0x15);
        markData();
    } else {
        shortCommand(0x14);
        markData();
    }
    epdSelect();
    epdSend((xWindowStart >> 8) & 0x01);
    epdDeselect();

    epdSelect();
    epdSend(xWindowStart & 0xFF);
    epdDeselect();

    epdSelect();
    epdSend((yWindowStart >> 8) & 0x01);
    epdDeselect();

    epdSelect();
    epdSend(yWindowStart & 0xFF);
    epdDeselect();

    epdSelect();
    epdSend((xWidth >> 8) & 0x01);
    epdDeselect();

    epdSelect();
    epdSend(xWidth & 0xFF);
    epdDeselect();

    epdSelect();
    epdSend((yHeight >> 8) & 0x01);
    epdDeselect();

    epdSelect();
    epdSend(yHeight & 0xFF);
    epdDeselect();
}

void setColorMode(uint8_t red, uint8_t bw) {
    // this does exactly nothing, just keeps the compiler from barking
    red = 1;
    bw = 0;
    return;
}
void clearScreen() {
    // shortCommand(CMD_PARTIAL_OUT);
    commandBegin(CMD_DISPLAY_START_TRANSMISSION_DTM2);
    for (uint16_t c = 0; c < ((1UL * SCREEN_HEIGHT * SCREEN_WIDTH) / 2); c++) {
        epdSelect();
        epdSend(0x00);
        epdDeselect();
    }
    commandEnd();
    epdWaitRdy();
    commandBegin(CMD_DISPLAY_START_TRANSMISSION_DTM1);
    for (uint16_t c = 0; c < ((1UL * SCREEN_HEIGHT * SCREEN_WIDTH) / 2); c++) {
        epdSelect();
        epdSend(0x00);
        epdDeselect();
    }
}
void draw() {
    shortCommand(CMD_DISPLAY_REFRESH);
    epdWaitRdy();
    timerDelay(133300);
}
void drawNoWait() {
    shortCommand(CMD_DISPLAY_REFRESH);
}
void drawWithSleep() {
    shortCommand(CMD_DISPLAY_REFRESH);

    epdWaitUntilBusy(1333000);

    uint8_t tmp_P2FUNC = P2FUNC;
    uint8_t tmp_P2DIR = P2DIR;
    uint8_t tmp_P2PULL = P2PULL;
    uint8_t tmp_P2LVLSEL = P2LVLSEL;
    P2FUNC &= 0xfd;
    P2DIR |= 2;
    P2PULL |= 2;
    P2LVLSEL &= ~(2);

    P2CHSTA &= 0xfd;
    P2INTEN = 2;
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
    timerDelay(133300);
}
void epdWaitRdy() {
    epdBusyWait(TIMER_TICKS_PER_SECOND * 120);
    timerDelay(133300);
}
void beginFullscreenImage() {
    //drawDirection = true;
    // epdDrawDirection(false);
}
void beginWriteFramebuffer(bool color) {
    if (color == EPD_COLOR_RED) {
        epdWrite(CMD_DISPLAY_START_TRANSMISSION_DTM2, 0);
    } else {
        epdWrite(CMD_DISPLAY_START_TRANSMISSION_DTM1, 0);
    }
}
void endWriteFramebuffer() {
}

void loadRawBitmap(uint8_t* bmp, uint16_t x, uint16_t y, bool color) __reentrant {
    // this function is very badly hurt by the switch to UC8151, taking up LOTS of valuable idata space. Only defining variables
    // as static, or the function as reentrant (relegating variables to the stack) seemed to fix the idata issue. Fix me, or put me out of my misery...

    uint16_t xsize = bmp[0] / 8;
    if (bmp[0] % 8) xsize++;
    uint16_t ysize = bmp[1];
    uint16_t size = xsize * bmp[1];

    // shortCommand1(CMD_DATA_ENTRY_MODE, 3);

    bmp += 2;

    uint16_t c = 0;
    uint16_t curY = y;
    while (1) {
        if (c % xsize == 0) {
            if (drawDirection) {
                setWindowXY(x, x + xsize * 8, SCREEN_HEIGHT - curY - 1, SCREEN_HEIGHT - curY);
            } else {
                setWindowXY(x, x + xsize * 8, curY - 1, curY);
            }
            curY++;
            if (color) {
                startWindow(true);
            } else {
                startWindow(false);
            }
        }
        epdSelect();
        epdSend(*(bmp++));
        epdDeselect();
        c++;
        if (!size--) break;
    }
}
void printBarcode(const uint8_t* string, uint16_t x, uint16_t y) {
    setWindowXY(x, x + 8, SCREEN_HEIGHT - y, SCREEN_HEIGHT);
    startWindow(false);
    struct BarcodeInfo __xdata bci = {
        .str = string,
    };
    while (!barcodeIsDone(&bci)) {
        epdSelect();
        if (barcodeNextBar(&bci)) {
            epdSend(0xFF);
        } else {
            epdSend(0x00);
        }
        epdDeselect();
    }
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
            epdSelect();
            epdSend(rbuffer[i]);
            epdDeselect();
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
        epdSelect();
        epdSend(byte);
        epdDeselect();
    } else {
        uint8_t offset = rbuffer[1];
        rbuffer[0] |= (byte >> offset);
        epdSelect();
        epdSend(rbuffer[0]);
        epdDeselect();
        // epdSend(byte);
        rbuffer[0] = (byte << (8 - offset));
        rbuffer[2]++;
        if (rbuffer[2] == rbuffer[3]) {
            epdSelect();
            epdSend(rbuffer[0]);
            epdDeselect();
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
    c -= 0x20;
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

        // setWindowY(y, 1);
        if (epdCharSize == 2) {
            setWindowXY(x, x + 32 + extra, SCREEN_HEIGHT - y, SCREEN_HEIGHT);
            // setPosXY(x, y);
        } else {
            setWindowXY(x, x + 16 + extra, SCREEN_HEIGHT - y, SCREEN_HEIGHT);
            // setPosXY(x, y);
        }
        // shortCommand1(CMD_DATA_ENTRY_MODE, 1);  // was 3
    } else {
        if (epdCharSize == 2) {
            x /= 2;
            x *= 2;
            setWindowXY(x, SCREEN_WIDTH, y, y + 32);
        } else {
            setWindowXY(x, SCREEN_WIDTH, y, y + 16);
        }
        // setPosXY(x, y);
        fontCurXpos = x;
        // setWindowXY(x, SCREEN_WIDTH);
        //  shortCommand1(CMD_DATA_ENTRY_MODE, 7);
        memset(rbuffer, 0, 32);
    }

    if (color) {
        startWindow(true);
    } else {
        startWindow(false);
    }
    epdSelect();
}
void epdPrintEnd() {
    if (!directionY && ((fontCurXpos % 8) != 0)) {
        for (uint8_t i = 0; i < (16 * epdCharSize); i++) {
            epdSelect();
            epdSend(rbuffer[i]);
            epdDeselect();
        }
    }
    epdDrawDirection(true);
}