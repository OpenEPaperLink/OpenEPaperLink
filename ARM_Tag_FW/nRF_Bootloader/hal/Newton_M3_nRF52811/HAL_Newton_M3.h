
#define RADIO_FIRST_CHANNEL				(11)		//2.4-GHz channels start at 11

#define eepromByte spiByte
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

#define EPD_RST 4
#define EPD_BS 2
#define EPD_CS 6
#define EPD_DC 5
#define EPD_BUSY 3
#define EPD_CLK 19
#define EPD_MOSI 20

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

void initRTC0(uint32_t ms);
int8_t startHFCLK(void);
uint8_t isHFCLKstable(void);
void boardGetOwnMac(uint8_t *mac);
void sleepForMs(uint32_t ms);
void setled(uint64_t parta,u_int32_t partb);
void resettimer();