
#include "oepl-protocol.h"

#include "powermgt.h"
#include "main.h"

extern "C" {
#include "board.h"
#include "comms.h"
#include "mz100/eeprom.h"
#include "mz100/mz100_sleep.h"
#include "mz100/printf.h"
#include "proto.h"
#include "mz100/timer.h"
#include "mz100/util.h"
#include "mz100/mz100_flash.h"
#include "zigbee.h"
#include "../common/md5.h"
}

#include "drawing.h"

#include "oepl_fs.h"

#include "epd_interface.h"

#include "settings.h"

#define BLOCKSIZE_MS 270  // was 270

#define FW88MZ100

#define HAL_PacketRX commsRxUnenc
#define millis() timerGet()
#define HAL_TIMER_TICK (TIMER_TICKS_PER_MSEC)


void inline HAL_msDelay(uint32_t t) {
    timerDelay(t * TIMER_TICKS_PER_MSEC);
}

void executeCommand(uint8_t c) {
}

void loadSettingsFromBuffer(uint8_t *buffer) {
}

#define EEPROM_IMG_START fsEnd

#define FW_LOC fsEnd
#define FW_METADATA_LOC 0

void dump(const uint8_t *a, const uint16_t l);

static bool validateEepromMD5(uint64_t ver, uint32_t eepromstart, uint32_t flen) {
#define CHUNK_SIZE 512
    uint8_t chunk[CHUNK_SIZE];
    MD5Context ctx;
    md5Init(&ctx);
    // Open the executable itself for reading
    for (uint32_t offset = 0; offset < flen; offset += CHUNK_SIZE) {
        uint32_t len = flen - offset;
        if (len > CHUNK_SIZE) len = CHUNK_SIZE;
        FLASH_Read(FLASH_NORMAL_READ, eepromstart + offset, chunk, len);
        // eepromRead(eepromstart + offset, chunk, 512);
        md5Update(&ctx, chunk, len);
    }

    // Retrieve the final hash
    md5Finalize(&ctx);

    bool isValid = ver == *((uint64_t *)ctx.digest);
    if (!isValid) {
        printf("MD5 failed check! This is what we should get:\n");
        dump((const uint8_t *)&(ver), 8);
        printf("This is what we got:\n");
        dump(ctx.digest, 16);
    } else {
#ifdef DEBUG_PROTO
        printf("PROTO: MD5 Pass\n");
#endif
    }
#ifdef DEBUG_DONTVALIDATEPROTO
    return true;
#else
    return isValid;
#endif
}

#include "../common/oepl-protocol.cpp"
