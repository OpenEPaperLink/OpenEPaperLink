#include <Arduino.h>

#define FLASHER_AP_PORT 0
#ifdef HAS_EXT_FLASHER
#define FLASHER_EXT_PORT 1
#define FLASHER_ALTRADIO_PORT 2
#endif

// Which port we should use for the AP process
// (useful for testing, can only be FLASHER_AP_PORT for any other board than the OpenEPaperLinkPCB board)
#define AP_PROCESS_PORT FLASHER_AP_PORT

// flasher options
#define CUSTOM_MAC_HDR 0x0000

#define MAX_WRITE_ATTEMPTS 5