// S3 <-> radio protocol v2: message definitions and framing, shared by the ESP32 AP firmware
// (ESP32_AP-Flasher) and the ESP32-C6/H2 radio firmware (ARM_Tag_FW/OpenEPaperLink_esp32_C6_AP).
//
// On the wire every frame is COBS(header | payload | crc16) followed by 0x00. All integers
// little-endian, all structs packed, MACs in over-the-air byte order (as in oepl-proto.h).
// The payload length follows from the frame; variable-length payloads end with a flexible array.
//
// Startup: the S3 talks the old ASCII protocol at 115200 baud first (RDY?, NFO?). A v2 radio adds
// CAP>xxxx to its NFO? answer; the S3 then sends FRM1, both wait 20 ms after the radio's ACK> and
// continue at 2 Mbaud with frames, starting with HELLO.
//
// Every frame that is not a response gets exactly one response (ACK, NAK, HELLO_REPLY or STATUS)
// with the same seq. The sender resends an unanswered frame after V2_ACK_TIMEOUT_MS with the same
// seq and V2_FLAG_RETRANSMIT, at most V2_RETRIES times. The receiver handles a seq only once and
// answers a repeated one with the response it sent before.

#pragma once

#include <stddef.h>
#include <stdint.h>

#define V2_PROTO_MAJOR 2
#define V2_PROTO_MINOR 1

#define V2_BAUD 2000000
#define V2_MAX_PAYLOAD 256  // bytes after the header, before the CRC
#define V2_MAX_FRAME_RAW (3 + V2_MAX_PAYLOAD + 2)
#define V2_MAX_FRAME_COBS (V2_MAX_FRAME_RAW + V2_MAX_FRAME_RAW / 254 + 1)
#define V2_CHUNK_MAX 240     // data bytes in one DATA_CHUNK
#define V2_BLOCK_SIZE 4096   // the tag's request unit
#define V2_ACK_TIMEOUT_MS 30
#define V2_RETRIES 3
#define V2_CHUNK_WINDOW 4
#define V2_HEARTBEAT_MS 1000
#define V2_SWITCH_DELAY_MS 20  // after the ACK> to FRM1, before both sides change baud rate

// --- frame header -------------------------------------------------------------------------------

#define V2_FLAG_RESPONSE 0x01    // ACK, NAK, HELLO_REPLY, STATUS
#define V2_FLAG_RETRANSMIT 0x02  // diagnostics only

struct v2Header {
    uint8_t type;
    uint8_t seq;  // per direction; a response carries the seq of what it answers
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
    V2_ALLOW_CLEAR = 0x09,
    V2_ALLOW_ADD = 0x0A,
    V2_ALLOW_REMOVE = 0x0B,
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

// capability bits: caps in HELLO_REPLY, wantedCaps in HELLO, CAP> in the ASCII NFO? answer
#define V2_CAP_FRAMING (1 << 0)
#define V2_CAP_BLOCK_CACHE (1 << 1)
#define V2_CAP_SUBGHZ (1 << 2)
#define V2_CAP_TAG_RETURN_DATA (1 << 3)
#define V2_CAP_RESERVED_BAUD (1 << 4)  // reserved: 2 Mbaud only
#define V2_CAP_LOG (1 << 5)
#define V2_CAP_ALLOW_LIST (1 << 6)  // ALLOW_* and the lock bits in SET_CONFIG

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
// Bits 1-2: the tag inventory lock (the AP's "Lock tag inventory" setting). Locked, the radio answers
// only the tags on its allow-list (ALLOW_*); learning, also a full check-in of a tag that just booted.
// Frames from other tags get no reply and no event.
#define V2_CFG_LOCK_MASK 0x06
#define V2_CFG_LOCK_SHIFT 1
#define V2_LOCK_OFF 0
#define V2_LOCK_LOCKED 1
#define V2_LOCK_LEARNING 2

struct v2SetConfig {
    uint8_t channel;        // 11..26 (802.15.4); 0xFF = do not change
    uint8_t subghzChannel;  // 0xFF = do not change
    int8_t txPowerDbm;      // 0x7F = do not change
    uint8_t flags;          // V2_CFG_LOG_FRAMES, V2_CFG_LOCK_MASK
    uint8_t ledBrightness;  // activity LED: 0 = off, 255 = full
} __attribute__((packed));

#define V2_PENDING_DATA_FOLLOWS 0x01  // the S3 sends the data as DATA_CHUNKs right away

// One entry per tag MAC: SET_PENDING for a MAC that has one replaces it.
struct v2SetPending {
    uint8_t mac[8];
    uint64_t dataVer;
    uint32_t dataSize;
    uint8_t dataType;
    uint8_t dataTypeArg;
    uint16_t nextCheckin;  // as AvailDataInfo.nextCheckIn: minutes, bit 15 = seconds
    uint16_t ttlMin;       // minutes until the radio drops the entry and reports XFER_TIMEOUT
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

// ALLOW_CLEAR has no payload. ALLOW_ADD and ALLOW_REMOVE carry 1..V2_ALLOW_MAX_MACS MACs of 8 bytes.
// Adding a MAC that is on the list, or removing one that is not, is no error. When the list is full,
// ALLOW_ADD adds what fits and is answered with NAK NO_SLOT.
#define V2_ALLOW_MAX_MACS (V2_CHUNK_MAX / 8)

// --- radio -> S3 --------------------------------------------------------------------------------

struct v2HelloReply {
    uint8_t protoMajor;
    uint8_t protoMinor;
    uint16_t fwVersion;
    uint8_t hwType;  // as TYP> in the ASCII protocol (0xC6 for the C6 and H2)
    uint8_t mac[8];
    uint16_t caps;
    uint16_t maxPayload;    // V2_MAX_PAYLOAD or smaller
    uint16_t cacheKb;       // 0 = no block cache
    uint16_t pendingSlots;  // size of the pending table
} __attribute__((packed));

#define V2_NO_SUBGHZ_CHANNEL 0xFF

struct v2Status {
    uint32_t uptimeMs;
    uint8_t channel;
    uint8_t subghzChannel;  // V2_NO_SUBGHZ_CHANNEL without a sub-GHz radio
    int8_t txPowerDbm;
    uint8_t lastResetReason;  // esp_reset_reason_t on the C6/H2
    uint16_t pending;         // entries with data
    uint16_t pendingIdle;     // entries without data, idle requests
    uint16_t cacheUsedKb;
    uint32_t rxFrames;  // radio frames received
    uint32_t txFrames;  // radio frames sent
    uint16_t lateRequestsSkipped;  // tag requests too old to answer, since the last STATUS
    uint16_t serialCrcErrors;
    uint16_t serialOverflows;
    uint16_t retransmits;
    uint8_t eventsQueued;
} __attribute__((packed));

#define V2_CHECKIN_FULL 0
#define V2_CHECKIN_SHORT 1

// One per check-in the radio answered. adr is the tag's AvailDataReq as received (zero for a short
// request); answered* is what the radio offered (dataType 0 when it had nothing).
struct v2Checkin {
    uint8_t mac[8];
    uint32_t timestampUs;  // radio time of the request's start of frame
    int8_t rssi;           // as measured by the radio
    uint8_t lqi;
    uint8_t radio;  // V2_RADIO_*
    uint8_t channel;
    uint16_t pan;
    uint8_t kind;  // V2_CHECKIN_*
    uint64_t answeredVer;
    uint8_t answeredType;
    uint8_t adr[21];  // struct AvailDataReq
} __attribute__((packed));

// The radio has no complete block for a block request. pleaseWaitMs is what it told the tag; the S3
// answers with DATA_CHUNKs for [offset, offset + length).
struct v2DataNeeded {
    uint8_t mac[8];
    uint64_t dataVer;
    uint32_t offset;  // blockId * V2_BLOCK_SIZE
    uint32_t length;  // up to V2_BLOCK_SIZE, less for the last block
    uint16_t pleaseWaitMs;
} __attribute__((packed));

// Statistics, one per block burst.
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
    uint8_t data[];  // the tag's tagReturnData as received
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

// --- framing ------------------------------------------------------------------------------------

// CRC-16/CCITT-FALSE: poly 0x1021, init 0xFFFF, no reflection, no final xor
static inline uint16_t v2Crc16(const uint8_t* data, size_t len) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (uint8_t b = 0; b < 8; b++) crc = (crc & 0x8000) ? (uint16_t)((crc << 1) ^ 0x1021) : (uint16_t)(crc << 1);
    }
    return crc;
}

// COBS-encodes len bytes from src into dst (room for len + len / 254 + 1 bytes); returns the encoded
// length. The 0x00 delimiter is not included.
static inline size_t v2CobsEncode(const uint8_t* src, size_t len, uint8_t* dst) {
    size_t out = 1, codePos = 0;
    uint8_t code = 1;
    for (size_t i = 0; i < len; i++) {
        if (src[i] == 0) {
            dst[codePos] = code;
            codePos = out++;
            code = 1;
        } else {
            dst[out++] = src[i];
            if (++code == 0xFF) {
                dst[codePos] = code;
                codePos = out++;
                code = 1;
            }
        }
    }
    dst[codePos] = code;
    return out;
}

// Decodes a COBS frame (without its 0x00 delimiter) into dst; returns the decoded length, or -1 if
// the frame is malformed or does not fit in dstMax.
static inline int v2CobsDecode(const uint8_t* src, size_t len, uint8_t* dst, size_t dstMax) {
    size_t in = 0, out = 0;
    while (in < len) {
        const uint8_t code = src[in++];
        if (code == 0) return -1;
        for (uint8_t i = 1; i < code; i++) {
            if (in >= len || out >= dstMax || src[in] == 0) return -1;
            dst[out++] = src[in++];
        }
        if (code != 0xFF && in < len) {
            if (out >= dstMax) return -1;
            dst[out++] = 0;
        }
    }
    return (int)out;
}

// Builds a complete wire frame (COBS + 0x00) in wire (room for V2_MAX_FRAME_COBS + 1 bytes);
// returns its length, or 0 if the payload is too long.
static inline size_t v2BuildFrame(uint8_t type, uint8_t seq, uint8_t flags, const void* payload, size_t len, uint8_t* wire) {
    uint8_t raw[V2_MAX_FRAME_RAW];
    if (len > V2_MAX_PAYLOAD) return 0;
    raw[0] = type;
    raw[1] = seq;
    raw[2] = flags;
    for (size_t i = 0; i < len; i++) raw[3 + i] = ((const uint8_t*)payload)[i];
    const uint16_t crc = v2Crc16(raw, 3 + len);
    raw[3 + len] = (uint8_t)(crc & 0xFF);
    raw[4 + len] = (uint8_t)(crc >> 8);
    const size_t n = v2CobsEncode(raw, len + 5, wire);
    wire[n] = 0x00;
    return n + 1;
}

// Checks a decoded frame (header | payload | crc16); true if the CRC matches. The payload length
// is rawLen - 5.
static inline int v2CheckFrame(const uint8_t* raw, int rawLen) {
    if (rawLen < 5) return 0;
    const uint16_t crc = v2Crc16(raw, (size_t)rawLen - 2);
    return raw[rawLen - 2] == (uint8_t)(crc & 0xFF) && raw[rawLen - 1] == (uint8_t)(crc >> 8);
}
