#include "ssd1619.h"

#include <Arduino.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include "main.h"
#include "epd_spi.h"
#include "board.h"
#include "font.h"
#include "lut.h"
#include "screen.h"
#include "settings.h"
#include "sleep.h"
#include "timer.h"
#include "wdt.h"

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

#define SCREEN_CMD_CLOCK_ON 0x80
#define SCREEN_CMD_CLOCK_OFF 0x01
#define SCREEN_CMD_ANALOG_ON 0x40
#define SCREEN_CMD_ANALOG_OFF 0x02
#define SCREEN_CMD_LATCH_TEMPERATURE_VAL 0x20
#define SCREEN_CMD_LOAD_LUT 0x10
#define SCREEN_CMD_USE_MODE_2 0x08 // modified commands 0x10 and 0x04
#define SCREEN_CMD_REFRESH 0xC7

#define commandEnd()                \
    do                              \
    {                               \
        digitalWrite(EPD_CS, HIGH); \
    } while (0)

#define markCommand()              \
    do                             \
    {                              \
        digitalWrite(EPD_DC, LOW); \
    } while (0)

#define markData()                  \
    do                              \
    {                               \
        digitalWrite(EPD_DC, HIGH); \
    } while (0)

extern void dump(uint8_t *a, uint16_t l); // remove me when done

static uint8_t epdCharSize = 1;  // character size, 1 or 2 (doubled)
static bool directionY = true;   // print direction, X or Y (true)
static uint8_t rbuffer[32];      // used to rotate bits around
static uint16_t fontCurXpos = 0; // current X value we're working with
static uint8_t currentLut = 0;
uint8_t dispLutSize = 0; // we'll need to expose this in the 'capabilities' flag

static bool isInited = false;

bool epdGPIOActive = false;

#define LUT_BUFFER_SIZE 128
static uint8_t waveformbuffer[LUT_BUFFER_SIZE];
uint8_t customLUT[LUT_BUFFER_SIZE] = {0};

struct waveform10 *waveform10 = (struct waveform10 *)waveformbuffer; // holds the LUT/waveform
struct waveform *waveform7 = (struct waveform *)waveformbuffer;      // holds the LUT/waveform

void epd_busy()
{
}

static void epdBusyWait(uint32_t timeout)
{
    uint32_t start = millis();
    wdt120s();
    attachInterrupt(digitalPinToInterrupt(EPD_BUSY), epd_busy, FALLING);
    NRF_GPIO->PIN_CNF[EPD_BUSY] &= ~((uint32_t)GPIO_PIN_CNF_SENSE_Msk);
    NRF_GPIO->PIN_CNF[EPD_BUSY] |= ((uint32_t)GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos);
    while (millis() - start < timeout)
    {
        initRTC0(100);
        __WFE();
        __SEV();
        __WFE();
        if (!digitalRead(EPD_BUSY))
        {
            wdt30s();
            return;
        }
    }
    printf("screen timeout %lu ms :(\n", millis() - start);
    while (1)
    {
        if (!digitalRead(EPD_BUSY))
        {
            wdt30s();
            break;
        }
    }
}
static void epdBusySleep(uint32_t timeout)
{
    epdBusyWait(timeout);
}
static void commandReadBegin(uint8_t cmd)
{
    epdSelect();
    markCommand();
    spi_write(cmd); // dump LUT
    pinMode(EPD_MOSI, INPUT);
    markData();
}
static void commandReadEnd()
{
    // set up pins for spi (0.0,0.1,0.2)
    pinMode(EPD_MOSI, OUTPUT);
    epdDeselect();
}
static uint8_t epdReadByte()
{
    uint8_t val = 0, i;

    for (i = 0; i < 8; i++)
    {
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
static void shortCommand(uint8_t cmd)
{
    epdSelect();
    markCommand();
    spi_write(cmd);
    epdDeselect();
}
static void shortCommand1(uint8_t cmd, uint8_t arg)
{
    epdSelect();
    markCommand();
    spi_write(cmd);
    markData();
    spi_write(arg);
    epdDeselect();
}
static void shortCommand2(uint8_t cmd, uint8_t arg1, uint8_t arg2)
{
    epdSelect();
    markCommand();
    spi_write(cmd);
    markData();
    spi_write(arg1);
    spi_write(arg2);
    epdDeselect();
}
static void commandBegin(uint8_t cmd)
{
    epdSelect();
    markCommand();
    spi_write(cmd);
    markData();
}
static void epdReset()
{
    delay(12);
    digitalWrite(EPD_RST, LOW);
    delay(20);
    digitalWrite(EPD_RST, HIGH);
    delay(20);

    shortCommand(CMD_SOFT_RESET); // software reset
    delay(10);
    shortCommand(CMD_SOFT_RESET2);
    delay(10);
}
void epdConfigGPIO(bool setup)
{
    if (epdGPIOActive == setup)
        return;
    if (setup)
    {
        pinMode(EPD_RST, OUTPUT);
        pinMode(EPD_BS, OUTPUT);
        pinMode(EPD_CS, OUTPUT);
        pinMode(EPD_DC, OUTPUT);
        pinMode(EPD_BUSY, INPUT);
        pinMode(EPD_CLK, OUTPUT);
        pinMode(EPD_MOSI, OUTPUT);
    }
    else
    {
    }
    epdGPIOActive = setup;
}
void epdEnterSleep()
{
    digitalWrite(EPD_RST, LOW);
    delay(10);
    digitalWrite(EPD_RST, HIGH);
    delay(50);
    shortCommand(CMD_SOFT_RESET2);
    epdBusyWait(15);
    shortCommand1(CMD_ENTER_SLEEP, 0x03);
    isInited = false;
}
void epdSetup()
{
    epdReset();
    shortCommand1(CMD_ANALOG_BLK_CTRL, 0x54);
    shortCommand1(CMD_DIGITAL_BLK_CTRL, 0x3B);
    shortCommand2(CMD_UNKNOWN_1, 0x04, 0x63);

    /*commandBegin(CMD_SOFT_START_CTRL);
    epdSend(0x8f);
    epdSend(0x8f);
    epdSend(0x8f);
    epdSend(0x3f);
    commandEnd();*/

    commandBegin(CMD_DRV_OUTPUT_CTRL);
    epdSend((SCREEN_HEIGHT - 1) & 0xff);
    epdSend((SCREEN_HEIGHT - 1) >> 8);
    epdSend(0x00);
    commandEnd();

    // shortCommand1(CMD_DATA_ENTRY_MODE, 0x03);
    // shortCommand1(CMD_BORDER_WAVEFORM_CTRL, 0xC0); // blurry edges
    shortCommand1(CMD_BORDER_WAVEFORM_CTRL, 0x01);
    shortCommand1(CMD_TEMP_SENSOR_CONTROL, 0x80);
    shortCommand1(CMD_DISP_UPDATE_CTRL2, 0xB1); // mode 1 (i2C)
    // shortCommand1(CMD_DISP_UPDATE_CTRL2, 0xB9);  // mode 2?
    shortCommand(CMD_ACTIVATION);
    epdBusyWait(1000);
    isInited = true;
    currentLut = EPD_LUT_DEFAULT;
}
static uint8_t epdGetStatus()
{
    uint8_t sta;
    commandReadBegin(0x2F);
    sta = epdReadByte();
    commandReadEnd();
    return sta;
}
uint16_t epdGetBattery(void)
{
    uint16_t voltage = 2600;
    uint8_t val;

    delay(50);

    digitalWrite(EPD_RST, LOW);
    delay(50);
    digitalWrite(EPD_RST, HIGH);
    delay(50);

    shortCommand(CMD_SOFT_RESET); // software reset
    epdBusyWait(30);
    shortCommand(CMD_SOFT_RESET2);
    epdBusyWait(30);

    shortCommand1(CMD_DISP_UPDATE_CTRL2, SCREEN_CMD_CLOCK_ON | SCREEN_CMD_ANALOG_ON);
    shortCommand(CMD_ACTIVATION);
    epdBusyWait(100);

    for (val = 3; val < 8; val++)
    {
        shortCommand1(CMD_SETUP_VOLT_DETECT, val);
        epdBusyWait(100);
        if (epdGetStatus() & 0x10)
        { // set if voltage is less than threshold ( == 1.9 + val / 10)
            voltage = 1850 + (val * 100);
            break;
        }
    }

    shortCommand(CMD_SOFT_RESET2);
    epdBusyWait(15);
    shortCommand1(CMD_ENTER_SLEEP, 0x03);

    return voltage;
}

void loadFixedTempOTPLUT()
{
    shortCommand1(0x18, 0x48);                  // external temp sensor
    shortCommand2(0x1A, 0x05, 0x00);            // < temp register
    shortCommand1(CMD_DISP_UPDATE_CTRL2, 0xB1); // mode 1 (i2C)
    shortCommand(CMD_ACTIVATION);
    epdBusyWait(1000);
}
static void writeLut()
{
    commandBegin(CMD_WRITE_LUT);
    for (uint8_t i = 0; i < (dispLutSize * 10); i++)
        epdSend(waveformbuffer[i]);
    commandEnd();
}
static void readLut()
{
    commandReadBegin(0x33);
    uint16_t checksum = 0;
    uint16_t ident = 0;
    uint16_t shortl = 0;
    for (uint16_t c = 0; c < LUT_BUFFER_SIZE; c++)
    {
        waveformbuffer[c] = epdReadByte();
    }
    commandReadEnd();
}
static uint8_t getLutSize()
{
    uint8_t ref = 0;
    for (uint8_t c = (LUT_BUFFER_SIZE - 4); c > 16; c--)
    {
        uint8_t check = waveformbuffer[c];
        for (uint8_t d = 1; d < 4; d++)
        {
            if (waveformbuffer[c + d] != check)
            {
                ref = c;
                goto end;
            }
        }
    }
end:;
    return ref + 1;
}
static void lutGroupDisable(uint8_t group)
{
    if (dispLutSize == 7)
    {
        memset(&(waveform7->group[group]), 0x00, 5);
    }
    else
    {
        memset(&(waveform10->group[group]), 0x00, 5);
    }
}
static void lutGroupSpeedup(uint8_t group, uint8_t speed)
{
    if (dispLutSize == 7)
    {
        for (uint8_t i = 0; i < 4; i++)
        {
            waveform7->group[group].phaselength[i] = 1 + (waveform7->group[group].phaselength[i] / speed);
        }
    }
    else
    {
        for (uint8_t i = 0; i < 4; i++)
        {
            waveform10->group[group].phaselength[i] = 1 + (waveform10->group[group].phaselength[i] / speed);
        }
    }
}
static void lutGroupRepeat(uint8_t group, uint8_t repeat)
{
    if (dispLutSize == 7)
    {
        waveform7->group[group].repeat = repeat;
    }
    else
    {
        waveform10->group[group].repeat = repeat;
    }
}
static void lutGroupRepeatReduce(uint8_t group, uint8_t factor)
{
    if (dispLutSize == 7)
    {
        waveform7->group[group].repeat = waveform7->group[group].repeat / factor;
    }
    else
    {
        waveform10->group[group].repeat = waveform10->group[group].repeat / factor;
    }
}
void selectLUT(uint8_t lut)
{
    /*if (currentLut == lut)
    {
        return;
    }

    // Handling if we received an OTA LUT
    if (lut == EPD_LUT_OTA)
    {
        memcpy(waveformbuffer, customLUT, dispLutSize * 10);
        writeLut();
        currentLut = lut;
        return;
    }

    if (currentLut != EPD_LUT_DEFAULT)
    {
        // load the 'default' LUT for the current temperature in the EPD lut register
        shortCommand1(CMD_DISP_UPDATE_CTRL2, 0xB1); // mode 1?
        shortCommand(CMD_ACTIVATION);
        epdBusyWait(1000);
    }

    currentLut = lut;

    // if we're going to be using the default LUT, we're done here.
    if (lut == EPD_LUT_DEFAULT)
    {
        return;
    }

    // download the current LUT from the waveform buffer
    readLut();

    if (dispLutSize == 0)
    {
        dispLutSize = getLutSize();
        dispLutSize /= 10;
        printf("lut size = %d\n", dispLutSize);
#ifdef PRINT_LUT
        dump(waveformbuffer, LUT_BUFFER_SIZE);
#endif
        memcpy(customLUT, waveformbuffer, dispLutSize * 10);
    }

    switch (lut)
    {
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

    if (dispLutSize == 10)
    {
        lutGroupDisable(LUTGROUP_UNUSED);
        lutGroupDisable(LUTGROUP_UNKNOWN);
        lutGroupDisable(LUTGROUP_UNUSED3);
        lutGroupDisable(LUTGROUP_UNUSED4);
    }
    writeLut();*/
}

void setWindowX(uint16_t start, uint16_t end)
{
    shortCommand2(CMD_WINDOW_X_SIZE, start / 8, end / 8 - 1);
}
void setWindowY(uint16_t start, uint16_t end)
{
    commandBegin(CMD_WINDOW_Y_SIZE);
    epdSend((start)&0xff);
    epdSend((start) >> 8);
    epdSend((end - 1) & 0xff);
    epdSend((end - 1) >> 8);
    commandEnd();
}
void setPosXY(uint16_t x, uint16_t y)
{
    shortCommand1(CMD_XSTART_POS, (uint8_t)(x / 8));
    commandBegin(CMD_YSTART_POS);
    epdSend((y)&0xff);
    epdSend((y) >> 8);
    commandEnd();
}
void setColorMode(uint8_t red, uint8_t bw)
{
    shortCommand1(CMD_DISP_UPDATE_CTRL, (red << 4) | bw);
}
void fillWindowWithPattern(bool color)
{
    if (color == EPD_COLOR_RED)
    {
        shortCommand1(CMD_WRITE_PATTERN_RED, 0x00);
    }
    else
    {
        shortCommand1(CMD_WRITE_PATTERN_BW, 0x00);
    }
}
void clearWindow(bool color)
{
    if (color == EPD_COLOR_RED)
    {
        shortCommand1(CMD_WRITE_PATTERN_RED, 0x66);
    }
    else
    {
        shortCommand1(CMD_WRITE_PATTERN_BW, 0x66);
    }
}
void clearScreen()
{
    setWindowX(0, SCREEN_WIDTH);
    setWindowY(0, SCREEN_HEIGHT);
    setPosXY(0, 0);
    shortCommand1(CMD_DATA_ENTRY_MODE, 3); // was 3
    shortCommand1(CMD_WRITE_PATTERN_BW, 0x66);
    epdBusyWait(100);
    shortCommand1(CMD_WRITE_PATTERN_RED, 0x66);
    epdBusyWait(100);
}
void draw()
{
    shortCommand1(0x22, 0xF7);
    // shortCommand1(0x22, SCREEN_CMD_REFRESH);
    shortCommand(0x20);
    epdBusyWait(120000);
}
void drawNoWait()
{
    shortCommand1(0x22, 0xF7);
    // shortCommand1(0x22, SCREEN_CMD_REFRESH);
    shortCommand(0x20);
}
void drawWithSleep()
{
    shortCommand1(0x22, 0xF7);
    // shortCommand1(0x22, SCREEN_CMD_REFRESH);
    shortCommand(0x20);
    /*uint8_t tmp_P2FUNC = P2FUNC;
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
    P2LVLSEL = tmp_P2LVLSEL;*/
    epdBusyWait(100000);
    eepromPrvDeselect();
}
void epdWaitRdy()
{
    epdBusyWait(120000);
}
void drawLineHorizontal(bool color, uint16_t x1, uint16_t x2, uint16_t y)
{
    setWindowX(x1, x2);
    setWindowY(y, y + 1);
    if (color)
    {
        shortCommand1(CMD_WRITE_PATTERN_RED, 0xE6);
    }
    else
    {
        shortCommand1(CMD_WRITE_PATTERN_BW, 0xE6);
    }
    epdBusyWait(100);
}
void drawLineVertical(bool color, uint16_t x, uint16_t y1, uint16_t y2)
{
    setWindowY(y1, y2);
    setWindowX(x, x + 8);
    shortCommand1(CMD_DATA_ENTRY_MODE, 3);
    setPosXY(x, y1);
    if (color)
    {
        commandBegin(CMD_WRITE_FB_RED);
    }
    else
    {
        commandBegin(CMD_WRITE_FB_BW);
    }
    uint8_t c = 0x80;
    c >>= (x % 8);
    for (; y1 < y2; y1++)
    {
        epdSend(c);
    }
    commandEnd();
}
void beginFullscreenImage()
{
    setColorMode(EPD_MODE_NORMAL, EPD_MODE_INVERT);
    setWindowX(0, SCREEN_WIDTH);
    setWindowY(0, SCREEN_HEIGHT);
    shortCommand1(CMD_DATA_ENTRY_MODE, 3);
    setPosXY(0, 0);
}
void beginWriteFramebuffer(bool color)
{
    if (color == EPD_COLOR_RED)
    {
        commandBegin(CMD_WRITE_FB_RED);
    }
    else
    {
        commandBegin(CMD_WRITE_FB_BW);
    }
    epdDeselect();
}
void endWriteFramebuffer()
{
    commandEnd();
}
void loadRawBitmap(uint8_t *bmp, uint16_t x, uint16_t y, bool color)
{
    uint16_t xsize = bmp[0] / 8;
    if (bmp[0] % 8)
        xsize++;
    uint16_t size = xsize * bmp[1];
    setWindowX(x, x + (xsize * 8));
    setWindowY(y, bmp[1] + y);
    setPosXY(x, y);
    shortCommand1(CMD_DATA_ENTRY_MODE, 3);
    if (color)
    {
        commandBegin(CMD_WRITE_FB_RED);
    }
    else
    {
        commandBegin(CMD_WRITE_FB_BW);
    }
    bmp += 2;
    while (size--)
    {
        epdSend(*(bmp++));
    }
    commandEnd();
}
void printBarcode(const uint8_t *string, uint16_t x, uint16_t y)
{
}
// stuff for printing text
static void pushXFontBytesToEPD(uint8_t byte1, uint8_t byte2)
{
    if (epdCharSize == 1)
    {
        uint8_t offset = 7 - (fontCurXpos % 8);
        for (uint8_t c = 0; c < 8; c++)
        {
            if (byte2 & (1 << (7 - c)))
                rbuffer[c] |= (1 << offset);
        }
        for (uint8_t c = 0; c < 8; c++)
        {
            if (byte1 & (1 << (7 - c)))
                rbuffer[8 + c] |= (1 << offset);
        }
        fontCurXpos++;
    }
    else
    {
        uint8_t offset = 6 - (fontCurXpos % 8);
        // double font size
        for (uint8_t c = 0; c < 8; c++)
        {
            if (byte2 & (1 << (7 - c)))
            {
                rbuffer[c * 2] |= (3 << offset);
                rbuffer[(c * 2) + 1] |= (3 << offset);
            }
        }
        for (uint8_t c = 0; c < 8; c++)
        {
            if (byte1 & (1 << (7 - c)))
            {
                rbuffer[(c * 2) + 16] |= (3 << offset);
                rbuffer[(c * 2) + 17] |= (3 << offset);
            }
        }
        fontCurXpos += 2;
    }
    if (fontCurXpos % 8 == 0)
    {
        // next byte, flush current byte to EPD
        for (uint8_t i = 0; i < (16 * epdCharSize); i++)
        {
            epdSend(rbuffer[i]);
        }
        memset(rbuffer, 0, 32);
    }
}
static void bufferByteShift(uint8_t byte)
{
    /*
                rbuffer[0] = 0;   // previous value
                rbuffer[1] = y%8; // offset
                rbuffer[2] = 0;   // current byte counter;
                rbuffer[3] = 1+(epdCharsize*2);
    */

    if (rbuffer[1] == 0)
    {
        epdSend(byte);
    }
    else
    {
        uint8_t offset = rbuffer[1];
        rbuffer[0] |= (byte >> offset);
        epdSend(rbuffer[0]);
        // epdSend(byte);
        rbuffer[0] = (byte << (8 - offset));
        rbuffer[2]++;
        if (rbuffer[2] == rbuffer[3])
        {
            epdSend(rbuffer[0]);
            rbuffer[0] = 0;
            rbuffer[2] = 0;
        }
    }
}
static void pushYFontBytesToEPD(uint8_t byte1, uint8_t byte2)
{
    if (epdCharSize == 2)
    {
        for (uint8_t j = 0; j < 2; j++)
        {
            uint8_t c = 0;
            for (uint8_t i = 7; i != 255; i--)
            {
                if (byte1 & (1 << i))
                    c |= (0x03 << ((i % 4) * 2));
                if ((i % 4) == 0)
                {
                    bufferByteShift(c);
                    c = 0;
                }
            }
            for (uint8_t i = 7; i != 255; i--)
            {
                if (byte2 & (1 << i))
                    c |= (0x03 << ((i % 4) * 2));
                if ((i % 4) == 0)
                {
                    bufferByteShift(c);
                    c = 0;
                }
            }
        }
    }
    else
    {
        bufferByteShift(byte1);
        bufferByteShift(byte2);
    }
}
void writeCharEPD(uint8_t c)
{
    // Writes a single character to the framebuffer
    bool empty = true;
    for (uint8_t i = 0; i < 20; i++)
    {
        if (font[c][i])
            empty = false;
    }
    if (empty)
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            if (directionY)
            {
                pushYFontBytesToEPD(0x00, 0x00);
            }
            else
            {
                pushXFontBytesToEPD(0x00, 0x00);
            }
        }
        return;
    }

    uint8_t begin = 0;
    while (font[c][begin] == 0x00 && font[c][begin + 1] == 0x00)
    {
        begin += 2;
    }

    uint8_t end = 20;
    while (font[c][end - 1] == 0x00 && font[c][end - 2] == 0x00)
    {
        end -= 2;
    }

    for (uint8_t pos = begin; pos < end; pos += 2)
    {
        if (directionY)
        {
            pushYFontBytesToEPD(font[c][pos + 1], font[c][pos]);
        }
        else
        {
            pushXFontBytesToEPD(font[c][pos], font[c][pos + 1]);
        }
    }

    // spacing between characters
    if (directionY)
    {
        pushYFontBytesToEPD(0x00, 0x00);
    }
    else
    {
        pushXFontBytesToEPD(0x00, 0x00);
    }
}

// Print text to the EPD. Origin is top-left
void epdPrintBegin(uint16_t x, uint16_t y, bool direction, bool fontsize, bool color)
{
    directionY = direction;
    epdCharSize = 1 + fontsize;
    if (directionY)
    {
        uint8_t extra = 0;

        // provisions for dealing with font in Y direction, byte-unaligned
        if (x % 8)
        {
            extra = 8;
            rbuffer[0] = 0;     // previous value
            rbuffer[1] = x % 8; // offset
            rbuffer[2] = 0;     // current byte counter;
            rbuffer[3] = (epdCharSize * 2);
        }
        else
        {
            rbuffer[1] = 0;
        }

        setWindowY(y, 1);
        if (epdCharSize == 2)
        {
            setWindowX(x, x + 32 + extra);
            setPosXY(x, y);
        }
        else
        {
            setWindowX(x, x + 16 + extra);
            setPosXY(x, y);
        }
        shortCommand1(CMD_DATA_ENTRY_MODE, 1); // was 3
    }
    else
    {
        if (epdCharSize == 2)
        {
            x /= 2;
            x *= 2;
            setWindowY(y, y + 32);
        }
        else
        {
            setWindowY(y, y + 16);
        }
        setPosXY(x, y);
        fontCurXpos = x;
        setWindowX(x, SCREEN_WIDTH);
        shortCommand1(CMD_DATA_ENTRY_MODE, 7);
        memset(rbuffer, 0, 32);
    }

    if (color)
    {
        commandBegin(CMD_WRITE_FB_RED);
    }
    else
    {
        commandBegin(CMD_WRITE_FB_BW);
    }
}
void epdPrintEnd()
{
    if (!directionY && ((fontCurXpos % 8) != 0))
    {
        for (uint8_t i = 0; i < (16 * epdCharSize); i++)
        {
            epdSend(rbuffer[i]);
        }
    }
    commandEnd();
}

extern uint8_t blockXferBuffer[];

void readRam()
{
    setWindowY(296, 0);
    setWindowX(0, 8);
    setPosXY(0, 296);
    shortCommand1(CMD_DATA_ENTRY_MODE, 1); // was 3
    shortCommand1(0x41, 0x00);
    commandReadBegin(0x27);
    epdReadByte();

    for (uint16_t c = 0; c < 293; c++)
    {
        blockXferBuffer[c] = epdReadByte() | 0x10;
    }
    commandReadEnd();
    commandBegin(CMD_WRITE_FB_BW);
    for (uint16_t c = 0; c < 296; c++)
    {
        epdSend(blockXferBuffer[c]);
    }
    commandEnd();
}

static void epdPutchar(uint8_t data)
{
    writeCharEPD(data);
}

void epdpr(const char *c, ...)
{
    char out_buffer[512]; // you can define your own buffer’s size

    va_list lst;
    va_start(lst, c);
    vsprintf(out_buffer, c, lst);
    va_end(lst);

    int posi = 0;
    while (out_buffer[posi] != 0)
    {
        epdPutchar(out_buffer[posi]);
        posi++;
    }
}
