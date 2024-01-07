#ifndef _PROTO_H_
#define _PROTO_H_
#include <stdint.h>

#include "tag_types.h"

// power saving algorithm
#define INTERVAL_BASE 40              // interval (in seconds) (when 1 packet is sent/received) for target current (7.2µA)
#define INTERVAL_AT_MAX_ATTEMPTS 600  // interval (in seconds) (at max attempts) for target average current
#define DATA_REQ_RX_WINDOW_SIZE 5UL   // How many milliseconds we should wait for a packet during the data_request.
                                      // If the AP holds a long list of data for tags, it may need a little more time to lookup the mac address
#define DATA_REQ_MAX_ATTEMPTS 14      // How many attempts (at most) we should do to get something back from the AP
#define POWER_SAVING_SMOOTHING 8      // How many samples we should use to smooth the data request interval
#define MINIMUM_INTERVAL 45           // IMPORTANT: Minimum interval for check-in; this determines overal battery life!
#define MAXIMUM_PING_ATTEMPTS 20      // How many attempts to discover an AP the tag should do

#define LONG_DATAREQ_INTERVAL 300     // How often (in seconds, approximately) the tag should do a long datareq (including temperature)
#define VOLTAGE_CHECK_INTERVAL 288    // How often the tag should do a battery voltage check (multiplied by LONG_DATAREQ_INTERVAL)
#define BATTERY_VOLTAGE_MINIMUM 2450  // 2600 or below is the best we can do on the EPD

// power saving when no AP's were found (scanning every X)
#define VOLTAGEREADING_DURING_SCAN_INTERVAL 2  // how often we should read voltages; this is done every scan attempt in interval bracket 3
#define INTERVAL_1_TIME 3600UL                 // Try every hour
#define INTERVAL_1_ATTEMPTS 24                 // for 24 attempts (an entire day)
#define INTERVAL_2_TIME 7200UL                 // Try every 2 hours
#define INTERVAL_2_ATTEMPTS 12                 // for 12 attempts (an additional day)
#define INTERVAL_3_TIME 86400UL                // Finally, try every day

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
} ;

struct MacFrameFromMaster {
    struct MacFcs fcs;
    uint8_t seq;
    uint16_t pan;
    uint8_t dst[8];
    uint16_t from;
} ;

struct MacFrameNormal {
    struct MacFcs fcs;
    uint8_t seq;
    uint16_t pan;
    uint8_t dst[8];
    uint8_t src[8];
} ;

struct MacFrameBcast {
    struct MacFcs fcs;
    uint8_t seq;
    uint16_t dstPan;
    uint16_t dstAddr;
    uint16_t srcPan;
    uint8_t src[8];
} ;

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
} ;

struct oldAvailDataReq {
    uint8_t checksum;
    uint8_t lastPacketLQI;
    int8_t lastPacketRSSI;
    int8_t temperature;
    uint16_t batteryMv;
    uint8_t hwType;
    uint8_t wakeupReason;
    uint8_t capabilities;
} ;

struct AvailDataInfo {
    uint8_t checksum;
    uint64_t dataVer;  // MD5 of potential traffic
    uint32_t dataSize;
    uint8_t dataType;
    uint8_t dataTypeArgument;  // extra specification or instruction for the tag (LUT to be used for drawing image)
    uint16_t nextCheckIn;      // when should the tag check-in again? Measured in minutes
} ;

struct pendingData {
    struct AvailDataInfo availdatainfo;
    uint16_t attemptsLeft;
    uint8_t targetMac[8];
} ;

struct blockPart {
    uint8_t checksum;
    uint8_t blockId;
    uint8_t blockPart;
    uint8_t data[];
} ;

struct blockData {
    uint16_t size;
    uint16_t checksum;
    uint8_t data[];
} ;

struct burstMacData {
    uint16_t offset;
    uint8_t targetMac[8];
} ;

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
} ;

struct blockRequestAck {
    uint8_t checksum;
    uint16_t pleaseWaitMs;
} ;

struct espBlockRequest {
    uint8_t checksum;
    uint64_t ver;
    uint8_t blockId;
    uint8_t src[8];
} ;

struct espXferComplete {
    uint8_t checksum;
    uint8_t src[8];
} ;

struct espAvailDataReq {
    uint8_t checksum;
    uint8_t src[8];
    struct AvailDataReq adr;
} ;

struct espSetChannelPower {
    uint8_t checksum;
    uint8_t channel;
    uint8_t power;
} ;

#define MACFMT "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x"
#define MACCVT(x) ((const uint8_t*)(x))[7], ((const uint8_t*)(x))[6], ((const uint8_t*)(x))[5], ((const uint8_t*)(x))[4], ((const uint8_t*)(x))[3], ((const uint8_t*)(x))[2], ((const uint8_t*)(x))[1], ((const uint8_t*)(x))[0]

#endif
