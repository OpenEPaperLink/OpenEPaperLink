// #pragma pack(1)

#include "drawing.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "epd_interface.h"

#include "mz100/printf.h"

#include "mz100/eeprom.h"

#include "compression.h"
#include "oepl_fs.h"

#include "../common/QRCode/src/qrcode.h"

#include "../../oepl-definitions.h"
#include "../../oepl-proto.h"

extern "C" {
extern void dump(const uint8_t *a, const uint16_t l);
#include "mz100/util.h"
#include "mz100/mz100_flash.h"
#include "../common/QRCode/src/qrcode.h"

}

#include "userinterface.h"

#include "epd_interface.h"

uint32_t __attribute__((always_inline)) inline HAL_flashRead(uint32_t address, uint8_t *buffer, uint32_t num){
     return FLASH_Read(FLASH_FAST_READ_QUAD_OUT, address, buffer, num);
}

#include "../common/drawing.cpp"