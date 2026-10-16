#pragma once
#ifdef HAS_BLE_WRITER
#include <Arduino.h>
#include <ArduinoJson.h>

// What the AP uses from an OpenDisplay tag's config (display packet 0x20, security 0x27,
// binary inputs 0x25). Read over BLE once and kept in /odcfg/<BLE address>.bin.
struct OdConfig {
    uint16_t panelIc;
    uint16_t width;
    uint16_t height;
    uint16_t widthMm;
    uint16_t heightMm;
    uint16_t legacyTagType;
    uint8_t rotation;
    uint8_t colorScheme;
    uint8_t transmissionModes;
    bool encryption;
    bool hasLed;
    uint8_t buttonBlocks;
    uint8_t buttonByteIndex[4];  // advertisement byte of each binary input block, 0xFF = none
};

struct imgParam;

void odInit();
// Rendering for an OpenDisplay tag: buffer layout and color order from its config, no
// compression (the upload compresses with the window the tag can take)
void odImageParams(const uint8_t mac[8], imgParam& imageParams);
// Called by the BLE task when it has nothing else to do: reads the config of one tag
// that needs it (blocks for the length of one BLE session).
void odService();
// Sends the pending image of a tag; false when it should be tried again later
bool odUpload(const uint8_t mac[8]);
// The wakeup reason for a button press seen in the advertisement's dynamic bytes, else 0
uint8_t odButtonEvent(const uint8_t mac[8], const uint8_t dynamic[11]);
bool odGetConfig(const uint8_t mac[8], OdConfig& cfg);
// The key changed: try that tag again
void odKeyChanged(const uint8_t mac[8]);
// Roaming between APs that have the same tag added (see od_session.cpp):
// this AP saw the tag; true when it owns it (now), so the check-in is processed
bool odLocalCheckin(const uint8_t mac[8]);
// another AP forwarded a check-in of the tag; true when that AP has it, so accept it
bool odRemoteCheckin(const uint8_t mac[8], IPAddress from);
// only the owner talks to the tag
bool odIsOwner(const uint8_t mac[8]);
// The tag was removed: drop its config, so adding it again reads it fresh (it may have been
// reconfigured meanwhile)
void odForget(const uint8_t mac[8]);
// State and display summary of an OpenDisplay tag for the web UI
void odDescribe(const uint8_t mac[8], JsonObject od);
#endif
