#ifndef VIRTUALTAG_H
#define VIRTUALTAG_H

#include <Arduino.h>

#include "commstructs.h"

// A "virtual tag" is a regular tagRecord with the isVirtual flag set. It has no
// physical radio counterpart; instead the web interface drives its behaviour:
// check-ins and button/GPIO/NFC presses are synthesized into the normal
// processDataReq() pipeline, and outgoing data (images, LED commands) is
// intercepted in sendDataAvail() and reflected back to the browser.

// Returns true if a tagRecord with this MAC exists and is flagged virtual.
bool vtagIsVirtual(const uint8_t* mac);

// Create (or convert) a virtual tag with the given MAC, hardware type and alias,
// then simulate a first check-in so content gets generated.
void vtagCreate(const uint8_t* mac, uint8_t hwType, const String& alias);

// Simulate the virtual tag waking up and checking in with the AP.
// wakeupReason is one of the WAKEUP_REASON_* values (TIMED, BUTTON1, BUTTON2,
// BUTTON3, GPIO, NFC, FIRSTBOOT, ...).
void vtagEvent(const uint8_t* mac, uint8_t wakeupReason);

// Periodic timed check-in: any virtual tag that hasn't been seen for >= 60s does
// a TIMED check-in, mimicking a real tag's wake interval. Call this regularly
// from the main loop.
void vtagAutoCheckin();

// Thread-safe request queue: the web handlers run in the async_tcp task and must
// NOT touch tagDB / the pending queue directly (they are owned by the loop/AP
// tasks). Instead they enqueue a request here, and vtagProcessPending() executes
// it from the main loop, on the same task as contentRunner/vtagAutoCheckin.
void vtagEnqueueCreate(const uint8_t* mac, uint8_t hwType, const String& alias);
void vtagEnqueueEvent(const uint8_t* mac, uint8_t wakeupReason);
void vtagProcessPending();

// Called from sendDataAvail() when the target MAC is a virtual tag. Consumes the
// pending data: images are "displayed" (preview written, like a real transfer),
// LED-flash commands are forwarded to the browser. Returns true (always handled).
bool vtagHandleDataAvail(struct pendingData* pending);

#endif  // VIRTUALTAG_H
