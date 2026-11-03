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
#elif defined(__GNUC__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 6))
// GCC before 4.6 (the TLSR's tc32-elf-gcc is 4.5) has no _Static_assert: a negative array size fails instead
#define V2_SA_CAT2(a, b) a##b
#define V2_SA_CAT(a, b) V2_SA_CAT2(a, b)
#define V2_STATIC_ASSERT(cond, msg) typedef char V2_SA_CAT(v2StaticAssert, __LINE__)[(cond) ? 1 : -1]
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

// CRC-16/CCITT-FALSE: poly 0x1021, init 0xFFFF, no reflection, no final xor.
// By table: one lookup per byte instead of eight shift/xor steps, with the same result. The bitwise
// loop cost 117 instructions a byte on the TLSR (GCC 4.5 parked the polynomial in a high register),
// 12 ms for a 4 KB block; the table costs 512 bytes of flash in each file that calls v2Crc16(), one
// per firmware today.
static const uint16_t v2Crc16Tab[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0,
};

static inline uint16_t v2Crc16(const uint8_t* data, size_t len) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; i++) crc = (uint16_t)((crc << 8) ^ v2Crc16Tab[(uint8_t)((crc >> 8) ^ data[i])]);
    return crc;
}

// COBS-encodes len bytes from src into dst (room for len + len / 254 + 1 bytes); returns the encoded
// length. The 0x00 delimiter is not included.
static inline size_t v2CobsEncode(const uint8_t* src, size_t len, uint8_t* dst) {
    size_t out = 1, codePos = 0;
    unsigned code = 1;  // not uint8_t: GCC 4.5 then truncates inside the loop, two instructions a byte
    for (size_t i = 0; i < len; i++) {
        if (src[i] == 0) {
            dst[codePos] = (uint8_t)code;
            codePos = out++;
            code = 1;
        } else {
            dst[out++] = src[i];
            if (++code == 0xFF) {
                dst[codePos] = (uint8_t)code;
                codePos = out++;
                code = 1;
            }
        }
    }
    dst[codePos] = (uint8_t)code;
    return out;
}

// Decodes a COBS frame (without its 0x00 delimiter) into dst; returns the decoded length, or -1 if
// the frame is malformed or does not fit in dstMax.
//
// The code byte says how long the run is, so both bounds are checked once per run; only the zero
// check stays per byte. Pointers and unsigned counters keep the ends in registers: 9 instructions a
// byte on the TLSR, against 22 for the plain version with indices and a uint8_t counter, which GCC
// 4.5 reloaded and truncated every pass. It rejects exactly the frames the plain version did: a zero
// inside a run, and a run past the end of the frame.
//
// dst may be src. A run's code byte is read before anything of the run is written, and the write
// pointer never passes the read pointer, so decoding in place reads every byte before it is
// overwritten. The TLSR AP decodes in place and does without a second frame buffer.
static inline int v2CobsDecode(const uint8_t* src, size_t len, uint8_t* dst, size_t dstMax) {
    const uint8_t* s = src;
    const uint8_t* const sEnd = src + len;
    uint8_t* d = dst;
    size_t dstFree = dstMax;  // room left in dst
    while (s < sEnd) {
        const unsigned code = *s++;
        if (code == 0) return -1;
        const unsigned run = code - 1u;          // data bytes carried by this run
        const size_t left = (size_t)(sEnd - s);  // bytes of frame after the code byte
        if (run > left) return -1;               // the run runs off the end of the frame
        const unsigned zero = (code != 0xFF && run < left) ? 1u : 0u;
        if (run + zero > dstFree) return -1;
        dstFree -= run + zero;
        for (unsigned i = run; i != 0; i--) {
            const uint8_t b = *s++;
            if (b == 0) return -1;  // a data byte of a run is never zero in a COBS frame
            *d++ = b;
        }
        if (zero) *d++ = 0;
    }
    return (int)(size_t)(d - dst);
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
