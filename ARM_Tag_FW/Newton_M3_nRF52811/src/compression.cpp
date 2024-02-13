#include <vector>
#include <Arduino.h>
#include "eeprom.h"
#include "comms.h"
#include "powermgt.h"

#include "syncedproto.h"
#include "hal.h"

#include "compression.h"

#include "../../../oepl-definitions.h"
#include "../../../oepl-proto.h"

#include "settings.h"

#define MAX_WINDOW_SIZE 8192
#define ZLIB_CACHE_SIZE 256

uint32_t __attribute__((always_inline)) inline HAL_flashRead(uint32_t address, uint8_t *buffer, uint16_t numbytes) {
    eepromRead(address, (void *)buffer, numbytes);
    return numbytes;;
}

#include "../../common/compression.cpp"