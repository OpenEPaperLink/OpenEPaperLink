#include <Arduino.h>

#define CHECK_IN_DELAY 900000
#define RETRY_DELAY 1000
#define FAILED_TILL_BLANK 2
#define FAILED_TILL_REASSOCIATE 1

// how long the we should keep the transfer metadata
#define PENDING_TIMEOUT 600
// this determines how long images will be cached;
#define PENDING_DATA_TIMEOUT 60

// flasher options
#define CUSTOM_MAC_HDR 0x0000

/*
#define RXD1 16
#define TXD1 17

#define ZBS_SS 5
#define ZBS_CLK 18
#define ZBS_MoSi 23
#define ZBS_MiSo 19
#define ZBS_Reset 2
#define ZBS_POWER1 13
#define ZBS_POWER2 15
*/

#define RXD1 13   
#define TXD1 12   

#define ZBS_SS 21
#define ZBS_CLK 18
#define ZBS_MoSi 22
#define ZBS_MiSo 19
#define ZBS_Reset 5
#define ZBS_POWER1 15
#define ZBS_POWER2 2

#define MAX_WRITE_ATTEMPTS 5

