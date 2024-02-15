#include "drawing.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "epd_interface.h"

#include "mz100/printf.h"

#include "mz100/eeprom.h"

#include "oepl_fs.h"

#include "compression.h"

#include "../../oepl-definitions.h"
#include "../../oepl-proto.h"

#include "settings.h"

extern "C" {
extern void dump(const uint8_t *a, const uint16_t l);
#include "mz100/util.h"
#include "mz100/mz100_flash.h"
#include "../common/uzlib/src/uzlib.h"
}

#define MAX_WINDOW_SIZE 8192
#define ZLIB_CACHE_SIZE 512

uint32_t __attribute__((always_inline)) inline HAL_flashRead(uint32_t address, uint8_t *buffer, uint32_t num){
     return FLASH_Read(FLASH_FAST_READ_QUAD_OUT, address, buffer, num);
}

#include "../common/compression.cpp"