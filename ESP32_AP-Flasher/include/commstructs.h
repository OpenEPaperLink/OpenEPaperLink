#include <Arduino.h>
#pragma pack(push, 1)

#include "../../oepl-definitions.h"
#include "../../oepl-proto.h"
#include "../../oepl-esp-ap-proto.h"

#define BLOCK_XFER_BUFFER_SIZE BLOCK_DATA_SIZE + sizeof(struct blockData)

#define PKT_XFER_TIMEOUT 0xED

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


#pragma pack(pop)