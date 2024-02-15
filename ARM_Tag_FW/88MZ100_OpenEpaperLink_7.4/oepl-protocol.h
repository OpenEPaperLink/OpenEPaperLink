#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "settings.h"

#define PERSISTENTVAR __attribute__((section(".aonshadow")))

#include "../common/oepl-protocol.h"