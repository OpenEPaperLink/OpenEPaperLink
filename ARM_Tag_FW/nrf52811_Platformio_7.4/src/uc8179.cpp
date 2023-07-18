#include "uc8179.h"

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

enum PSR_FLAGS
{
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

enum PWR_FLAGS_1
{
    VDS_EXTERNAL = 0b00000000,
    VDS_INTERNAL = 0b00000010,

    VDG_EXTERNAL = 0b00000000,
    VDG_INTERNAL = 0b00000001
};

enum PWR_FLAGS_2
{
    VCOM_VD = 0b00000000,
    VCOM_VG = 0b00000100,

    VGHL_16V = 0b00000000,
    VGHL_15V = 0b00000001,
    VGHL_14V = 0b00000010,
    VGHL_13V = 0b00000011
};

enum BOOSTER_FLAGS
{
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

enum PFS_FLAGS
{
    FRAMES_1 = 0b00000000,
    FRAMES_2 = 0b00010000,
    FRAMES_3 = 0b00100000,
    FRAMES_4 = 0b00110000
};

enum TSE_FLAGS
{
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

enum PLL_FLAGS
{
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
static uint16_t fontCurYpos = 0; // current Y value we're working with
static uint8_t currentLut = 0;
static uint8_t dispLutSize = 0;
static bool drawDirection = false;
static bool isInited = false;

bool epdGPIOActive = false;

#define LUT_BUFFER_SIZE 128
uint8_t waveformbuffer[LUT_BUFFER_SIZE];
struct waveform10 *waveform10 = (struct waveform10 *)waveformbuffer; // holds the LUT/waveform
struct waveform *waveform7 = (struct waveform *)waveformbuffer;      // holds the LUT/waveform

void epd_busy()
{
}
static void epdBusyWait(uint32_t timeout)
{

    uint32_t start = millis();
    wdt120s();
    attachInterrupt(digitalPinToInterrupt(EPD_BUSY), epd_busy, RISING);
    NRF_GPIO->PIN_CNF[EPD_BUSY] &= ~((uint32_t)GPIO_PIN_CNF_SENSE_Msk);
    NRF_GPIO->PIN_CNF[EPD_BUSY] |= ((uint32_t)GPIO_PIN_CNF_SENSE_High << GPIO_PIN_CNF_SENSE_Pos);
    while (millis() - start < timeout)
    {
        initRTC0(100);
        __WFE();
        __SEV();
        __WFE();
        if (digitalRead(EPD_BUSY))
        {
            printf("Screen done %lu ms\n", millis() - start);
            wdt30s();
            return;
        }
    }
    printf("screen timeout %lu ms :(\n", millis() - start);
    while (1)
    {
        if (digitalRead(EPD_BUSY))
        {
            printf("Screen done %lu ms\n", millis() - start);
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
    digitalWrite(EPD_RST, HIGH);
    delay(12);
    digitalWrite(EPD_RST, LOW);
    delay(20);
    digitalWrite(EPD_RST, HIGH);
    delay(20);
}
void epdConfigGPIO(bool setup)
{
    if (epdGPIOActive == setup)
        return;
    if (setup)
    {
        pinMode(EPD_POWER, OUTPUT);
        digitalWrite(EPD_POWER, HIGH);
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
        digitalWrite(EPD_POWER, LOW);
        pinMode(EPD_POWER, DEFAULT);
    }
    epdGPIOActive = setup;
}
void epdEnterSleep()
{
    shortCommand1(CMD_VCOM_INTERVAL, 0x17);
    shortCommand1(CMD_VCOM_DC_SETTING, 0x00);
    // shortCommand(CMD_POWER_OFF);
    // epdWaitRdy();
    shortCommand1(CMD_DEEP_SLEEP, 0xA5);
    isInited = false;
}

static void epdDrawDirection(bool direction)
{
    if (direction == drawDirection)
        return;

    drawDirection = direction;

    uint8_t psr_setting = RES_160x296 | FORMAT_BWR | BOOSTER_ON | RESET_NONE | LUT_OTP | SHIFT_RIGHT;
    if (drawDirection)
    {
        psr_setting |= SCAN_DOWN;
    }
    else
    {
        psr_setting |= SCAN_UP;
    }
    shortCommand2(CMD_PANEL_SETTING, psr_setting, 0);
}

void epdSetup()
{
    epdReset();

    drawDirection = false;
    epdDrawDirection(true);

    shortCommand2(CMD_VCOM_INTERVAL, 0x30, 0x07);

    commandBegin(CMD_RESOLUTION_SETING);
    epdSend(SCREEN_WIDTH >> 8);
    epdSend(SCREEN_WIDTH & 0xFF);
    epdSend(SCREEN_HEIGHT >> 8);
    epdSend(SCREEN_HEIGHT & 0xFF);
    commandEnd();


    /* commandBegin(CMD_POWER_SETTING);
     epdSend(VDS_INTERNAL | VDG_INTERNAL);
     epdSend(VCOM_VD | VGHL_16V);
     epdSend(0b101011);
     epdSend(0b101011);
     epdSend(0b101011);
     commandEnd();

     shortCommand(CMD_POWER_ON);
     epdWaitRdy();

     commandBegin(CMD_BOOSTER_SOFT_START);
     epdSend(START_10MS | STRENGTH_3 | OFF_6_58US);
     epdSend(START_10MS | STRENGTH_3 | OFF_6_58US);
     epdSend(START_10MS | STRENGTH_3 | OFF_6_58US);
     commandEnd();

     commandBegin(CMD_RESOLUTION_SETING);
     epdSend(SCREEN_WIDTH >> 8);
     epdSend(SCREEN_WIDTH & 0xFF);
     epdSend(SCREEN_HEIGHT >> 8);
     epdSend(SCREEN_HEIGHT & 0xFF);
     commandEnd();

     shortCommand1(CMD_POWER_OFF_SEQUENCE, FRAMES_1);
     shortCommand1(CMD_TEMPERATURE_SELECT, TEMP_INTERNAL | OFFSET_0);
     shortCommand1(CMD_TCON_SETTING, 0x22);
     shortCommand1(CMD_VCOM_INTERVAL, 0x8d);  // 0x87
     shortCommand1(CMD_PLL_CONTROL, HZ_200);
     epdWaitRdy();*/
    shortCommand(CMD_POWER_ON);
    epdWaitRdy();
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
    return 0;
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
    // implement alternative LUTs here. Currently just reset the watchdog to two minutes,
    // to ensure it doesn't reset during the much longer bootup procedure
    lut += 1; // make the compiler a happy camper
    wdt120s();
    return;
}

void setWindowXY(uint16_t xstart, uint16_t xend, uint16_t ystart, uint16_t yend)
{
    shortCommand(CMD_PARTIAL_IN);
    commandBegin(CMD_PARTIAL_WINDOW);
    epdSend(xstart >> 8);
    epdSend(xstart & 0xFF);
    epdSend((xend - 1) >> 8);
    epdSend((xend - 1) & 0xff);
    epdSend(ystart >> 8);
    epdSend(ystart & 0xFF);
    epdSend((yend - 1) >> 8);
    epdSend((yend - 1) & 0xff);
    epdSend(0x01);
    commandEnd();
}

void setColorMode(uint8_t red, uint8_t bw)
{
    // this does exactly nothing, just keeps the compiler from barking
    red = 1;
    bw = 0;
    return;
}
void clearScreen()
{
    shortCommand(CMD_PARTIAL_OUT);
    commandBegin(CMD_DISPLAY_START_TRANSMISSION_DTM2);
    for (uint16_t c = 0; c < ((1UL * SCREEN_HEIGHT * SCREEN_WIDTH) / 8); c++)
    {
        epdSend(0x00);
    }
    commandEnd();
    epdWaitRdy();
    commandBegin(CMD_DISPLAY_START_TRANSMISSION_DTM1);
    for (uint16_t c = 0; c < ((1UL * SCREEN_HEIGHT * SCREEN_WIDTH) / 8); c++)
    {
        epdSend(0x00);
    }
    commandEnd();
}
void draw()
{
    shortCommand(CMD_DISPLAY_REFRESH);
    epdWaitRdy();
}
void drawNoWait()
{
    shortCommand(CMD_DISPLAY_REFRESH);
}
void drawWithSleep()
{
    shortCommand(CMD_DISPLAY_REFRESH);
    epdBusyWait(120000);
}
void epdWaitRdy()
{
    epdBusyWait(120000);
}
void beginFullscreenImage()
{
    shortCommand(CMD_PARTIAL_OUT);
    epdDrawDirection(false);
    // shortCommand1(CMD_DATA_ENTRY_MODE, 3);
    // setPosXY(0, 0);
}
void beginWriteFramebuffer(bool color)
{
    if (color == EPD_COLOR_RED)
    {
        commandBegin(CMD_DISPLAY_START_TRANSMISSION_DTM2);
    }
    else
    {
        commandBegin(CMD_DISPLAY_START_TRANSMISSION_DTM1);
    }
    epdDeselect();
}
void endWriteFramebuffer()
{
    commandEnd();
}

void loadRawBitmap(uint8_t *bmp, uint16_t x, uint16_t y, bool color)
{
    // this function is very badly hurt by the switch to UC8151, taking up LOTS of valuable idata space. Only defining variables
    // as static, or the function as reentrant (relegating variables to the stack) seemed to fix the idata issue. Fix me, or put me out of my misery...

    uint16_t xsize = bmp[0] / 8;
    if (bmp[0] % 8)
        xsize++;
    uint16_t ysize = bmp[1];
    uint16_t size = xsize * bmp[1];

    // shortCommand1(CMD_DATA_ENTRY_MODE, 3);

    bmp += 2;

    uint16_t c = 0;
    uint16_t curY = y;
    while (1)
    {
        if (c % xsize == 0)
        {
            commandEnd();
            if (drawDirection)
            {
                setWindowXY(x, x + xsize * 8, SCREEN_HEIGHT - curY - 1, SCREEN_HEIGHT - curY);
            }
            else
            {
                setWindowXY(x, x + xsize * 8, curY - 1, curY);
            }
            curY++;
            if (color)
            {
                commandBegin(CMD_DISPLAY_START_TRANSMISSION_DTM2);
            }
            else
            {
                commandBegin(CMD_DISPLAY_START_TRANSMISSION_DTM1);
            }
        }
        epdSend(*(bmp++));
        c++;
        if (!size--)
            break;
    }
    commandEnd();
    shortCommand(CMD_PARTIAL_OUT);
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

        // setWindowY(y, 1);
        if (epdCharSize == 2)
        {
            setWindowXY(x, x + 32 + extra, SCREEN_HEIGHT - y, SCREEN_HEIGHT);
            // setPosXY(x, y);
        }
        else
        {
            setWindowXY(x, x + 16 + extra, SCREEN_HEIGHT - y, SCREEN_HEIGHT);
            // setPosXY(x, y);
        }
        // shortCommand1(CMD_DATA_ENTRY_MODE, 1);  // was 3
    }
    else
    {
        if (epdCharSize == 2)
        {
            x /= 2;
            x *= 2;
            setWindowXY(x, SCREEN_WIDTH, y, y + 32);
        }
        else
        {
            setWindowXY(x, SCREEN_WIDTH, y, y + 16);
        }
        // setPosXY(x, y);
        fontCurXpos = x;
        // setWindowXY(x, SCREEN_WIDTH);
        //  shortCommand1(CMD_DATA_ENTRY_MODE, 7);
        memset(rbuffer, 0, 32);
    }

    if (color)
    {
        commandBegin(CMD_DISPLAY_START_TRANSMISSION_DTM2);
    }
    else
    {
        commandBegin(CMD_DISPLAY_START_TRANSMISSION_DTM1);
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
    shortCommand(CMD_PARTIAL_OUT);
    epdDrawDirection(true);
}

extern uint8_t blockXferBuffer[];

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
