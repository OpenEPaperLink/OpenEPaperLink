#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "printf.h"
#include "sem9010_uk.h"
#include "spi.h"
#include "timer.h"
#include "zbs243.h"

__bit display_is_drawing = 0;
uint32_t __xdata screen_start_time = 0;
__bit isInverted = false;

uint8_t __xdata curEpdSegmentData[12] = {0};
uint8_t __xdata epdSegmentData[12] = {0};

#pragma callee_saves screenPrvTimedWait
static __bit screenPrvTimedWait(uint32_t maxTicks) {
    uint32_t startTicks = timerGet();

    while (timerGet() - startTicks < maxTicks) {
        if (!P2_0)
            return true;
    }

    return false;
}
#pragma callee_saves screenPrvRegWriteGuts
static __bit screenPrvRegWriteGuts(uint32_t val, uint8_t reg) {  // order because sdcc sucks
    if (!screenPrvTimedWait(TIMER_TICKS_PER_SECOND / 1000))
        return false;

    P2_1 = 0;
    spiByte((uint8_t)0x80 + (uint8_t)(reg << 1));
    spiByte(val >> 16);
    spiByte(val >> 8);
    spiByte(val);
    P2_1 = 1;

    return true;
}

void epdWaitRdy() {
    if (!screenPrvTimedWait(TIMER_TICKS_PER_SECOND / 1000))
        return;
}
#define screenPrvRegWrite(_reg, _val) screenPrvRegWriteGuts(_val, _reg)
static uint8_t charDecode(uint8_t c) {
    const uint8_t __code numbers[] = {0b11110110, 0b11000000, 0b01101110, 0b11101010, 0b11011000, 0b10111010, 0b10111110, 0b11100000, 0b11111110, 0b11111010};

    static const uint8_t __code lowercase[] = {
        0b11111100,
        0b10011110,
        0b00001110,
        0b11001110,
        0b00111110,
        0b00111100,
        0b11111010,
        0b10011100,
        0b10000000,
        0b11000010,
        0b00011100,
        0b00000110,
        0b10101100,
        0b10001100,
        0b10001110,
        0b01111100,
        0b11111000,
        0b00001100,
        0b10111010,
        0b00011110,
        0b10000110,
        0b10010110,
        0b01011010,
        0b10001000,
        0b11011010,
        0b01101110,
    };

    static const uint8_t __code uppercase[] = {
        0b11111100,
        0b10011110,
        0b00110110,
        0b11001110,
        0b00111110,
        0b00111100,
        0b10110110,
        0b11011100,
        0b11000000,
        0b11000010,
        0b00011100,
        0b00010110,
        0b10101100,
        0b11110100,
        0b11110110,
        0b01111100,
        0b11111000,
        0b00001100,
        0b10111010,
        0b00011110,
        0b11010110,
        0b10010110,
        0b01011010,
        0b10001000,
        0b11011010,
        0b01101110,
    };

    if (c > 0x2F && c < 0x3A) {
        return numbers[c - 0x30];
    }

    if (c > 0x60 && c < 0x7b) {
        return lowercase[c - 0x61];
    }

    if (c > 0x40 && c < 0x5B) {
        return uppercase[c - 0x41];
    }
    //  / \\__5__//
    // |   |     |   |
    // | 4 |     | 6 |
    // |   |     |   |
    //  \ /<  3  >\ /
    //  / \       /
    // |   |     |   |
    // | 2 |     | 7 |
    // |   |     |   |
    //  \ //__1__\\ /
    switch (c) {
        case 0x20:  // space
            return 0x00;
        case 0x2D:  // -
            return 0b00001000;
        case 0x28:  // (
        case 0x5B:  // [
            return 0b00110110;
        case 0x29:  // )
        case 0x5D:  // ]
            return 0b11100010;
        case 0x3C:  // <
            return 0b00001110;
        case 0x3E:  // >
            return 0b10001010;
        case 0x5F:  // _
            return 0b00000010;
        case 0x5E:  // ^
            return 0b01111000;
        case 0x3D:  // =
            return 0b00001010;
        case 0x23:  // #
            return 0b10000100;
        default:
            return 0x00;
    }
}
static uint8_t segmentBitSwap(uint8_t val) {
    uint8_t __xdata ret = 0;
    if (val & 0x02) ret |= (1 << 5);
    if (val & 0x04) ret |= (1 << 6);
    if (val & 0x08) ret |= (1 << 3);
    if (val & 0x10) ret |= (1 << 7);
    if (val & 0x20) ret |= (1 << 1);
    if (val & 0x40) ret |= (1 << 2);
    if (val & 0x80) ret |= (1 << 4);
    return ret;
}
static void writeDigitToBuffer(uint8_t pos, uint8_t c) {
    switch (pos) {
        case 0:
        case 1:
        case 4:
        case 5:
        case 6:
            epdSegmentData[pos] &= 0x01;
            epdSegmentData[pos] |= charDecode(c);
            break;
        case 2:
            epdSegmentData[3] &= 0x01;
            epdSegmentData[3] |= segmentBitSwap(charDecode(c));
            break;
        case 3:
            epdSegmentData[2] &= 0x01;
            epdSegmentData[2] |= segmentBitSwap(charDecode(c));
            break;
        case 7:
            epdSegmentData[9] &= 0x01;
            epdSegmentData[9] |= segmentBitSwap(charDecode(c));
            break;
        case 8:
            epdSegmentData[8] &= 0x01;
            epdSegmentData[8] |= segmentBitSwap(charDecode(c));
            break;
        case 9:
            epdSegmentData[7] &= 0x01;
            epdSegmentData[7] |= segmentBitSwap(charDecode(c));
            break;
    }
}
static void iconSet(uint8_t byte, uint8_t bit, bool on) {
    if (on) {
        epdSegmentData[byte] |= (1 << bit);
    } else {
        epdSegmentData[byte] &= ~(1 << bit);
    }
}

void setEPDIcon(uint16_t iconvalue, bool on) {
    if (iconvalue & EPD_SIGN_POUND_LARGE)
        iconSet(2, 0, on);
    if (iconvalue & EPD_DIGIT_ONE_LARGE)
        iconSet(1, 0, on);
    if (iconvalue & EPD_PERIOD_LARGE)
        iconSet(3, 0, on);
    if (iconvalue & EPD_SIGN_PENCE_LARGE)
        iconSet(4, 0, on);
    if (iconvalue & EPD_BG_COLOR)
        iconSet(0, 0, on);
    if (iconvalue & EPD_SIGN_POUND_SMALL)
        iconSet(6, 0, on);
    if (iconvalue & EPD_DIGIT_ONE_SMALL)
        iconSet(7, 0, on);
    if (iconvalue & EPD_PERIOD_SMALL)
        iconSet(9, 0, on);
    if (iconvalue & EPD_SIGN_PENCE_SMALL)
        iconSet(8, 0, on);
    if (iconvalue & EPD_ICON_DIAMOND)
        iconSet(10, 0, on);
    if (iconvalue & EPD_ICON_ARROW)
        iconSet(10, 1, on);
    if (iconvalue & EPD_ICON_STAR)
        iconSet(10, 2, on);
    if (iconvalue & EPD_ICON_CIRCLE)
        iconSet(10, 3, on);
    if (iconvalue & EPD_ICON_SQUARE)
        iconSet(10, 4, on);
}
void epdEnable() {
    // P2_0 = BUSY
    // P2_1 = _CS
    // P2_2 = power
    // P1_7 = _RESET
    // P1_6 = EXT_CLK
    // P0_0 = CLK
    // P0_1 = MoSi
    // P0_2 = MiSo
    P2FUNC &= ~((1 << 0) | (1 << 1) | (1 << 2));  // disable functions on busy, _cs, power
    P2DIR &= ~((1 << 1) | (1 << 2));              // _CS 2.1 and POWER 2.2(output)
    P2DIR |= (1 << 0);                            // BUSY (input)
    //
    P1DIR &= ~((1 << 7) | (1 << 6));  // _RESET and EPD_EXT_CLK as output
    P1FUNC &= ~(1 << 7);              // disable function on _reset
    P1FUNC |= (1 << 6);               // EPD_EXT_CLK-out enable
    //
    P0FUNC |= (1 << 0) | (1 << 1) | (1 << 2);  // enable clk, mosi, miso functions
    P0DIR &= ~((1 << 0) | (1 << 1));           // enable output on clk, mosi
    P0DIR |= (1 << 2);                         // miso as input
    P0PULL = (1 << 2);                         // pullup on miso

    spiInit();
}
void epdDisable() {
    P1_7 = 0;  // assert reset
    // timerDelay(TIMER_TICKS_PER_SECOND / 100);
    P2_2 = 0;  // power it down

    // shutdown
    P2DIR |= (1 << 1) | (1 << 2);                 // _cs and power as input
    P1DIR |= (1 << 7) | (1 << 6);                 // reset and epd-ext-clk as input
    P0DIR |= (1 << 0) | (1 << 1);                 // mosi/clk as input
    P0PULL &= ~(1 << 2);                          // disable miso pullup
    P1FUNC &= ~(1 << 6);                          // disable clock output
    P0FUNC &= ~((1 << 0) | (1 << 1) | (1 << 2));  // disable SPI functions
}

__bit epdDraw() {
    // data (doesnt agree with spec but makes sense)
    if (!screenPrvRegWrite(0x0d, *(uint32_t __xdata *)(epdSegmentData + 0)))
        return false;
    if (!screenPrvRegWrite(0x0e, *(uint32_t __xdata *)(epdSegmentData + 3)))
        return false;
    if (!screenPrvRegWrite(0x0f, *(uint32_t __xdata *)(epdSegmentData + 6)))
        return false;
    if (!screenPrvRegWrite(0x10, *(uint32_t __xdata *)(epdSegmentData + 9)))
        return false;
    // update!
    if (!screenPrvRegWrite(0x00, isInverted ? 0xa0001c : 0x80001c))
        return false;

    timerDelay(TIMER_TICKS_PER_SECOND / 10000);
    memcpy(curEpdSegmentData, epdSegmentData, sizeof(epdSegmentData));
    screen_start_time = timerGet();
    display_is_drawing = 1;
    return true;
}

__bit epdUpdate() {
    if (memcmp(curEpdSegmentData, epdSegmentData, sizeof(epdSegmentData)) == 0) {
        return false;
    } else {
        return epdDraw();
    }
}

__bit epdSetup(__bit inverted) {
    // P2DIR = 0b11111001;
    isInverted = inverted;
    display_is_drawing = 0;
    P2_1 = 1;
    P2_2 = 1;  // power it up
    timerDelay(TIMER_TICKS_PER_SECOND / 1000);
    P1_7 = 0;  // assert reset
    timerDelay(TIMER_TICKS_PER_SECOND / 100);
    P1_7 = 1;  // release reset
    timerDelay(TIMER_TICKS_PER_SECOND / 1000);

    // wait for it
    if (!screenPrvTimedWait(TIMER_TICKS_PER_SECOND)) {
        return false;
    }

    // internal oscillator init?
    // if (!screenPrvRegWrite(0x03, 0x3a0000))
    //    return false;
    // iface init?
    if (!screenPrvRegWrite(0x01, 0x070000))
        return false;
    if (!screenPrvRegWrite(0x03, 0x400000))
        return false;
    if (!screenPrvRegWrite(0x04, 0xfc0000))
        return false;
    // LUTs
    if (!screenPrvRegWrite(0x14, 0x440000))
        return false;
    if (!screenPrvRegWrite(0x15, inverted ? 0x680001 : 0x860000))
        return false;
    if (!screenPrvRegWrite(0x16, inverted ? 0x240000 : 0x040000))
        return false;

    if (!screenPrvRegWrite(0x19, 0x082514))
        return false;
    if (!screenPrvRegWrite(0x1a, 0xa02000))
        return false;
    memset(curEpdSegmentData, 0, sizeof(epdSegmentData));
    return true;
}
void epdClear() {
    memset(epdSegmentData, 0, sizeof(epdSegmentData));
}

static uint8_t __xdata position = 0;

void epdSetPos(uint8_t p) {
    if (p >= sizeof(epdSegmentData)) {
        p = 0;
    }
    position = p;
}

void writeCharEPD(char c) {
    writeDigitToBuffer(position, c);
    position++;
    if (position >= sizeof(epdSegmentData)) {
        position = 0;
    }
}

void epdPrint(uint8_t pos, const char __code *fmt, ...) __reentrant {
    static uint8_t __xdata buffer[10];
    va_list vl;
    va_start(vl, fmt);
    spr(buffer, fmt, vl);
    char *p = buffer;
    while (*p && pos < sizeof(epdSegmentData)) {
        writeDigitToBuffer(pos++, *p++);
    }
    va_end(vl);
}
uint8_t is_drawing() {
    if (display_is_drawing) {
        if (!P2_0 || (timerGet() - screen_start_time > (TIMER_TICKS_PER_SECOND * 3))) {
            display_is_drawing = 0;
            return 0;
        }
        return 1;
    }
    return 0;
}
