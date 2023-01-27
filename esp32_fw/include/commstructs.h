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

struct AvailDataReq {
    uint8_t checksum;
    uint8_t lastPacketLQI;  // zero if not reported/not supported to be reported
    int8_t lastPacketRSSI;  // zero if not reported/not supported to be reported
    uint8_t temperature;    // zero if not reported/not supported to be reported. else, this minus CHECKIN_TEMP_OFFSET is temp in degrees C
    uint16_t batteryMv;
    uint8_t softVer;
    uint8_t hwType;
    uint8_t protoVer;
    uint8_t buttonState;
} __packed;

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
    uint64_t dataVer;
    uint32_t dataSize;
    uint8_t dataType;
    uint16_t nextCheckIn;
} __packed;

struct pendingData {
    struct AvailDataInfo availdatainfo;
    uint16_t attemptsLeft;
    uint8_t targetMac[8];
} __packed;

#define BLOCK_DATA_SIZE 4096
#define BLOCK_XFER_BUFFER_SIZE BLOCK_DATA_SIZE + sizeof(struct blockData)