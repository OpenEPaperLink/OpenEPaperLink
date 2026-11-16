#pragma once

// The access point logic: answers tags from the pending table and the block cache, talks protocol v2
// (oepl-radio-v2.h) with the ESP32-S3. Ported from the ESP32-C6/H2 radio firmware (main.c, 0x0024);
// hardware access only through hal.h.

#include <stdint.h>

#define AP_HW_TYPE 0xF8  // TLSR_ZIGBEE_AP in oepl-definitions.h: "TLSR Zigbee 802.15.4"
#ifndef AP_FW_VERSION
#define AP_FW_VERSION 0x0080  // the closed-source binary reports 0x007C (listed as 0001 in firmware_TLSR.json)
#endif

void apInit(void);
// One pass of the main loop. Returns how long the caller may sleep before the next pass has work
// (0 = call again at once); a received radio frame or serial byte ends such a sleep early.
uint32_t apLoop(void);
