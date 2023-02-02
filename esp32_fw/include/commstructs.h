#include <Arduino.h>
#pragma pack(push, 1)

struct espBlockRequest {
    uint8_t checksum;
    uint64_t ver;
    uint8_t blockId;
} __packed;

struct espXferComplete {
    uint8_t checksum;
    uint8_t src[8];
} __packed;

struct blockData {
    uint16_t size;
    uint16_t checksum;
    uint8_t data[];
} __packed;

#define SOLUM_154_033 0
#define SOLUM_29_033 1
#define SOLUM_42_033 2

#define WAKEUP_REASON_TIMED     0
#define WAKEUP_REASON_BOOTUP    1
#define WAKEUP_REASON_GPIO      2
#define WAKEUP_REASON_NFC       3

struct AvailDataReq {
    uint8_t checksum;
    uint8_t lastPacketLQI : 7;
    uint8_t lastPacketRSSI : 7;  // is negative
    int8_t temperature : 7;      // zero if not reported/not supported to be reported. else, this minus CHECKIN_TEMP_OFFSET is temp in degrees C
    uint16_t batteryMv : 12;
    uint8_t hwType : 5;          // 32 types of tags supported
    uint8_t wakeupReason : 2;    // supports 4 types of wakeup reasons
    uint8_t capabilities;       // undefined, as of now
} __packed; // 7 bytes

struct espAvailDataReq {
    uint8_t checksum;
    uint8_t src[8];
    struct AvailDataReq adr;
} __packed;

#define DATATYPE_NOUPDATE 0
#define DATATYPE_IMG 1
#define DATATYPE_IMGRAW 2
#define DATATYPE_UPDATE 3

struct AvailDataInfo {
    uint8_t checksum;
    uint64_t dataVer;              // MD5 of potential traffic
    uint32_t dataSize;              
    uint8_t dataType : 4;          // allows for 16 different datatypes
    uint8_t dataTypeArgument : 4;  // extra specification or instruction for the tag (LUT to be used for drawing image)
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