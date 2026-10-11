#pragma once

#include <Arduino.h>

#define WAKEUP_REASON_TIMED 0
#define WAKEUP_REASON_BOOT 1
#define WAKEUP_REASON_GPIO 2
#define WAKEUP_REASON_NFC 3
#define WAKEUP_REASON_BUTTON1 4
#define WAKEUP_REASON_BUTTON2 5
#define WAKEUP_REASON_BUTTON3 6
#define WAKEUP_REASON_FAILED_OTA_FW 0xE0
#define WAKEUP_REASON_FIRSTBOOT 0xFC
#define WAKEUP_REASON_NETWORK_SCAN 0xFD
#define WAKEUP_REASON_WDT_RESET 0xFE

void enableNtpFromDhcp();
void initTime(void* parameter);
void logLine(const char* buffer);
void logLine(const String& text);
void logLineRateLimited(const String& key, const String& text, const uint32_t intervalSeconds = 3600);
String urlForLog(const String& url);
void logStartUp();
const char* resetReasonName();
bool crashedLastBoot();
time_t lastTimeSync();
String ntpServerName();
void logCoreDump();
size_t coreDumpSize();
size_t readCoreDump(uint8_t* buffer, size_t offset, size_t len);
bool eraseCoreDump();
