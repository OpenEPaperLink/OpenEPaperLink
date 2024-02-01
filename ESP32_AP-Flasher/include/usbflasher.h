#include <Arduino.h>

#define TRANSPORT_USB 0
#define TRANSPORT_TCP 1

#ifdef HAS_USB
void usbFlasherTask(void* parameter);
#endif
void flasherDataHandler(uint8_t* data, size_t len, uint8_t transportType);
void processFlasherCommand(struct flasherCommand* cmd, uint8_t transportType);
