#include <Arduino.h>

#define FLASHER_AP_PORT 0
#ifdef OPENEPAPERLINK_PCB
#define FLASHER_EXT_PORT 1
#define FLASHER_ALTRADIO_PORT 2
#endif

// how long the we should keep the transfer metadata
#define PENDING_TIMEOUT 24 * 3600
// this determines how long images will be cached;
#define PENDING_DATA_TIMEOUT 60
// maximum time (in minutes) that a tag is put to sleep if no update is expected.
#define MIN_RESPONSE_TIME 10

// Which port we should use for the AP process
// (useful for testing, can only be FLASHER_AP_PORT for any other board than the OpenEPaperLinkPCB board)
#define AP_PROCESS_PORT FLASHER_AP_PORT

// flasher options
#define CUSTOM_MAC_HDR 0x0000

#define MAX_WRITE_ATTEMPTS 5