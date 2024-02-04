#include <Arduino.h>

#ifdef HAS_EXT_FLASHER

#include "web.h"

#define WEBFLASH_ENABLE_AUTOFLASH 1
#define WEBFLASH_ENABLE_USBFLASHER 2
#define WEBFLASH_FOCUS 3
#define WEBFLASH_BLUR 4

class Logger : public Print {
   public:
    Logger();

    ~Logger();

    size_t write(uint8_t c) override;
    size_t write(const uint8_t* buffer, size_t size) override;
    size_t printf(const char* format, ...);
    size_t println(const char* text);

   private:
};

void webFlasherTask(void* parameter);
void handleWSdata(uint8_t* data, size_t len, AsyncWebSocketClient* client);
void sendDataToClient(const uint8_t* data, size_t len);

#endif