#pragma once

// Interface between the AP logic (serialap.cpp, newproto.cpp) and the serial protocol to the radio
// co-processor. LegacyTransport (legacytransport.cpp) is the protocol all radios speak today; a framed
// protocol (docs/design/radio-protocol-v2.md) is to be added as a second implementation, chosen when
// the radio comes online. The AP-state checks (no radio, offline, flashing) stay in serialap.cpp.

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

void addRXQueue(uint8_t* data, uint8_t len, uint8_t type);
extern SemaphoreHandle_t txActive;        // one command on the line at a time
extern volatile uint32_t lastAPActivity;  // last time the radio said something

class RadioTransport {
   public:
    virtual ~RadioTransport() {}
    virtual const char* name() = 0;

    // Commands to the radio; true (or the block checksum) when the radio confirmed them
    virtual uint16_t sendBlock(const void* data, uint16_t len) = 0;
    virtual bool sendDataAvail(struct pendingData* pending) = 0;
    virtual bool cancelPending(struct pendingData* pending) = 0;
    virtual bool setChannelPower(struct espSetChannelPower* scp) = 0;

    // Link setup, used by bringAPOnline()
    virtual bool ping() = 0;            // is the radio there?
    virtual bool requestInfo() = 0;     // fills apInfo (type, version, MAC, channel, ...)
    virtual bool enableFastLink() = 0;  // switch the link to its fast mode (2 Mbaud)

    // Receiving, called by rxSerialTask(): onIdle() before reading, onByte() for every byte
    virtual void onIdle() = 0;
    virtual void onByte(uint8_t c) = 0;
};

extern RadioTransport* radioTransport;
