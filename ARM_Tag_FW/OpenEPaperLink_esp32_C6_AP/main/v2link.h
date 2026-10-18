#pragma once

// Radio side of the S3 <-> radio protocol v2 (oepl-radio-v2.h): frame reception, duplicate
// detection, responses, and the queue of events to the S3 with retransmission. Everything runs in
// the main loop; nothing here waits for the S3.

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../../../oepl-radio-v2.h"

struct v2Reply {
    uint8_t type;  // V2_ACK, V2_NAK, V2_HELLO_REPLY or V2_STATUS
    uint8_t len;
    uint8_t data[48];
};

// Implemented by the application (main.c): handle one request from the S3 and fill in the reply.
// reply->type is V2_ACK with no data when called.
void v2HandleRequest(uint8_t type, const uint8_t *payload, size_t len, struct v2Reply *reply);

void v2Start(void);    // after FRM1, at the new baud rate
void v2Stop(void);     // back to the ASCII protocol
bool v2Active(void);
void v2RxBytes(const uint8_t *data, size_t len);
void v2Poll(void);                  // sends and resends events; call every loop pass
uint32_t v2MsUntilWork(void);       // how long the loop may sleep before v2Poll() has work
uint32_t v2MsSinceLastFrame(void);  // since the last valid frame from the S3

// Queues an event for the S3. Urgent events (DATA_NEEDED) go before the other queued events.
void v2QueueEvent(uint8_t type, const void *payload, size_t len, bool urgent);

// Counters for STATUS
extern uint16_t v2CrcErrors;
extern uint16_t v2Overflows;
extern uint16_t v2Retransmits;
uint8_t v2EventsQueued(void);
