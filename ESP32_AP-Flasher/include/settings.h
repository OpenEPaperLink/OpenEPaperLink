#include <Arduino.h>

// how long the we should keep the transfer metadata
#define PENDING_TIMEOUT 24 * 3600
// this determines how long images will be cached;
#define PENDING_DATA_TIMEOUT 60
// maximum time (in minutes) that a tag is put to sleep if no update is expected.
#define MIN_RESPONSE_TIME 10

// flasher options
#define CUSTOM_MAC_HDR 0x0000

#define MAX_WRITE_ATTEMPTS 5