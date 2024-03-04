#ifndef __packed
#define __packed __attribute__((packed))
#endif

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
#ifdef HAS_SUBGHZ
    uint8_t subghzchannel;
#endif
} __packed;

struct espAvailDataReq {
    uint8_t checksum;
    uint8_t src[8];
    struct AvailDataReq adr;
} __packed;

struct espTagReturnData {
    uint8_t checksum;
    uint8_t src[8];
    uint8_t len;
    struct tagReturnData returnData;
} __packed;

