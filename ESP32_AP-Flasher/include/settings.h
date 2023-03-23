#include <Arduino.h>

// how long the we should keep the transfer metadata
#define PENDING_TIMEOUT 24 * 3600
// this determines how long images will be cached;
#define PENDING_DATA_TIMEOUT 60
// maximum time (in minutes) that a tag is put to sleep if no update is expected.
#define MIN_RESPONSE_TIME 10

// flasher options
#define CUSTOM_MAC_HDR 0x0000

#if (PINOUT == OPENEPAPERLINK_PCB)
#define FLASHER_AP_SS 4
#define FLASHER_AP_CLK 5
#define FLASHER_AP_MOSI 7
#define FLASHER_AP_MISO 6
#define FLASHER_AP_RESET 15
#define FLASHER_AP_POWER 0
#define FLASHER_AP_TXD 16
#define FLASHER_AP_RXD 18
#define FLASHER_AP_TEST 17

#define FLASHER_EXT_SS 40
#define FLASHER_EXT_CLK 41
#define FLASHER_EXT_MOSI 2
#define FLASHER_EXT_MISO 42
#define FLASHER_EXT_RESET 1
#define FLASHER_EXT_POWER 8
#define FLASHER_EXT_TXD 38
#define FLASHER_EXT_RXD 39
#define FLASHER_EXT_TEST 47

#define FLASHER_ALT_SS 3
#define FLASHER_ALT_CLK 46
#define FLASHER_ALT_MOSI 10
#define FLASHER_ALT_MISO 9
#define FLASHER_ALT_RESET 11
#define FLASHER_ALT_TXD 12
#define FLASHER_ALT_RXD 14
#define FLASHER_ALT_TEST 13

#define FLASHER_LED 21
#define FLASHER_RGB_LED 48
#endif

#if (PINOUT == SIMPLE_AP)
/* Lolin32 lite connections to AP tag*/
#define FLASHER_AP_SS 5
#define FLASHER_AP_CLK 18
#define FLASHER_AP_MOSI 23
#define FLASHER_AP_MISO 19
#define FLASHER_AP_RESET 2
#define FLASHER_AP_POWER 13
#define FLASHER_AP_POWER2 15
#define FLASHER_AP_TEST -1
#define FLASHER_AP_TXD 17
#define FLASHER_AP_RXD 16

#define FLASHER_LED 22
#endif

#define MAX_WRITE_ATTEMPTS 5
