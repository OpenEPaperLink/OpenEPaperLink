// S3 <-> radio protocol v2: message definitions (design, task #30).
// Normative description: radio-protocol-v2.md next to this file. Implementation (tasks #14, #29, #15)
// moves this header to where both the S3 and the radio firmware include it.
//
// On the wire every frame is COBS(header | payload | crc16) followed by 0x00. All integers
// little-endian, all structs packed, MACs in over-the-air byte order (as in proto.h / commstructs.h).
// The payload length follows from the frame; variable-length payloads end with a flexible array.

#pragma once

#include <stdint.h>

#define V2_PROTO_MAJOR 2
#define V2_PROTO_MINOR 0

#define V2_MAX_PAYLOAD 256      // bytes after the header, before the CRC
#define V2_MAX_FRAME_RAW (sizeof(struct v2Header) + V2_MAX_PAYLOAD + 2)
#define V2_CHUNK_MAX 240        // data bytes in one DATA_CHUNK
#define V2_BLOCK_SIZE 4096      // the tag's request unit; the radio caches per block
#define V2_ACK_TIMEOUT_MS 30
#define V2_RETRIES 3
#define V2_CHUNK_WINDOW 4
#define V2_HEARTBEAT_MS 1000

// --- frame header -------------------------------------------------------------------------------

#define V2_FLAG_RESPONSE 0x01    // ACK, NAK, HELLO_REPLY, STATUS
#define V2_FLAG_RETRANSMIT 0x02  // diagnostics only

struct v2Header {
    uint8_t type;
    uint8_t seq;    // per direction; a response carries the seq of what it answers
    uint8_t flags;
} __attribute__((packed));

// --- message types ------------------------------------------------------------------------------

enum v2Type {  // values for v2Header.type
    // S3 -> radio
    V2_HELLO = 0x01,
    V2_HEARTBEAT = 0x02,
    V2_SET_CONFIG = 0x03,
    V2_SET_PENDING = 0x04,
    V2_CANCEL_PENDING = 0x05,
    V2_DATA_CHUNK = 0x06,
    V2_DATA_DROP = 0x07,
    V2_GET_STATS = 0x08,
    // radio -> S3, events
    V2_CHECKIN = 0x40,
    V2_DATA_NEEDED = 0x41,
    V2_BLOCK_SERVED = 0x42,
    V2_XFER_COMPLETE = 0x43,
    V2_XFER_TIMEOUT = 0x44,
    V2_TAG_RETURN_DATA = 0x45,
    V2_LOG = 0x46,
    // responses, both directions
    V2_ACK = 0x7E,
    V2_NAK = 0x7F,
    // radio -> S3, typed replies
    V2_HELLO_REPLY = 0x81,
    V2_STATUS = 0x82,
};

enum v2NakCode {  // values for v2Nak.code
    V2_NAK_UNKNOWN_TYPE = 1,
    V2_NAK_BAD_LENGTH = 2,
    V2_NAK_INVALID_PARAM = 3,
    V2_NAK_NO_SLOT = 4,
    V2_NAK_NO_CACHE_SPACE = 5,
    V2_NAK_BUSY = 6,
};

// capability bits: caps in HELLO_REPLY, wanted_caps in HELLO, CAP> in the ASCII NFO? answer
#define V2_CAP_FRAMING (1 << 0)
#define V2_CAP_BLOCK_CACHE (1 << 1)
#define V2_CAP_SUBGHZ (1 << 2)
#define V2_CAP_TAG_RETURN_DATA (1 << 3)
#define V2_CAP_RESERVED_BAUD (1 << 4)  // reserved: 2 Mbaud only (decision 4)
#define V2_CAP_LOG (1 << 5)

#define V2_RADIO_24GHZ 0
#define V2_RADIO_SUBGHZ 1

// --- S3 -> radio --------------------------------------------------------------------------------

struct v2Hello {
    uint8_t protoMajor;
    uint8_t protoMinor;
    uint16_t wantedCaps;
} __attribute__((packed));

// HEARTBEAT and GET_STATS have no payload; both are answered with STATUS.

#define V2_CFG_UNCHANGED_U8 0xFF
#define V2_CFG_UNCHANGED_POWER 0x7F
#define V2_CFG_LOG_FRAMES 0x01

struct v2SetConfig {
    uint8_t channel;        // 11..26 (802.15.4); 0xFF = do not change
    uint8_t subghzChannel;  // 0 = off; 0xFF = do not change
    int8_t txPowerDbm;      // 0x7F = do not change (today: driver default, 20 dBm)
    uint8_t flags;          // V2_CFG_LOG_FRAMES
    uint8_t ledBrightness;  // activity LED: 0 = off, 255 = full (config.led, PR #587)
} __attribute__((packed));

#define V2_PENDING_DATA_FOLLOWS 0x01  // the S3 sends the data as DATA_CHUNKs right away

// One entry per tag MAC: SET_PENDING for a MAC that has one replaces it (as SDA> does today).
struct v2SetPending {
    uint8_t mac[8];
    uint64_t dataVer;
    uint32_t dataSize;
    uint8_t dataType;
    uint8_t dataTypeArg;
    uint16_t nextCheckin;  // as AvailDataInfo.nextCheckIn: minutes, bit 15 = seconds
    uint16_t ttlMin;       // minutes until the radio drops the entry and reports XFER_TIMEOUT (was attemptsLeft)
    uint8_t flags;         // V2_PENDING_DATA_FOLLOWS
} __attribute__((packed));

struct v2CancelPending {
    uint8_t mac[8];  // removes every entry for this MAC; no XFER_TIMEOUT
} __attribute__((packed));

struct v2DataChunk {
    uint64_t dataVer;
    uint32_t offset;     // within the file; a chunk never crosses a 4 KB block boundary
    uint32_t totalSize;  // file size, so the radio knows the size of the last block
    uint8_t bytes[];     // 1..V2_CHUNK_MAX
} __attribute__((packed));

struct v2DataDrop {
    uint64_t dataVer;  // remove all cached blocks of this data
} __attribute__((packed));

// --- radio -> S3 --------------------------------------------------------------------------------

struct v2HelloReply {
    uint8_t protoMajor;
    uint8_t protoMinor;
    uint16_t fwVersion;  // e.g. 0x0100
    uint8_t hwType;      // as TYP> today (0xC6 for the C6, …)
    uint8_t mac[8];
    uint16_t caps;
    uint16_t maxPayload;    // V2_MAX_PAYLOAD or smaller
    uint16_t cacheKb;       // 0 = no block cache
    uint16_t pendingSlots;  // size of the pending table
} __attribute__((packed));

struct v2Status {
    uint32_t uptimeMs;
    uint8_t channel;        // as set by SET_CONFIG (ZCH> today)
    uint8_t subghzChannel;  // SCH>
    int8_t txPowerDbm;      // ZPW>
    uint8_t lastResetReason;
    uint16_t pending;       // entries with data (PEN>)
    uint16_t pendingIdle;   // entries without data, idle requests (NOP>)
    uint16_t cacheUsedKb;
    uint32_t rxFrames;
    uint32_t txFrames;
    uint16_t lateRequestsSkipped;  // tag requests too old to answer, since the last STATUS
    uint16_t serialCrcErrors;
    uint16_t serialOverflows;
    uint16_t retransmits;
    uint8_t eventsQueued;
} __attribute__((packed));

#define V2_CHECKIN_FULL 0
#define V2_CHECKIN_SHORT 1

// One per check-in the radio answered. adr is the tag's AvailDataReq as received (zero for a short
// request); answered* is what the radio offered (dataType DATATYPE_NOUPDATE when it had nothing).
struct v2Checkin {
    uint8_t mac[8];
    uint32_t timestampUs;  // radio time of the request's start of frame (sub-GHz: when it was read)
    int8_t rssi;           // as measured by the radio
    uint8_t lqi;
    uint8_t radio;         // V2_RADIO_*
    uint8_t channel;
    uint16_t pan;
    uint8_t kind;  // V2_CHECKIN_*
    uint64_t answeredVer;
    uint8_t answeredType;
    uint8_t adr[21];  // struct AvailDataReq
} __attribute__((packed));

// The radio had no complete cached block for a block request. pleaseWaitMs is what it told the tag;
// the S3 answers with DATA_CHUNKs for [offset, offset + length).
struct v2DataNeeded {
    uint8_t mac[8];
    uint64_t dataVer;
    uint32_t offset;  // blockId * V2_BLOCK_SIZE
    uint32_t length;  // up to V2_BLOCK_SIZE, less for the last block
    uint16_t pleaseWaitMs;
} __attribute__((packed));

// Statistics, one per block request served.
struct v2BlockServed {
    uint8_t mac[8];
    uint64_t dataVer;
    uint8_t blockId;
    uint8_t partsRequested;
    uint8_t framesSent;
    int16_t lateMs;  // burst start minus the promised time
} __attribute__((packed));

struct v2XferEnd {  // XFER_COMPLETE (once per transfer) and XFER_TIMEOUT
    uint8_t mac[8];
    uint64_t dataVer;
} __attribute__((packed));

struct v2TagReturnData {
    uint8_t mac[8];
    uint32_t timestampUs;
    int8_t rssi;
    uint8_t lqi;
    uint8_t data[];  // the tag's tagReturnData as received (today at most 101 bytes)
} __attribute__((packed));

struct v2Log {
    uint8_t level;  // as esp_log_level_t
    char text[];    // not terminated
} __attribute__((packed));

struct v2Nak {
    uint8_t code;  // v2NakCode
} __attribute__((packed));

// --- size checks --------------------------------------------------------------------------------

#ifdef __cplusplus
#define V2_STATIC_ASSERT static_assert
#else
#define V2_STATIC_ASSERT _Static_assert
#endif

V2_STATIC_ASSERT(sizeof(struct v2Header) == 3, "header");
V2_STATIC_ASSERT(sizeof(struct v2Hello) == 4, "HELLO");
V2_STATIC_ASSERT(sizeof(struct v2SetConfig) == 5, "SET_CONFIG");
V2_STATIC_ASSERT(sizeof(struct v2SetPending) == 27, "SET_PENDING");
V2_STATIC_ASSERT(sizeof(struct v2CancelPending) == 8, "CANCEL_PENDING");
V2_STATIC_ASSERT(sizeof(struct v2DataChunk) + V2_CHUNK_MAX <= V2_MAX_PAYLOAD, "DATA_CHUNK");
V2_STATIC_ASSERT(sizeof(struct v2DataChunk) == 16, "DATA_CHUNK header");
V2_STATIC_ASSERT(sizeof(struct v2DataDrop) == 8, "DATA_DROP");
V2_STATIC_ASSERT(sizeof(struct v2HelloReply) == 21, "HELLO_REPLY");
V2_STATIC_ASSERT(sizeof(struct v2Status) == 31, "STATUS");
V2_STATIC_ASSERT(sizeof(struct v2Checkin) == 49, "CHECKIN");
V2_STATIC_ASSERT(sizeof(struct v2DataNeeded) == 26, "DATA_NEEDED");
V2_STATIC_ASSERT(sizeof(struct v2BlockServed) == 21, "BLOCK_SERVED");
V2_STATIC_ASSERT(sizeof(struct v2XferEnd) == 16, "XFER_COMPLETE / XFER_TIMEOUT");
V2_STATIC_ASSERT(sizeof(struct v2TagReturnData) + 101 <= V2_MAX_PAYLOAD, "TAG_RETURN_DATA");
