#include "v2link.h"

#include <string.h>

#include "hal.h"

// --- what waits for the S3: one byte ring -------------------------------------------------------
// Events and LOG lines wait in one buffer of variable-length records, each costing what it carries:
// a CHECKIN 51 bytes, a transfer result 18, a tag report up to 117, a LOG line its length plus two.
//
//   0 | event | event | ... evHi -->      free      <-- logLo ... | LOG | LOG | V2_OUTQ_BYTES
//     ^ evLo, the oldest event (on the line while evOutstanding)   the oldest LOG line ends at logHi ^
//
// Events grow up from the bottom and LOG lines down from the top. They are separate queues because
// they leave separately: an event goes out at once and waits for its ACK, a LOG line only while no
// event waits and V2_LOG_GAP_MS after the last one. Each queue is consumed from its front by moving
// an index, not by memmove, which is slow on the TLSR; the free space is moved back to the middle
// (outqCompact) only when a record does not fit there but would fit in the ring.
//
// Event records, oldest at evLo: [type][len][payload: len bytes].
// LOG records, oldest ending at logHi: [level][text][len], len = 1 + the text's length, the payload of
// the LOG frame - the length last, because the oldest line is found from the top, from its end.
//
// 1024 bytes: the S3 can leave the ring unread for at most V2_LINK_LOST_MS (5 s) before the link goes
// back to ASCII and v2Stop() empties it. Transfer results, DATA_NEEDED and tag reports must survive
// that and are bounded by the transfers in progress; check-ins are what fill the ring, and 1024 holds
// 20 of them, four a second for the whole stall. When it is full, LOG lines and then check-ins are
// dropped and counted (v2LogsDropped, v2EventsDropped), never a transfer result.
#define V2_OUTQ_BYTES 1024
#define V2_EVENT_MAX 255  // the record's length byte; the largest event ap.c sends is 115 (a tag report)
static uint8_t outq[V2_OUTQ_BYTES];
static uint16_t evLo = 0, evHi = 0;                            // events: [evLo, evHi)
static uint16_t logLo = V2_OUTQ_BYTES, logHi = V2_OUTQ_BYTES;  // LOG lines: [logLo, logHi)
V2_STATIC_ASSERT(V2_OUTQ_BYTES <= 0xFFFF, "the ring's indices are 16 bits");
V2_STATIC_ASSERT(sizeof(struct v2Checkin) + 2 <= 51, "a CHECKIN costs 51 bytes, as the sizing above says");
V2_STATIC_ASSERT(sizeof(struct v2TagReturnData) + 101 <= V2_EVENT_MAX, "TAG_RETURN_DATA fits a record");
V2_STATIC_ASSERT(V2_EVENT_MAX <= V2_MAX_PAYLOAD, "an event fits a frame");
// Every frame is encoded straight into the UART's transmit buffer (sendFrame), which holds one
V2_STATIC_ASSERT(V2_MAX_FRAME_COBS + 1 <= HAL_SERIAL_TX_MAX, "a frame and its delimiter fit the TX buffer");

// AP_DEBUG: the one switch that compiles the instruments in (see src/ap.c). Here it means LOG frames
// whatever SET_CONFIG says, since the instruments write their lines through v2Log() and the S3 may
// not have asked for them.
#ifndef AP_DEBUG
#define AP_DEBUG 0
#endif

static bool active = false;
static bool logEnabled = AP_DEBUG;
static uint16_t logCount = 0;  // a line can be two bytes, so more than 255 fit
// Time between LOG frames, so that the S3's USB console, which drops bytes when several lines arrive
// within a few milliseconds, stays roughly readable. Its web log needs no gap, and a longer one costs
// lines when they come fast enough to fill the ring.
#define V2_LOG_GAP_MS 60
static uint32_t lastLogMs;
static bool logSentOnce = false;

// reception; a complete frame is decoded in place, so this is also the received frame (v2RxBytes)
static uint8_t rxCobs[V2_MAX_FRAME_COBS];
V2_STATIC_ASSERT(V2_MAX_FRAME_COBS >= V2_MAX_FRAME_RAW, "a decoded frame fits where it was received");
static size_t rxLen = 0;
static bool rxDiscard = false;  // frame too long: skip to the next delimiter
static uint32_t lastFrameMs = 0;
static uint32_t bytesSinceFrame = 0;

// Duplicate detection. The S3 keeps up to V2_CHUNK_WINDOW requests unanswered (sendChunks() in
// ESP32_AP-Flasher/src/v2transport.cpp), so a retransmission need not carry the newest sequence
// number, and handling it twice can do harm (a SET_PENDING would reset attemptsLeft). So the sequence
// numbers last handled are kept in a ring as deep as that window, with how each was answered;
// membership is the whole test, so the 8-bit wrap does not matter. Only a DATA_CHUNK can fall out of
// the ring unanswered (control messages are stop-and-wait, docs/design/radio-protocol-v2.md), and
// cacheChunk() is idempotent.
#define V2_DUP_WINDOW V2_CHUNK_WINDOW
static uint8_t dupSeq[V2_DUP_WINDOW];
// how that request was answered: 0 an ACK, 1..0xFE the code of a NAK, 0xFF a reply we cannot rebuild
static uint8_t dupAnswer[V2_DUP_WINDOW];
#define DUP_ACK 0
#define DUP_LONG 0xFF
static uint8_t dupFilled = 0;  // slots in use, 0..V2_DUP_WINDOW
static uint8_t dupNext = 0;    // where the next request handled goes
// the reply to the newest request, byte for byte, for the common case of a lost ack
static uint8_t lastReplyWire[sizeof(struct v2Reply) + 8];
static uint8_t lastReplyLen = 0;

// Events to the S3, stop-and-wait: the event at the front (evLo) is on the line while evOutstanding
// is set.
static uint8_t queued = 0;  // events in the ring; 1024 / 18, the smallest record, is 56
static bool evOutstanding = false;
static bool linkDown = false;  // retries used up; send again when the S3 shows life
static uint8_t txSeq = 0;
static uint8_t evRetries = 0;
static uint32_t evSentMs = 0;

uint16_t v2CrcErrors = 0;
uint16_t v2Overflows = 0;
uint16_t v2Retransmits = 0;
uint16_t v2EventsDropped = 0;
uint8_t v2EventsQueued(void) { return queued; }
static uint8_t evHigh = 0;
static uint16_t logHigh = 0, outHigh = 0;
static uint16_t logsDropped = 0;
// The slowest request since boot, from receipt to the reply being written, and its type
static uint32_t reqMaxUs = 0;
static uint8_t reqMaxType = 0;
uint32_t v2ReqMaxUs(void) { return reqMaxUs; }
uint8_t v2ReqMaxType(void) { return reqMaxType; }
uint8_t v2EventsHigh(void) { return evHigh; }
uint16_t v2OutHigh(void) { return outHigh; }
uint16_t v2LogHigh(void) { return logHigh; }
uint16_t v2LogsDropped(void) { return logsDropped; }

// --- the ring -----------------------------------------------------------------------------------
static unsigned outqUsed(void) { return (unsigned)(evHi - evLo) + (unsigned)(logHi - logLo); }
static unsigned outqFree(void) { return V2_OUTQ_BYTES - outqUsed(); }
static unsigned evSize(unsigned at) { return 2u + outq[at + 1]; }

// Puts all the free space between the two queues, where both of them grow
static void outqCompact(void) {
    const unsigned ev = (unsigned)(evHi - evLo), lg = (unsigned)(logHi - logLo);
    if (evLo) {
        memmove(outq, outq + evLo, ev);
        evLo = 0;
        evHi = (uint16_t)ev;
    }
    if (logHi != V2_OUTQ_BYTES) {
        memmove(outq + V2_OUTQ_BYTES - lg, outq + logLo, lg);
        logLo = (uint16_t)(V2_OUTQ_BYTES - lg);
        logHi = V2_OUTQ_BYTES;
    }
}

static void outqMark(void) {
    const unsigned used = outqUsed();
    if (used > outHigh) outHigh = (uint16_t)used;
}

// The oldest LOG line leaves the ring: sent, or dropped for room
static void logPop(void) {
    logHi = (uint16_t)(logHi - 1u - outq[logHi - 1]);
    if (!--logCount) logLo = logHi = V2_OUTQ_BYTES;  // empty: back to the top, so compaction stays rare
}

// An event leaves the ring from the middle (dropped for room); the ones after it move down over it
static void evRemove(unsigned at) {
    const unsigned size = evSize(at);
    memmove(outq + at, outq + at + size, evHi - at - size);
    evHi = (uint16_t)(evHi - size);
    if (!--queued) evLo = evHi = 0;
}

// The oldest event the drop policy may give up: a CHECKIN or a BLOCK_SERVED that is not on the line.
// Transfer results, DATA_NEEDED and tag reports are never chosen. Returns its offset, or 0xFFFF.
static unsigned evVictim(void) {
    unsigned at = evLo;
    if (evOutstanding && at < evHi) at += evSize(at);
    for (; at < evHi; at += evSize(at)) {
        if (outq[at] == V2_CHECKIN || outq[at] == V2_BLOCK_SERVED) return at;
    }
    return 0xFFFF;
}

// What the drop policy could free for an event: every LOG line, and every event evVictim() could pick
static unsigned outqReclaimable(void) {
    unsigned n = (unsigned)(logHi - logLo);
    unsigned at = evLo;
    if (evOutstanding && at < evHi) at += evSize(at);
    for (; at < evHi; at += evSize(at)) {
        if (outq[at] == V2_CHECKIN || outq[at] == V2_BLOCK_SERVED) n += evSize(at);
    }
    return n;
}

// --- frames to the S3 ---------------------------------------------------------------------------
// The frame is COBS-encoded straight into the UART's transmit buffer (halSerialTxBuf), with the CRC
// computed on the way, so no frame is ever built in a buffer of its own. A run's code byte is its
// length, known only at its end, so the encoder keeps a hole for it and fills it at a zero, after 254
// non-zero bytes (code 0xFF), and at the frame's end. The output equals v2CobsEncode()'s byte for
// byte, which the host tests check.
static size_t encodeFrame(uint8_t *dst, uint8_t type, uint8_t seq, uint8_t flags, const uint8_t *payload,
                          size_t len) {
    const uint8_t head[3] = {type, seq, flags};
    uint8_t tail[2];
    uint16_t crc = 0xFFFF;
    // not uint8_t: GCC 4.5 truncates inside the loop otherwise, as in v2CobsEncode()
    unsigned out = 1, codePos = 0, code = 1;
    const uint8_t *src = head;
    size_t n = sizeof(head);
    for (unsigned part = 0;; part++) {
        for (size_t i = 0; i < n; i++) {
            const uint8_t b = src[i];
            // v2Crc16(), a byte at a time. It also runs over the two CRC bytes, where it is not used:
            // cheaper than a test in every pass.
            crc = (uint16_t)((crc << 8) ^ v2Crc16Tab[(uint8_t)((crc >> 8) ^ b)]);
            if (b == 0) {
                dst[codePos] = (uint8_t)code;
                codePos = out++;
                code = 1;
            } else {
                dst[out++] = b;
                if (++code == 0xFF) {
                    dst[codePos] = (uint8_t)code;
                    codePos = out++;
                    code = 1;
                }
            }
        }
        if (part == 0) {
            src = payload;
            n = len;
        } else if (part == 1) {
            tail[0] = (uint8_t)(crc & 0xFF);  // header and payload: the frame's CRC, low byte first
            tail[1] = (uint8_t)(crc >> 8);
            src = tail;
            n = sizeof(tail);
        } else {
            break;
        }
    }
    dst[codePos] = (uint8_t)code;
    dst[out++] = 0x00;
    return out;
}

// Encodes the frame into the UART's buffer and sends it; returns its length on the wire (0 if too
// long). keep: it is the reply to a request, kept byte for byte to resend if its ack is lost.
static size_t sendFrame(uint8_t type, uint8_t seq, uint8_t flags, const void *payload, size_t len, bool keep) {
    if (keep) lastReplyLen = 0;
    if (len > V2_MAX_PAYLOAD) return 0;
    uint8_t *const wire = halSerialTxBuf();
    const size_t n = encodeFrame(wire, type, seq, flags, (const uint8_t *)payload, len);
    if (keep && n <= sizeof(lastReplyWire)) {
        memcpy(lastReplyWire, wire, n);  // before the commit, after which the buffer is the UART's
        lastReplyLen = (uint8_t)n;
    }
    halSerialTxCommit(n);
    return n;
}

void v2Start(void) {
    active = true;
    rxLen = 0;
    rxDiscard = false;
    dupFilled = 0;
    dupNext = 0;
    lastReplyLen = 0;
    queued = 0;  // v2Stop() left it empty, and nothing queues while the link is not active
    evLo = evHi = 0;
    evOutstanding = false;
    linkDown = false;
    evRetries = 0;
    txSeq = 0;
    logEnabled = AP_DEBUG;
    logCount = 0;
    logLo = logHi = V2_OUTQ_BYTES;
    logSentOnce = false;
    lastFrameMs = halMillis();
    bytesSinceFrame = 0;
}

void v2Stop(void) {
    active = false;
    queued = 0;  // the S3 resends everything after the next negotiation
    evLo = evHi = 0;
    logCount = 0;  // nothing sends them while the link is down, and v2Start() would clear them
    logLo = logHi = V2_OUTQ_BYTES;
    evOutstanding = false;
}

bool v2Active(void) { return active; }

uint32_t v2MsSinceLastFrame(void) { return halMillis() - lastFrameMs; }

static void sendHead(bool retransmit) {
    sendFrame(outq[evLo], txSeq, retransmit ? V2_FLAG_RETRANSMIT : 0, outq + evLo + 2, outq[evLo + 1], false);
    evOutstanding = true;
    evSentMs = halMillis();
}

static void popHead(void) {
    if (!queued) return;
    evLo = (uint16_t)(evLo + evSize(evLo));
    if (!--queued) evLo = evHi = 0;  // an empty queue starts at the bottom again, so compaction stays rare
    evOutstanding = false;
    evRetries = 0;
    txSeq++;
}

// Answers a request we have already handled, without handling it again; false if it is new to us
static bool answerDuplicate(uint8_t seq) {
    uint8_t slot = 0;
    while (slot < dupFilled && dupSeq[slot] != seq) slot++;
    if (slot == dupFilled) return false;

    if (seq == dupSeq[(dupNext + V2_DUP_WINDOW - 1) % V2_DUP_WINDOW]) {
        // The request we handled last: our reply got lost, so send the same bytes again. This is what
        // keeps the long replies honest -- a STATUS carries counters that cannot be rebuilt.
        if (lastReplyLen) halSerialWrite(lastReplyWire, lastReplyLen);
        return true;
    }
    if (dupAnswer[slot] != DUP_LONG) {
        // An older one, still in the S3's window. Answer it the way we answered it the first time;
        // applying it again is the point of all this. A refusal is repeated as a refusal, so a chunk
        // the cache had no room for is not now reported as accepted.
        const bool isNak = dupAnswer[slot] != DUP_ACK;
        sendFrame(isNak ? V2_NAK : V2_ACK, seq, V2_FLAG_RESPONSE, &dupAnswer[slot], isNak ? 1 : 0, false);
        return true;
    }
    // A STATUS or HELLO_REPLY, asked for only by HEARTBEAT, GET_STATS and HELLO, which the S3 sends
    // one at a time. Should one ever reach here it is a read, so answering it afresh is safe.
    return false;
}

static void handleFrame(const uint8_t *raw, size_t rawLen) {
    const struct v2Header *h = (const struct v2Header *)raw;
    const uint8_t *payload = raw + sizeof(struct v2Header);
    const size_t len = rawLen - sizeof(struct v2Header) - 2;

    lastFrameMs = halMillis();
    bytesSinceFrame = 0;
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

    // HELLO restarts the link and the S3's sequence numbers with it: it is never a duplicate, and the
    // requests handled before it say nothing about the ones that follow
    if (h->type == V2_HELLO) {
        dupFilled = 0;
        dupNext = 0;
        lastReplyLen = 0;
    } else if (answerDuplicate(h->seq)) {
        return;
    }

    struct v2Reply reply;
    reply.type = V2_ACK;
    reply.len = 0;
    const uint32_t t0 = halMicros();
    v2HandleRequest(h->type, payload, len, &reply);
    dupSeq[dupNext] = h->seq;
    if (reply.type == V2_ACK) {
        dupAnswer[dupNext] = DUP_ACK;
    } else if (reply.type == V2_NAK && reply.len && reply.data[0] != DUP_LONG) {
        dupAnswer[dupNext] = reply.data[0];
    } else {
        dupAnswer[dupNext] = DUP_LONG;
    }
    dupNext = (dupNext + 1) % V2_DUP_WINDOW;
    if (dupFilled < V2_DUP_WINDOW) dupFilled++;
    sendFrame(reply.type, h->seq, V2_FLAG_RESPONSE, reply.data, reply.len, true);
    const uint32_t us = (halMicros() - t0) & HAL_US_MASK;  // handled and answered: the S3 waits on this
    if (us > reqMaxUs) {
        reqMaxUs = us;
        reqMaxType = h->type;
    }
}

uint32_t v2BytesSinceFrame(void) { return bytesSinceFrame; }

void v2RxBytes(const uint8_t *data, size_t len) {
    bytesSinceFrame += len;
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
            // Decoded in place (v2CobsDecode allows dst == src). The encoded bytes are not needed
            // again: rxLen goes to 0 below whatever the frame is, and nothing handleFrame() reaches
            // reads the serial port.
            const int n = v2CobsDecode(rxCobs, rxLen, rxCobs, V2_MAX_FRAME_RAW);
            if (n >= 5 && v2CheckFrame(rxCobs, n)) {
                handleFrame(rxCobs, (size_t)n);
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
    const unsigned need = 2u + (unsigned)len;
    if (outqFree() < need) {
        // The S3 is not reading. Room comes from LOG lines, oldest first, then from the oldest
        // check-ins and block reports not on the line; transfer results, DATA_NEEDED and tag data are
        // kept. Whether that can make enough room is decided before anything goes, so an event that
        // will not fit anyway does not cost the ones already waiting.
        if (outqFree() + outqReclaimable() < need) {
            v2EventsDropped++;  // nothing left to sacrifice: this event is lost
            return;
        }
        while (logCount && outqFree() < need) {
            logPop();
            if (logsDropped != 0xFFFF) logsDropped++;
        }
        while (outqFree() < need) {
            const unsigned victim = evVictim();
            if (victim == 0xFFFF) return;  // cannot happen after the test above; never index with it
            evRemove(victim);
            v2EventsDropped++;  // the check-in or block report we dropped to make room
        }
    }
    unsigned at;
    if (urgent) {
        // Before every event waiting, but behind the one on the line, which the S3 may already have.
        // With room below the queue only the event on the line moves, down; otherwise the events
        // behind it move up into the middle.
        const unsigned head = evOutstanding ? evSize(evLo) : 0;
        if (evLo >= need) {
            memmove(outq + evLo - need, outq + evLo, head);
            evLo = (uint16_t)(evLo - need);
        } else {
            if ((unsigned)(logLo - evHi) < need) outqCompact();
            memmove(outq + evLo + head + need, outq + evLo + head, evHi - evLo - head);
            evHi = (uint16_t)(evHi + need);
        }
        at = evLo + head;
    } else {
        if ((unsigned)(logLo - evHi) < need) outqCompact();
        at = evHi;
        evHi = (uint16_t)(evHi + need);
    }
    outq[at] = type;
    outq[at + 1] = (uint8_t)len;
    memcpy(outq + at + 2, payload, len);
    queued++;
    if (queued > evHigh) evHigh = queued;
    outqMark();
}

// A LOG frame takes a seq like an event, but gets no reply: send it only while no event waits for its
// ACK, so the S3 never sees the seq of an unanswered event twice.
static void sendPendingLog(void) {
    if (!logCount || evOutstanding || linkDown) return;
    if (logSentOnce && halMillis() - lastLogMs < V2_LOG_GAP_MS) return;
    lastLogMs = halMillis();
    logSentOnce = true;
    const unsigned len = outq[logHi - 1];
    sendFrame(V2_LOG, txSeq, 0, outq + logHi - 1 - len, len, false);
    logPop();
    txSeq++;
}

void v2Poll(void) {
    if (!active) return;
    sendPendingLog();
    if (!queued || linkDown) return;
    if (!evOutstanding) {
        sendHead(false);
        return;
    }
    if (halMillis() - evSentMs < V2_ACK_TIMEOUT_MS) return;
    if (evRetries < V2_RETRIES) {
        evRetries++;
        v2Retransmits++;
        sendHead(true);
    } else {
        // Keep the event and keep answering tags from local state; try again when the S3 talks
        linkDown = true;
    }
}

uint32_t v2MsUntilWork(void) {
    if (!active || linkDown) return UINT32_MAX;
    if (logCount && !evOutstanding) {
        const uint32_t since = halMillis() - lastLogMs;
        if (!logSentOnce || since >= V2_LOG_GAP_MS) return 0;
        if (!queued) return V2_LOG_GAP_MS - since;
    }
    if (!queued) return UINT32_MAX;
    if (!evOutstanding) return 0;
    const uint32_t since = halMillis() - evSentMs;
    return since >= V2_ACK_TIMEOUT_MS ? 0 : V2_ACK_TIMEOUT_MS - since;
}

void v2SetLogEnabled(bool on) { logEnabled = on || AP_DEBUG; }

// Queues one line, which v2Poll() sends when the line is free. Room for it comes from older lines,
// oldest first, and never from an event: with the ring full of events the new line is the one lost.
void v2Log(uint8_t level, const char *text) {
    if (!active || !logEnabled) return;
    size_t n = strlen(text);
    if (n > V2_LOG_MAX) n = V2_LOG_MAX;
    const unsigned need = (unsigned)n + 2u;  // level, text, length
    if (outqFree() + (unsigned)(logHi - logLo) < need) {
        if (logsDropped != 0xFFFF) logsDropped++;  // events fill the ring: this line is lost, not older ones
        return;
    }
    while (outqFree() < need) {
        logPop();
        if (logsDropped != 0xFFFF) logsDropped++;
    }
    if ((unsigned)(logLo - evHi) < need) outqCompact();
    logLo = (uint16_t)(logLo - need);
    outq[logLo] = level;
    memcpy(outq + logLo + 1, text, n);
    outq[logLo + 1 + n] = (uint8_t)(n + 1);
    logCount++;
    if (logCount > logHigh) logHigh = logCount;
    outqMark();
    // Queue only: v2Poll() sends it. Sending blocks until the bytes are in the UART, and a block request
    // is logged before its ack is scheduled, so sending here would delay the reply the tag is waiting
    // for (docs/design/radio-chain-analysis.md: nothing blocking in the radio path).
}
