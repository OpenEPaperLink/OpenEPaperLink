#include <Arduino.h>
#pragma pack(push, 1)

#include "../../tag_types.h"

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

#define EPD_LUT_DEFAULT 0
#define EPD_LUT_NO_REPEATS 1
#define EPD_LUT_FAST_NO_REDS 2
#define EPD_LUT_FAST 3
#define EPD_LUT_OTA 0x10

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

#define PKT_AVAIL_DATA_REQ 0xE5
#define PKT_AVAIL_DATA_INFO 0xE6
#define PKT_XFER_COMPLETE 0xEA
#define PKT_XFER_TIMEOUT 0xED
#define PKT_CANCEL_XFER 0xEC
#define PKT_APLIST_REQ 0x80
#define PKT_APLIST_REPLY 0x81
#define PKT_TAGINFO 0x82

struct APlist {
    uint32_t src;
    char alias[32];
    uint8_t channelId;
    uint8_t tagCount;
    uint16_t version;
} __packed;

#define SYNC_NOSYNC 0
#define SYNC_USERCFG 1
#define SYNC_TAGSTATUS 2
#define SYNC_DELETE 3
#define SYNC_VERSION 0xAA00

struct TagInfo {
    uint16_t structVersion = SYNC_VERSION;
    uint8_t mac[8];
    uint8_t syncMode;
    char alias[32];
    uint32_t lastseen;
    uint32_t nextupdate;
    bool pending;
    uint32_t expectedNextCheckin;
    uint8_t hwType;
    uint8_t wakeupReason;
    uint8_t capabilities;
    uint16_t pendingIdle;
    uint8_t contentMode;
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

#pragma pack(pop)