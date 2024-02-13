#include "drawing.h"
#include <stdint.h>
#include <Arduino.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>

#include "eeprom.h"

#include "../../../oepl-proto.h"
#include "../../../oepl-definitions.h"

#include "userinterface.h"  // for addIcons

#include "powermgt.h"
#include "hal.h"
#include "qrcode.h"

#include "epd_driver/epd_interface.h"

#include "settings.h"

#include "compression.h"

uint32_t __attribute__((always_inline)) inline HAL_flashRead(uint32_t address, uint8_t *buffer, uint32_t num){
    eepromRead(address, buffer, num);
    return num;
}

#include "../../common/drawing.cpp"