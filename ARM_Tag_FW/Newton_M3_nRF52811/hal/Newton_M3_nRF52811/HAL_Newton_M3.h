#include "zigbee.h"
#include "epd_spi.h"
#include "eeprom_spi.h"

#define RADIO_FIRST_CHANNEL (11)  // 2.4-GHz channels start at 11

// #define eepromByte spiByte ??
#define eepromPrvSelect()            \
    do {                             \
        digitalWrite(FLASH_CS, LOW); \
    } while (0)
#define eepromPrvDeselect()           \
    do {                              \
        digitalWrite(FLASH_CS, HIGH); \
    } while (0)

#define LED_RED 16
#define LED_GREEN 17
#define LED_BLUE 18

#define BUTTON1 28
#define BUTTON2 29
#define PEGHOOK_BUTTON 23

#define EPD_RST 4
#define EPD_BS 2
#define EPD_CS 6
#define EPD_DC 5
#define EPD_BUSY 3
#define EPD_CLK 19
#define EPD_MOSI 20

// these are used by the UC8159 controller.
// EPD_HLT is External (EPD)EEPROM _CS
#define EPD_HLT 23
// This is the EEPROM MISO pin (from SPI EEPROM to nRF)
#define EPD_VPP 24

#define NFC_I2C_SDA 8
#define NFC_I2C_SCL 9
#define NFC_POWER 10
#define NFC_IRQ 11

#define FLASH_CS 12
#define FLASH_MISO 13
#define FLASH_CLK 14
#define FLASH_MOSI 15

#define EPD_POWER 7

// Can be used as wanted UART RXD and TXD are used for Debug output
#define DBG_RST 21
#define DBG_DL 22
#define DBG_TXD 25
#define DBG_RXD 26
#define DBG_TEST 27

#define EEPROM_SETTINGS_AREA_START 0

void initRTC0(uint32_t ms);
int8_t startHFCLK(void);
uint8_t isHFCLKstable(void);
void boardGetOwnMac(uint8_t *mac);
void sleepForMs(uint32_t ms);
void setled(uint64_t parta, u_int32_t partb);
void resettimer();

extern uint16_t batteryVoltage;
extern bool lowBattery;
extern int8_t temperature;
//extern bool disablePinInterruptSleep;

void setupBatteryVoltage();
void getVoltage();
void setupTemperature();
void getTemperature();


class epdInterface {
   public:
    virtual void epdSetup() = 0;
    virtual void epdEnterSleep() = 0;
    virtual void draw() = 0;
    virtual void drawNoWait() = 0;
    virtual void epdWaitRdy() = 0;
    virtual void selectLUT(uint8_t lut) = 0;
    uint8_t controllerType = 0;
    uint16_t Xres;
    uint16_t Yres;
    uint16_t effectiveXRes;
    uint16_t effectiveYRes;
    uint16_t XOffset = 0;
    uint16_t YOffset = 0;
    bool drawDirectionRight = false;
    bool epdMirrorV = false;
    bool epdMirrorH = false;
//    bool mirrorV = false;
//    bool mirrorH = false;
   protected:
    virtual void epdWriteDisplayData() = 0;
};

extern epdInterface* epd;

#define NRF_BOARDTYPE_REGULAR 0
#define NRF_BOARDTYPE_PEGHOOK 1

struct tagSpecs {
    uint8_t buttonCount = 0;
    bool hasNFC = false;
    bool hasLED = false;
    bool ledInverted = false;
    uint8_t thirdColor = false;
    uint8_t boardType = NRF_BOARDTYPE_REGULAR;
    uint16_t macSuffix = 0x0000;
    uint8_t OEPLtype = 0;
    uint8_t solumType = 0;
    uint32_t imageSize = 0;
};

extern tagSpecs tag;