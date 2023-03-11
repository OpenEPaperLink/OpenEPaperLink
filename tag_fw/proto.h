#ifndef _PROTO_H_
#define _PROTO_H_
#define __packed
#include <stdint.h>

enum TagScreenType {
    TagScreenEink_BW_1bpp,
    TagScreenEink_BW_2bpp,
    TagScreenEink_BW_4bpp,
    TagScreenEink_BWY_only,  // 2bpp, but only 3 colors (BW?Y)
    TagScreenEink_BWY_2bpp,
    TagScreenEink_BWY_4bpp,
    TagScreenEink_BWR_only,  // 2bpp, but only 3 colors (BW?R)
    TagScreenEink_BWR_2bpp,
    TagScreenEink_BWR_4bpp,

    TagScreenEink_BWY_3bpp,
    TagScreenEink_BWR_3bpp,
    TagScreenEink_BW_3bpp,

    TagScreenPersistentLcd_1bpp,

    TagScreenEink_BWY_5colors,
    TagScreenEink_BWR_5colors,

    TagScreenEink_BWY_6colors,
    TagScreenEink_BWR_6colors,

    TagScreenTypeOther = 0x7f,
};

#define SOLUM_154_033 0
#define SOLUM_29_033 1
#define SOLUM_42_033 2

#ifndef __packed
#define __packed __attribute__((packed))
#endif

#define PROTO_PAN_ID (0x4447)  // PAN ID compression shall be used

#define RADIO_MAX_PACKET_LEN (125)  // useful payload, not including the crc

#define ADDR_MODE_NONE (0)
#define ADDR_MODE_SHORT (2)
#define ADDR_MODE_LONG (3)

#define FRAME_TYPE_BEACON (0)
#define FRAME_TYPE_DATA (1)
#define FRAME_TYPE_ACK (2)
#define FRAME_TYPE_MAC_CMD (3)

#define SHORT_MAC_UNUSED (0x10000000UL)  // for radioRxFilterCfg's myShortMac

struct MacFcs {
    uint8_t frameType : 3;
    uint8_t secure : 1;
    uint8_t framePending : 1;
    uint8_t ackReqd : 1;
    uint8_t panIdCompressed : 1;
    uint8_t rfu1 : 1;
    uint8_t rfu2 : 2;
    uint8_t destAddrType : 2;
    uint8_t frameVer : 2;
    uint8_t srcAddrType : 2;
} __packed;

struct MacFrameFromMaster {
    struct MacFcs fcs;
    uint8_t seq;
    uint16_t pan;
    uint8_t dst[8];
    uint16_t from;
} __packed;

struct MacFrameNormal {
    struct MacFcs fcs;
    uint8_t seq;
    uint16_t pan;
    uint8_t dst[8];
    uint8_t src[8];
} __packed;

struct MacFrameBcast {
    struct MacFcs fcs;
    uint8_t seq;
    uint16_t dstPan;
    uint16_t dstAddr;
    uint16_t srcPan;
    uint8_t src[8];
} __packed;

#define PKT_AVAIL_DATA_SHORTREQ 0xE3
#define PKT_AVAIL_DATA_REQ 0xE5
#define PKT_AVAIL_DATA_INFO 0xE6
#define PKT_BLOCK_PARTIAL_REQUEST 0xE7
#define PKT_BLOCK_REQUEST_ACK 0xE9
#define PKT_BLOCK_REQUEST 0xE4
#define PKT_BLOCK_PART 0xE8
#define PKT_XFER_COMPLETE 0xEA
#define PKT_XFER_COMPLETE_ACK 0xEB
#define PKT_CANCEL_XFER 0xEC
#define PKT_PING 0xED
#define PKT_PONG 0xEE

#define PKT_EVENT_PONG 0xC1
#define PKT_EVENT_DATA_REQ 0xC2
#define PKT_EVENT_DATA 0xC3

struct AvailDataReq {
    uint8_t checksum;
    uint8_t lastPacketLQI;
    int8_t lastPacketRSSI;
    int8_t temperature;
    uint16_t batteryMv;
    uint8_t hwType;
    uint8_t wakeupReason;
    uint8_t capabilities;  // undefined, as of now
} __packed;

#define DATATYPE_NOUPDATE 0
#define DATATYPE_IMG_BMP 2
#define DATATYPE_FW_UPDATE 3
#define DATATYPE_IMG_DIFF 0x10             // always 1BPP
#define DATATYPE_IMG_RAW_1BPP 0x20         // 2888 bytes for 1.54"  / 4736 2.9" / 15000 4.2"
#define DATATYPE_IMG_RAW_2BPP 0x21         // 5776 bytes for 1.54"  / 9472 2.9" / 30000 4.2"
#define DATATYPE_IMG_RAW_1BPP_DIRECT 0x3F  // only for 1.54", don't write to EEPROM, but straightaway to the EPD

struct AvailDataInfo {
    uint8_t checksum;
    uint64_t dataVer;  // MD5 of potential traffic
    uint32_t dataSize;
    uint8_t dataType;
    uint8_t dataTypeArgument;  // extra specification or instruction for the tag (LUT to be used for drawing image)
    uint16_t nextCheckIn;      // when should the tag check-in again? Measured in minutes
} __packed;

struct blockPart {
    uint8_t checksum;
    uint8_t blockId;
    uint8_t blockPart;
    uint8_t data[];
} __packed;

struct blockData {
    uint16_t size;
    uint16_t checksum;
    uint8_t data[];
} __packed;

struct burstMacData {
    uint16_t offset;
    uint8_t targetMac[8];
} __packed;

struct eventData {
    uint8_t checksum;
    uint8_t eventDataID;
    uint8_t data[];
} __packed;

#define BLOCK_PART_DATA_SIZE 99
#define BLOCK_MAX_PARTS 42
#define BLOCK_DATA_SIZE 4096UL
#define BLOCK_XFER_BUFFER_SIZE BLOCK_DATA_SIZE + sizeof(struct blockData)
#define BLOCK_REQ_PARTS_BYTES 6

struct blockRequest {
    uint8_t checksum;
    uint64_t ver;
    uint8_t blockId;
    uint8_t type;
    uint8_t requestedParts[BLOCK_REQ_PARTS_BYTES];
} __packed;

struct blockRequestAck {
    uint8_t checksum;
    uint16_t pleaseWaitMs;
} __packed;

#define MACFMT "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x"
#define MACCVT(x) ((const uint8_t*)(x))[7], ((const uint8_t*)(x))[6], ((const uint8_t*)(x))[5], ((const uint8_t*)(x))[4], ((const uint8_t*)(x))[3], ((const uint8_t*)(x))[2], ((const uint8_t*)(x))[1], ((const uint8_t*)(x))[0]

#endif