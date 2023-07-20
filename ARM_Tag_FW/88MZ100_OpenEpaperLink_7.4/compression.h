#pragma once

#include "uzlib/src/uzlib.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

uint32_t decompress_file(uint32_t address_in, uint32_t size_compressed, uint32_t address_temp, uint32_t max_len_temp, char error_reason[100]);