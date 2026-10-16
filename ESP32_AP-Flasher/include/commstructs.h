#ifndef NEWPROTO_H
#define NEWPROTO_H 

#include<Arduino.h>
#pragma pack(push, 1)

#include "../../oepl-definitions.h"
#include "../../oepl-proto.h"
#include "../../oepl-esp-ap-proto.h"

#define BLOCK_XFER_BUFFER_SIZE BLOCK_DATA_SIZE + sizeof(struct blockData)

#define PKT_XFER_TIMEOUT 0xED

#define PKT_APLIST_REQ 0x80
#define PKT_APLIST_REPLY 0x81
#define PKT_TAGINFO 0x82

// Version of the UDP messages between APs (multi-AP). S3s with different firmware must keep working
// together, and older ones ignore unknown packet types and cut longer payloads to their struct size.
// So when a UDP struct has to change: give the new layout a new packet type, keep sending the old
// type as long as a peer reports a lower version (UDPcomm::minPeerVersion()), and bump this number.
// Version 0 = firmware from before this field existed; the layouts of 0 and 1 are the same.
#define UDP_PROTO_VERSION 1

struct APlist {
    uint32_t src;
    char alias[32];
    uint8_t channelId;
    uint8_t tagCount;
    uint16_t version;
    uint8_t udpVersion;  // UDP_PROTO_VERSION of the sender; 0 when sent by older firmware (field missing)
} __packed;

#define SYNC_NOSYNC 0
#define SYNC_USERCFG 1
#define SYNC_TAGSTATUS 2
#define SYNC_DELETE 3
#define SYNC_VERSION 0xAA02

struct TagInfo {
    uint16_t structVersion = SYNC_VERSION;
    uint8_t mac[8];
    uint8_t syncMode;
    char alias[32];
    uint32_t lastseen;
    uint32_t nextupdate;
    uint16_t pendingCount;
    uint32_t expectedNextCheckin;
    uint16_t hwType;
    uint8_t wakeupReason;
    uint8_t capabilities;
    uint16_t pendingIdle;
    uint8_t contentMode;
    uint8_t md5[8];  // of the image on the tag, so other APs can show its preview
    uint8_t reserved[7];
} __packed;

#pragma pack(pop)

#endif // NEWPROTO_H