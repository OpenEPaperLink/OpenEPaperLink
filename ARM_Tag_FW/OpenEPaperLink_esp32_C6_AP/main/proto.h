#ifndef _PROTO_H_
#define _PROTO_H_
#include <stdint.h>

#define LED1 22
#define LED2 23

#define PROTO_PAN_ID (0x4447)  // PAN ID compression shall be used
#define PROTO_PAN_ID_SUBGHZ   (0x1337)  // PAN ID compression shall be used

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
} __attribute__((packed, aligned(1)));

struct MacFrameFromMaster {
    struct MacFcs fcs;
    uint8_t seq;
    uint16_t pan;
    uint8_t dst[8];
    uint16_t from;
} __attribute__((packed, aligned(1)));

struct MacFrameNormal {
    struct MacFcs fcs;
    uint8_t seq;
    uint16_t pan;
    uint8_t dst[8];
    uint8_t src[8];
} __attribute__((packed, aligned(1)));

struct MacFrameBcast {
    struct MacFcs fcs;
    uint8_t seq;
    uint16_t dstPan;
    uint16_t dstAddr;
    uint16_t srcPan;
    uint8_t src[8];
} __attribute__((packed, aligned(1)));

#define PKT_TAG_RETURN_DATA 0xE1
#define PKT_TAG_RETURN_DATA_ACK 0xE2 
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

struct AvailDataReq {
    uint8_t checksum;
    uint8_t lastPacketLQI;
    int8_t lastPacketRSSI;
    int8_t temperature;
    uint16_t batteryMv;
    uint8_t hwType;
    uint8_t wakeupReason;
    uint8_t capabilities;
    uint16_t tagSoftwareVersion;
    uint8_t currentChannel;
    uint8_t customMode;
    uint8_t reserved[8];
} __attribute__((packed, aligned(1)));

struct oldAvailDataReq {
    uint8_t checksum;
    uint8_t lastPacketLQI;
    int8_t lastPacketRSSI;
    int8_t temperature;
    uint16_t batteryMv;
    uint8_t hwType;
    uint8_t wakeupReason;
    uint8_t capabilities;
} __attribute__((packed, aligned(1)));

struct AvailDataInfo {
    uint8_t checksum;
    uint64_t dataVer;  // MD5 of potential traffic
    uint32_t dataSize;
    uint8_t dataType;
    uint8_t dataTypeArgument;  // extra specification or instruction for the tag (LUT to be used for drawing image)
    uint16_t nextCheckIn;      // when should the tag check-in again? Measured in minutes
} __attribute__((packed, aligned(1)));

struct pendingData {
    struct AvailDataInfo availdatainfo;
    uint16_t attemptsLeft;
    uint8_t targetMac[8];
} __attribute__((packed, aligned(1)));

struct blockPart {
    uint8_t checksum;
    uint8_t blockId;
    uint8_t blockPart;
    uint8_t data[];
} __attribute__((packed, aligned(1)));

struct blockData {
    uint16_t size;
    uint16_t checksum;
    uint8_t data[];
} __attribute__((packed, aligned(1)));

#define TAG_RETURN_DATA_SIZE 90

struct tagReturnData {
	uint8_t checksum;
	uint8_t partId;
	uint64_t dataVer;
	uint8_t dataType;
	uint8_t data[TAG_RETURN_DATA_SIZE];
} __attribute__((packed, aligned(1)));

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
} __attribute__((packed, aligned(1)));

struct blockRequestAck {
    uint8_t checksum;
    uint16_t pleaseWaitMs;
} __attribute__((packed, aligned(1)));

struct espBlockRequest {
    uint8_t checksum;
    uint64_t ver;
    uint8_t blockId;
    uint8_t src[8];
} __attribute__((packed, aligned(1)));

struct espXferComplete {
    uint8_t checksum;
    uint8_t src[8];
} __attribute__((packed, aligned(1)));

struct espAvailDataReq {
    uint8_t checksum;
    uint8_t src[8];
    struct AvailDataReq adr;
} __attribute__((packed, aligned(1)));

struct espSetChannelPower {
    uint8_t checksum;
    uint8_t channel;
    uint8_t power;
#ifdef CONFIG_OEPL_SUBGIG_SUPPORT
    uint8_t subghzchannel;
#endif
} __attribute__((packed, aligned(1)));

struct espTagReturnData {
	uint8_t checksum;
	uint8_t src[8];
	uint8_t len;
	struct tagReturnData returnData;
} __attribute__((packed, aligned(1)));

#endif
