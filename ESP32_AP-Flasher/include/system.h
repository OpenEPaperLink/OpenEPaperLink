#include <Arduino.h>

#define WAKEUP_REASON_TIMED 0
#define WAKEUP_REASON_BOOT 1
#define WAKEUP_REASON_GPIO 2
#define WAKEUP_REASON_NFC 3
#define WAKEUP_REASON_BUTTON1 4
#define WAKEUP_REASON_BUTTON2 5
#define WAKEUP_REASON_FIRSTBOOT 0xFC
#define WAKEUP_REASON_NETWORK_SCAN 0xFD
#define WAKEUP_REASON_WDT_RESET 0xFE

void initTime(void* parameter);
void logLine(const char* buffer);
void logLine(const String& text);
void logStartUp();
