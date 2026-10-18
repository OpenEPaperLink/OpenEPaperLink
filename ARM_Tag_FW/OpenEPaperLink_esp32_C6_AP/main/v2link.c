#include "v2link.h"

#include <string.h>

#include "esp_log.h"
#include "second_uart.h"
#include "utils.h"

static const char *TAG = "V2";

#define V2_EVENT_MAX 120  // largest event payload: TAG_RETURN_DATA, 14 + the tag's 101 bytes
#define V2_EVQ_LEN 24

struct v2Event {
    uint8_t type;
    uint8_t len;
    uint8_t data[V2_EVENT_MAX];
};

static bool active = false;

// reception
static uint8_t rxCobs[V2_MAX_FRAME_COBS];
static size_t rxLen = 0;
static bool rxDiscard = false;  // frame too long: skip to the next delimiter
static uint32_t lastFrameMs = 0;

// duplicate detection: the last request handled and the reply sent for it
static int lastRxSeq = -1;
static uint8_t lastReplyWire[sizeof(struct v2Reply) + 8];
static size_t lastReplyLen = 0;

// events to the S3, stop-and-wait: queue[0] is on the line while evOutstanding is set
static struct v2Event queue[V2_EVQ_LEN];
static uint8_t queued = 0;
static bool evOutstanding = false;
static bool linkDown = false;  // retries used up; send again when the S3 shows life
static uint8_t txSeq = 0;
static uint8_t evRetries = 0;
static uint32_t evSentMs = 0;

uint16_t v2CrcErrors = 0;
uint16_t v2Overflows = 0;
uint16_t v2Retransmits = 0;
uint8_t v2EventsQueued(void) { return queued; }

static void sendFrame(uint8_t type, uint8_t seq, uint8_t flags, const void *payload, size_t len, uint8_t *keep, size_t *keepLen) {
    uint8_t wire[V2_MAX_FRAME_COBS + 1];
    const size_t n = v2BuildFrame(type, seq, flags, payload, len, wire);
    if (!n) return;
    uartWrite(wire, n);
    if (keep && n <= sizeof(lastReplyWire)) {
        memcpy(keep, wire, n);
        *keepLen = n;
    } else if (keep) {
        *keepLen = 0;
    }
}

void v2Start(void) {
    active = true;
    rxLen = 0;
    rxDiscard = false;
    lastRxSeq = -1;
    lastReplyLen = 0;
    evOutstanding = false;
    linkDown = false;
    evRetries = 0;
    txSeq = 0;
    lastFrameMs = getMillis();
}

void v2Stop(void) {
    active = false;
    queued = 0;  // the S3 resends everything after the next negotiation
    evOutstanding = false;
}

bool v2Active(void) { return active; }

uint32_t v2MsSinceLastFrame(void) { return getMillis() - lastFrameMs; }

static void sendHead(bool retransmit) {
    sendFrame(queue[0].type, txSeq, retransmit ? V2_FLAG_RETRANSMIT : 0, queue[0].data, queue[0].len, NULL, NULL);
    evOutstanding = true;
    evSentMs = getMillis();
}

static void popHead(void) {
    if (!queued) return;
    queued--;
    memmove(&queue[0], &queue[1], queued * sizeof(struct v2Event));
    evOutstanding = false;
    evRetries = 0;
    txSeq++;
}

static void handleFrame(const uint8_t *raw, size_t rawLen) {
    const struct v2Header *h = (const struct v2Header *)raw;
    const uint8_t *payload = raw + sizeof(struct v2Header);
    const size_t len = rawLen - sizeof(struct v2Header) - 2;

    lastFrameMs = getMillis();
    if (linkDown) {
        linkDown = false;
        evRetries = 0;
        evOutstanding = false;  // send the head again as a new attempt
    }

    if (h->flags & V2_FLAG_RESPONSE) {
        // the S3's answer to our event (ACK, or NAK when it could not use it: nothing to redo)
        if (evOutstanding && h->seq == txSeq && (h->type == V2_ACK || h->type == V2_NAK)) popHead();
        return;
    }

    if (h->type != V2_HELLO && (int)h->seq == lastRxSeq) {
        // a retransmission of what we handled last: the reply got lost
        if (lastReplyLen) uartWrite(lastReplyWire, lastReplyLen);
        return;
    }

    struct v2Reply reply = {.type = V2_ACK, .len = 0};
    v2HandleRequest(h->type, payload, len, &reply);
    lastRxSeq = h->seq;
    sendFrame(reply.type, h->seq, V2_FLAG_RESPONSE, reply.data, reply.len, lastReplyWire, &lastReplyLen);
}

void v2RxBytes(const uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        const uint8_t c = data[i];
        if (c != 0x00) {
            if (rxLen < sizeof(rxCobs)) {
                rxCobs[rxLen++] = c;
            } else if (!rxDiscard) {
                rxDiscard = true;
                v2Overflows++;
            }
            continue;
        }
        // delimiter: one frame complete
        if (!rxDiscard && rxLen) {
            uint8_t raw[V2_MAX_FRAME_RAW];
            const int n = v2CobsDecode(rxCobs, rxLen, raw, sizeof(raw));
            if (n >= 5 && v2CheckFrame(raw, n)) {
                handleFrame(raw, (size_t)n);
            } else {
                v2CrcErrors++;
            }
        }
        rxLen = 0;
        rxDiscard = false;
    }
}

void v2QueueEvent(uint8_t type, const void *payload, size_t len, bool urgent) {
    if (!active || len > V2_EVENT_MAX) return;
    if (queued == V2_EVQ_LEN) {
        // The S3 is not reading. Make room by dropping the oldest check-in that is not on the line;
        // transfer results and tag data are kept.
        uint8_t victim = 0xFF;
        for (uint8_t i = evOutstanding ? 1 : 0; i < queued; i++) {
            if (queue[i].type == V2_CHECKIN || queue[i].type == V2_BLOCK_SERVED) {
                victim = i;
                break;
            }
        }
        if (victim == 0xFF) {
            ESP_LOGW(TAG, "event queue full, event %02X dropped", type);
            return;
        }
        queued--;
        memmove(&queue[victim], &queue[victim + 1], (queued - victim) * sizeof(struct v2Event));
    }
    uint8_t pos = queued;
    if (urgent) {
        pos = evOutstanding ? 1 : 0;
        if (pos > queued) pos = queued;
        memmove(&queue[pos + 1], &queue[pos], (queued - pos) * sizeof(struct v2Event));
    }
    queue[pos].type = type;
    queue[pos].len = (uint8_t)len;
    memcpy(queue[pos].data, payload, len);
    queued++;
}

void v2Poll(void) {
    if (!active || !queued || linkDown) return;
    if (!evOutstanding) {
        sendHead(false);
        return;
    }
    if (getMillis() - evSentMs < V2_ACK_TIMEOUT_MS) return;
    if (evRetries < V2_RETRIES) {
        evRetries++;
        v2Retransmits++;
        sendHead(true);
    } else {
        // Keep the event and keep answering tags from local state; try again when the S3 talks
        ESP_LOGW(TAG, "no answer from the S3 to event %02X", queue[0].type);
        linkDown = true;
    }
}

uint32_t v2MsUntilWork(void) {
    if (!active || !queued || linkDown) return UINT32_MAX;
    if (!evOutstanding) return 0;
    const uint32_t since = getMillis() - evSentMs;
    return since >= V2_ACK_TIMEOUT_MS ? 0 : V2_ACK_TIMEOUT_MS - since;
}
