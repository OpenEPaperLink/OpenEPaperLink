#pragma once

// Radio side of the S3 <-> radio protocol v2 (oepl-radio-v2.h): frame reception, duplicate
// detection, responses, and the queue of events to the S3 with retransmission. Everything runs in
// the main loop; nothing here waits for the S3. Ported from the ESP32-C6/H2 radio firmware.

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../../../oepl-radio-v2.h"

#define V2_LOG_MAX 100  // longest LOG text; ap.c builds its lines in buffers of this size

struct v2Reply {
    uint8_t type;  // V2_ACK, V2_NAK, V2_HELLO_REPLY or V2_STATUS
    uint8_t len;
    uint8_t data[48];
};

// Implemented by the application (ap.c): handle one request from the S3 and fill in the reply.
// reply->type is V2_ACK with no data when called.
void v2HandleRequest(uint8_t type, const uint8_t *payload, size_t len, struct v2Reply *reply);

void v2Start(void);  // after FRM1, at the new baud rate
void v2Stop(void);   // back to the ASCII protocol
bool v2Active(void);
void v2RxBytes(const uint8_t *data, size_t len);
void v2Poll(void);                  // sends and resends events; call every loop pass
uint32_t v2MsUntilWork(void);       // how long the loop may sleep before v2Poll() has work
uint32_t v2MsSinceLastFrame(void);  // since the last valid frame from the S3
// Bytes received since the last valid frame. Bytes without a frame mean the S3 is sending at another
// speed (it rebooted and is back at 115200), not that the line is quiet.
uint32_t v2BytesSinceFrame(void);

// Queues an event for the S3. Urgent events (DATA_NEEDED) go before the other queued events.
void v2QueueEvent(uint8_t type, const void *payload, size_t len, bool urgent);

// A LOG frame (no reply, may be lost); only sent when the S3 enabled them, and only between events
void v2SetLogEnabled(bool on);
void v2Log(uint8_t level, const char *text);

// Counters for STATUS
extern uint16_t v2CrcErrors;
extern uint16_t v2Overflows;
extern uint16_t v2Retransmits;
extern uint16_t v2EventsDropped;  // events the ring had no room for (the S3 is not reading)
uint8_t v2EventsQueued(void);
// High-water marks since boot, for sizing the one ring events and LOG lines share: the most events
// waiting at once, the most bytes of it in use (V2_OUTQ_BYTES in v2link.c), the most LOG lines
// waiting, and LOG lines lost to a full ring (so a report missing lines is not mistaken for a short
// one).
uint8_t v2EventsHigh(void);
uint16_t v2OutHigh(void);
uint16_t v2LogHigh(void);
uint16_t v2LogsDropped(void);
uint32_t v2ReqMaxUs(void);    // the slowest request handled since boot, received to answered
uint8_t v2ReqMaxType(void);   // ... and what it was
