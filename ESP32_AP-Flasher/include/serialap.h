#include <Arduino.h>

extern struct espSetChannelPower curChannel;

#define AP_STATE_OFFLINE 0
#define AP_STATE_ONLINE 1
#define AP_STATE_FLASHING 2
#define AP_STATE_WAIT_RESET 3
#define AP_STATE_REQUIRED_POWER_CYCLE 4
#define AP_STATE_FAILED 5
#define AP_STATE_COMING_ONLINE 6
#define AP_STATE_NORADIO 7

struct APInfoS {
    volatile bool isOnline = false;
    volatile uint8_t state = AP_STATE_OFFLINE;
    uint8_t type;
    uint16_t version = 0;
    uint8_t channel;
    uint8_t mac[8];
    uint8_t power;
    uint8_t pendingBuffer;
    uint8_t nop;
#ifdef HAS_SUBGHZ
    bool hasSubGhz = false;
    uint8_t SubGhzChannel;
#endif
};

extern struct APInfoS apInfo;

/* Readable name for an AP type byte, for logs that would otherwise print a
   bare number. The compile time SHORT_CHIP_NAME cannot do this job: a board
   that can carry either module has it fixed to one of them, so it says "C6"
   even while a TLSR is being flashed. apInfo.type is what the module itself
   reported, so it is the only value that is actually true. */
const char *apTypeName(uint8_t type);

// The task that reads the radio's UART (rxSerialTask). Everything that needs the port for
// something else (flashing, self-test) stops it first; bringAPOnline() starts it again.
// startApReader() does nothing while it runs, so there is never more than one reader.
bool startApReader();
bool stopApReader();
bool apReaderRunning();

void APTask(void* parameter);

bool sendCancelPending(struct pendingData* pending);
bool sendDataAvail(struct pendingData* pending);
bool sendPing();
void APEnterEarlyReset();
bool sendChannelPower(struct espSetChannelPower* scp);
void rxSerialTask2(void* parameter);
void APTagReset();
bool bringAPOnline(uint8_t newState = AP_STATE_ONLINE);
void setAPstate(bool isOnline, uint8_t state);
