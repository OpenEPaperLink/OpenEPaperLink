#ifndef _PROTO_H_
#define _PROTO_H_

#include <stdint.h>

#include "../../oepl-definitions.h"
#include "../../oepl-proto.h"

#define MACFMT "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x"
#define MACCVT(x) ((const uint8_t*)(x))[7], ((const uint8_t*)(x))[6], ((const uint8_t*)(x))[5], ((const uint8_t*)(x))[4], ((const uint8_t*)(x))[3], ((const uint8_t*)(x))[2], ((const uint8_t*)(x))[1], ((const uint8_t*)(x))[0]

#endif