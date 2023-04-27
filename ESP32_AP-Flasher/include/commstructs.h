#include <Arduino.h>
#pragma pack(push, 1)

struct espBlockRequest {
    uint8_t checksum;
    uint64_t ver;
    uint8_t blockId;
    uint8_t src[8];
} __packed;

struct espXferComplete {
    uint8_t checksum;
    uint8_t src[8];
} __packed;

struct espSetChannelPower {
    uint8_t checksum;
    uint8_t channel;
    uint8_t power;
} __packed;

struct blockData {
    uint16_t size;
    uint16_t checksum;
    uint8_t data[];
} __packed;

#define SOLUM_154_033 0
#define SOLUM_29_033 1
#define SOLUM_42_033 2

#define WAKEUP_REASON_TIMED 0
#define WAKEUP_REASON_GPIO 2
#define WAKEUP_REASON_NFC 3
#define WAKEUP_REASON_FIRSTBOOT 0xFC
#define WAKEUP_REASON_NETWORK_SCAN 0xFD
#define WAKEUP_REASON_WDT_RESET 0xFE

struct AvailDataReq {
    uint8_t checksum;
    uint8_t lastPacketLQI;
    int8_t lastPacketRSSI;
    int8_t temperature;
    uint16_t batteryMv;
    uint8_t hwType;
    uint8_t wakeupReason;
    uint8_t capabilities;
} __packed;

struct espAvailDataReq {
    uint8_t checksum;
    uint8_t src[8];
    struct AvailDataReq adr;
} __packed;

#define DATATYPE_NOUPDATE 0
#define DATATYPE_IMG_BMP 2
#define DATATYPE_FW_UPDATE 3
#define DATATYPE_IMG_DIFF 0x10             // always 1BPP
#define DATATYPE_IMG_RAW_1BPP 0x20         // 2888 bytes for 1.54"  / 4736 2.9" / 15000 4.2"
#define DATATYPE_IMG_RAW_2BPP 0x21         // 5776 bytes for 1.54"  / 9472 2.9" / 30000 4.2"
#define DATATYPE_IMG_RAW_1BPP_DIRECT 0x3F  // only for 1.54", don't write to EEPROM, but straightaway to the EPD
#define DATATYPE_NFC_RAW_CONTENT 0xA0      // raw memory content for the NT3H1101
#define DATATYPE_NFC_URL_DIRECT 0xA1       // URL format for NT3H1101

#define EPD_LUT_DEFAULT 0
#define EPD_LUT_NO_REPEATS 1
#define EPD_LUT_FAST_NO_REDS 2
#define EPD_LUT_FAST 3

struct AvailDataInfo {
    uint8_t checksum;
    uint64_t dataVer;              // MD5 of potential traffic
    uint32_t dataSize;              
    uint8_t dataType;          // allows for 16 different datatypes
    uint8_t dataTypeArgument;  // extra specification or instruction for the tag (LUT to be used for drawing image)
    uint16_t nextCheckIn;          // when should the tag check-in again? Measured in minutes
} __packed;

struct pendingData {
    struct AvailDataInfo availdatainfo;
    uint16_t attemptsLeft;
    uint8_t targetMac[8];
} __packed;

#define BLOCK_DATA_SIZE 4096
#define BLOCK_XFER_BUFFER_SIZE BLOCK_DATA_SIZE + sizeof(struct blockData)

#pragma pack(pop)