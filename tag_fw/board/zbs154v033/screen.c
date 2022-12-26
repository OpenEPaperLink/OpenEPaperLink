#include "screen.h"

#include <stdbool.h>

#include "adc.h"
#include "asmUtil.h"
#include "board.h"
#include "cpu.h"
#include "printf.h"
#include "sleep.h"
#include "spi.h"
#include "timer.h"

uint8_t __xdata mScreenRow[320];

static __bit mInited = false, mPartial;
static uint8_t __xdata mPassNo;

#define SCREEN_CMD_CLOCK_ON 0x80
#define SCREEN_CMD_CLOCK_OFF 0x01

#define SCREEN_CMD_ANALOG_ON 0x40
#define SCREEN_CMD_ANALOG_OFF 0x02

#define SCREEN_CMD_LATCH_TEMPERATURE_VAL 0x20

#define SCREEN_CMD_LOAD_LUT 0x10
#define SCREEN_CMD_USE_MODE_2 0x08  // modified commands 0x10 and 0x04

#define SCREEN_CMD_REFRESH 0xC7

static const uint8_t __code mColorMap[][6] =
    {
        // colors are: B, DG, G, LG, W, R
        // phase 0 (LUTS: B:W:R:G, purpose: BWR, prepare greys)
        {
            1, 1, 1, 1, 0, 0,  // lo plane (B)
        },
        {
            0, 0, 0, 0, 0, 1,  // hi plane (R)
        }};

static const uint8_t __code partial_lut[] = {
    // lut0 (KEEP) voltages
    0x40,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    // lut1 (W2B) voltages
    0x80,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    // lut2 (B2W) voltages
    0x40,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    // lut3 (unused) voltages
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    // lut4 (vcom) voltages
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,

    // group0 phase lengths and repeat count
    45,
    0x00,
    0x00,
    0x00,
    0x00,

    // group1 not used
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,

    // group2 not used
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,

    // group3 phase lengths and repeat count
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,

    // group4 phase lengths and repeat count
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,

    // group5 phase lengths and repeat count
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,

    // group6 phase lengths and repeat count
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
};

#define einkPrvSelect() \
    do {                \
        P1_7 = 0;       \
    } while (0)

#define einkPrvDeselect() \
    do {                  \
        P1_7 = 1;         \
    } while (0)
// urx pin
#define einkPrvMarkCommand() \
    do {                     \
        P2_2 = 0;            \
    } while (0)

#define einkPrvMarkData() \
    do {                  \
        P2_2 = 1;         \
    } while (0)

#pragma callee_saves einkPrvCmd
static void einkPrvCmd(uint8_t cmd)  // sets chip select
{
    einkPrvSelect();
    einkPrvMarkCommand();
    spiByte(cmd);
}

#pragma callee_saves einkPrvData
static void einkPrvData(uint8_t byte) {
    einkPrvMarkData();
    spiByte(byte);
}

#pragma callee_saves einkPrvCmdWithOneByte
static void einkPrvCmdWithOneByte(uint16_t vals)  // passing in one u16 is better than two params cause SDCC sucks
{
    einkPrvCmd(vals >> 8);
    einkPrvData(vals);
    einkPrvDeselect();
}

#pragma callee_saves einkPrvWaitWithTimeout
static void einkPrvWaitWithTimeout(uint32_t timeout) {
    uint32_t __xdata start = timerGet();

    while (timerGet() - start < timeout) {
        if (!P2_1)
            return;
    }
    pr("screen timeout %lu ticks\n", timerGet() - start);
    while (1)
        ;
}

#pragma callee_saves einkPrvWaitWithTimeout
static void einkPrvWaitWithTimeoutSleep(uint32_t timeout) {
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
    P2CHSTA &= 0xfd;
    P2INTEN &= 0xfd;

    P2FUNC = tmp_P2FUNC;
    P2DIR = tmp_P2DIR;
    P2PULL = tmp_P2PULL;
    P2LVLSEL = tmp_P2LVLSEL;
    /*if (!P2_1)
            return;

    pr("screen timeout\n");
    while(1);*/
}

#pragma callee_saves einkPrvReadByte
static uint8_t einkPrvReadByte(void) {
    uint8_t val = 0, i;

    P0DIR = (P0DIR & ~(1 << 0)) | (1 << 1);
    P0 &= ~(1 << 0);
    P0FUNC &= ~((1 << 0) | (1 << 1));

    P2_2 = 1;

    for (i = 0; i < 8; i++) {
        P0_0 = 1;
        __asm__("nop\nnop\nnop\nnop\nnop\n");
        val <<= 1;
        if (P0_1)
            val++;
        P0_0 = 0;
        __asm__("nop\nnop\nnop\nnop\nnop\n");
    }

    // set up pins for spi (0.0,0.1,0.2)
    P0FUNC |= (1 << 0) | (1 << 1);

    return val;
}

#pragma callee_saves einkPrvReadStatus
static uint8_t einkPrvReadStatus(void) {
    uint8_t sta;
    einkPrvCmd(0x2f);

    sta = einkPrvReadByte();
    einkPrvDeselect();

    return sta;
}

#pragma callee_saves screenPrvStartSubPhase
static void screenPrvStartSubPhase(__bit redSubphase) {
    einkPrvCmd(0x4e);
    einkPrvData(0);
    einkPrvDeselect();

    einkPrvCmd(0x4f);
    einkPrvData(0x00);
    einkPrvData(0x00);
    einkPrvDeselect();

    einkPrvCmd(redSubphase ? 0x26 : 0x24);

    einkPrvDeselect();
}

void moveToXY(uint8_t x, uint16_t y, bool color) {
    einkPrvCmd(0x4e);
    einkPrvData(x);
    einkPrvDeselect();
    einkPrvCmd(0x4f);
    einkPrvData((uint8_t)(y & 0xFF));
    einkPrvData((uint8_t)(y >> 8));
    einkPrvDeselect();
    einkPrvCmd(color ? 0x26 : 0x24);
    einkPrvDeselect();
}

#pragma callee_saves screenInitIfNeeded
static void screenInitIfNeeded(__bit forPartial) {
    if (mInited)
        return;

    mInited = true;
    mPartial = forPartial;

    timerDelay(TIMER_TICKS_PER_SECOND / 1000);
    P2_0 = 0;
    timerDelay(TIMER_TICKS_PER_SECOND / 1000);
    P2_0 = 1;
    timerDelay(TIMER_TICKS_PER_SECOND / 1000);

    einkPrvCmd(0x12);  // software reset
    einkPrvDeselect();
    timerDelay(TIMER_TICKS_PER_SECOND / 1000);

    einkPrvCmdWithOneByte(0x7454);

    einkPrvCmdWithOneByte(0x7e3b);

    einkPrvCmd(0x2b);
    einkPrvData(0x04);
    einkPrvData(0x63);
    einkPrvDeselect();

    einkPrvCmd(0x0c);  // they send 8f 8f 8f 3f
    einkPrvData(0x8f);
    einkPrvData(0x8f);
    einkPrvData(0x8f);
    einkPrvData(0x3f);
    einkPrvDeselect();

    einkPrvCmd(0x01);
    einkPrvData(0x97);
    einkPrvData(0x00);
    einkPrvData(0x00);
    einkPrvDeselect();

    einkPrvCmdWithOneByte(0x1103);
    einkPrvDeselect();

    einkPrvCmdWithOneByte(0x2200 | SCREEN_CMD_CLOCK_ON | SCREEN_CMD_ANALOG_ON);
    einkPrvCmd(0x20);
    einkPrvDeselect();

    einkPrvWaitWithTimeout(TIMER_TICKS_PER_SECOND * 10);

    einkPrvCmd(0x44);
    einkPrvData(0x00);
    einkPrvData(SCREEN_WIDTH / 8 - 1);
    einkPrvDeselect();

    einkPrvCmd(0x45);
    einkPrvData(0x00);
    einkPrvData(0x00);
    einkPrvData(0x97);
    einkPrvData(0x00);
    einkPrvDeselect();

    einkPrvCmdWithOneByte(0x3c01);  // border will be HiZ
    einkPrvDeselect();
    einkPrvCmdWithOneByte(0x1880);  // internal temp sensor

    einkPrvCmdWithOneByte(0x2108);

    // turn on clock & analog
    einkPrvCmdWithOneByte(0x22B1);
    einkPrvCmd(0x20);  // do action
    einkPrvDeselect();
    einkPrvWaitWithTimeout(TIMER_TICKS_PER_SECOND);

    if (forPartial) {
        einkPrvCmd(0x32);
        for (int i = 0; i < sizeof(partial_lut); i++)
            einkPrvData(partial_lut[i]);
    }
    einkPrvDeselect();
}

#pragma callee_saves screenPrvDraw
static void screenPrvDraw(void) {
    einkPrvCmdWithOneByte(0x2200 | SCREEN_CMD_REFRESH);
    einkPrvCmd(0x20);  // do actions
    if (0) {
        einkPrvWaitWithTimeoutSleep(1000 * 60UL);
        screenSleep();
    } else {
        einkPrvWaitWithTimeout(TIMER_TICKS_PER_SECOND * 60UL);
    }
}

__bit screenTxStart(__bit forPartial) {
    screenInitIfNeeded(forPartial);
    mPassNo = 0;

    screenPrvStartSubPhase(false);

    return true;
}

void screenEndPass(void) {
    switch (mPassNo) {
        case 0:
            screenPrvStartSubPhase(true);
            break;
        default:
            return;
    }
    mPassNo++;
}

void screenTxEnd(void) {
    screenPrvDraw();
    screenShutdown();
}

void screenShutdown(void) {
    if (!mInited)
        return;

    mInited = false;
    einkPrvCmdWithOneByte(0x1003);  // shut down
}

void screenSleep(void) {
    P2_0 = 0;
    timerDelay(10);
    P2_0 = 1;
    timerDelay(50);

    einkPrvCmd(0x12);  // software reset
    einkPrvDeselect();
    einkPrvWaitWithTimeout(TIMER_TICKS_PER_SECOND / 250);
    einkPrvCmdWithOneByte(0x1003);  // shut down
}

#pragma callee_saves screenByteTx
void screenByteTx(uint8_t byte) {
    static uint8_t __xdata prev, step = 0;

    prev <<= 2;
    prev |= (mColorMap[mPassNo][byte >> 4] << 1) | mColorMap[mPassNo][byte & 0x0f];
    if (++step == 4) {
        step = 0;
        einkPrvSelect();
        einkPrvData(prev);
        einkPrvDeselect();
    }
}

void screenByteRawTx(uint8_t byte) {
    einkPrvSelect();
    einkPrvData(byte);
    einkPrvDeselect();
}

// yes this is here...
uint16_t adcSampleBattery(void) {
    __bit wasInited = mInited;
    uint16_t voltage = 2600;

    if (!mInited)
        screenInitIfNeeded(false);

    uint8_t val;

    einkPrvCmdWithOneByte(0x2200 | SCREEN_CMD_CLOCK_ON | SCREEN_CMD_ANALOG_ON);
    einkPrvCmd(0x20);  // do action
    einkPrvDeselect();
    einkPrvWaitWithTimeout(TIMER_TICKS_PER_SECOND);

    for (val = 3; val < 8; val++) {
        einkPrvCmdWithOneByte(0x1500 + val);
        einkPrvWaitWithTimeout(TIMER_TICKS_PER_SECOND);
        if (einkPrvReadStatus() & 0x10) {  // set if voltage is less than threshold ( == 1.9 + val / 10)
            voltage = 1850 + mathPrvMul8x8(val, 100);
            break;
        }
    }

    einkPrvCmdWithOneByte(0x22B1);
    einkPrvCmd(0x20);  // do action
    einkPrvDeselect();
    einkPrvWaitWithTimeout(TIMER_TICKS_PER_SECOND);

    if (!wasInited)
        screenShutdown();

    return voltage;
}
