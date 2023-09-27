
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
} __packed;

struct oldAvailDataReq {
    uint8_t checksum;
    uint8_t lastPacketLQI;
    int8_t lastPacketRSSI;
    int8_t temperature;
    uint16_t batteryMv;
    uint8_t hwType;
    uint8_t wakeupReason;
    uint8_t capabilities;
} __packed;

struct AvailDataInfo {
    uint8_t checksum;
    uint64_t dataVer;  // MD5 of potential traffic
    uint32_t dataSize;
    uint8_t dataType;
    uint8_t dataTypeArgument;  // extra specification or instruction for the tag (LUT to be used for drawing image)
    uint16_t nextCheckIn;      // when should the tag check-in again? Measured in minutes
} __packed;

struct pendingData {
    struct AvailDataInfo availdatainfo;
    uint16_t attemptsLeft;
    uint8_t targetMac[8];
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

#define TAG_RETURN_DATA_SIZE 90

struct tagReturnData {
    uint8_t checksum;
    uint8_t partId;
    uint64_t dataVer;
    uint8_t dataType;
    uint8_t data[TAG_RETURN_DATA_SIZE];
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

struct tagsettings {
    uint8_t settingsVer;                  // the version of the struct as written to the infopage
    uint8_t enableFastBoot;               // default 0; if set, it will skip splashscreen
    uint8_t enableRFWake;                 // default 0; if set, it will enable RF wake. This will add about ~0.9µA idle power consumption
    uint8_t enableTagRoaming;             // default 0; if set, the tag will scan for an accesspoint every few check-ins. This will increase power consumption quite a bit
    uint8_t enableScanForAPAfterTimeout;  // default 1; if a the tag failed to check in, after a few attempts it will try to find a an AP on other channels
    uint8_t enableLowBatSymbol;           // default 1; tag will show 'low battery' icon on screen if the battery is depleted
    uint8_t enableNoRFSymbol;             // default 1; tag will show 'no signal' icon on screen if it failed to check in for a longer period of time
    uint8_t fastBootCapabilities;         // holds the byte with 'capabilities' as detected during a normal tag boot; allows the tag to skip detecting buttons and NFC chip
    uint8_t customMode;                   // default 0; if anything else, tag will bootup in a different 'mode'
    uint16_t batLowVoltage;               // Low battery threshold voltage (2450 for 2.45v). defaults to BATTERY_VOLTAGE_MINIMUM from powermgt.h
    uint16_t minimumCheckInTime;          // defaults to BASE_INTERVAL from powermgt.h
    uint8_t fixedChannel;                 // default 0; if set to a valid channel number, the tag will stick to that channel
} __packed;

struct ledFlash {
    uint8_t mode : 4;
    uint8_t flashDuration : 4;
    uint8_t color1;
    uint8_t flashCount1 : 4;
    uint8_t flashSpeed1 : 4;
    uint8_t delay1;
    uint8_t color2;
    uint8_t flashCount2 : 4;
    uint8_t flashSpeed2 : 4;
    uint8_t delay2;
    uint8_t color3;
    uint8_t flashCount3 : 4;
    uint8_t flashSpeed3 : 4;
    uint8_t delay3;
    uint8_t repeats;
    uint8_t spare;
} __packed;

struct imageDataTypeArgStruct {
    uint8_t lut : 2;
    uint8_t preloadImage : 1;  // set to 0 will draw image immediately
    uint8_t specialType : 5;
} __packed;