#pragma once

// Interface between the AP logic (serialap.cpp, newproto.cpp) and the serial protocol to the radio
// co-processor. LegacyTransport (legacytransport.cpp) is the ASCII protocol of the TLSR and ZBS radios
// and older C6/H2 firmware; V2Transport (v2transport.cpp) the framed protocol of ../../oepl-radio-v2.h,
// chosen by bringAPOnline() when the radio announces it. The AP-state checks (no radio, offline,
// flashing) stay in serialap.cpp.

#include <Arduino.h>

#include "commstructs.h"

#define AP_SERIAL_PORT Serial1

// Messages from the radio, handed to rxCmdProcessor() (serialap.cpp) through addRXQueue()
#define RX_CMD_RQB 0x01
#define RX_CMD_ADR 0x02
#define RX_CMD_XFC 0x03
#define RX_CMD_XTO 0x04
#define RX_CMD_RDY 0x05
#define RX_CMD_RSET 0x06
#define RX_CMD_TRD 0x07
#define RX_CMD_PRELOAD 0x08  // send a pending item to the radio's block cache (struct espBlockRequest: src, ver)
#define RX_CMD_BLS 0x09      // the radio served a block (struct v2BlockServed)

void addRXQueue(uint8_t* data, uint8_t len, uint8_t type);
extern SemaphoreHandle_t txActive;        // one command on the line at a time
extern volatile uint32_t lastAPActivity;  // last time the radio said something

class RadioTransport {
   public:
    virtual ~RadioTransport() {}
    virtual const char* name() = 0;

    // Commands to the radio; true (or the block checksum) when the radio confirmed them. sendBlock gets
    // one block of a file: its data, where it starts in the file, and the file's version and size.
    virtual uint16_t sendBlock(const void* data, uint16_t len, uint64_t ver, uint32_t offset, uint32_t totalSize) = 0;
    virtual bool sendDataAvail(struct pendingData* pending) = 0;
    virtual bool cancelPending(struct pendingData* pending) = 0;
    virtual bool setChannelPower(struct espSetChannelPower* scp) = 0;
    // One block for the radio's block cache, ahead of the tag's request; false without a cache
    virtual bool cacheBlock(const void* data, uint16_t len, uint64_t ver, uint32_t offset, uint32_t totalSize) { return false; }

    // Link setup, used by bringAPOnline()
    virtual bool ping() = 0;            // is the radio there?
    virtual bool requestInfo() = 0;     // fills apInfo (type, version, MAC, channel, ...)
    virtual bool enableFastLink() = 0;  // switch the link to its fast mode (2 Mbaud)
    virtual bool enableFraming() { return false; }  // ask the radio to switch to protocol v2 (FRM1)

    // Link supervision by APTask: with heartbeat() the radio is pinged every pingIntervalMs(),
    // otherwise only after that long without hearing from it. After maxMissedPings() failures in a
    // row the link is brought up again.
    virtual bool heartbeat() { return false; }
    virtual uint32_t pingIntervalMs() { return 30 * 1000; }
    virtual uint8_t maxMissedPings() { return 6; }

    // The tag inventory lock in the radio (v2 radios with V2_CAP_ALLOW_LIST): sends the tag database
    // changes queued by radioAllowListChanged() to the radio's allow-list. Called by APTask.
    virtual void syncAllowList() {}

    // Receiving, called by rxSerialTask(): onIdle() before reading, onByte() for every byte
    virtual void onIdle() = 0;
    virtual void onByte(uint8_t c) = 0;
};

extern RadioTransport* radioTransport;
RadioTransport* legacyRadioTransport();
RadioTransport* v2RadioTransport();
bool v2StartLink();  // after FRM1 and the switch to V2_BAUD: HELLO, then radioTransport is v2
void radioAllowListChanged(const uint8_t mac[8], bool added);  // a MAC came into or left the tag database
