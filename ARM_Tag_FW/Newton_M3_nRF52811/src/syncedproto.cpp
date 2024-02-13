#include "syncedproto.h"

#include <Arduino.h>
#include <md5.h>

#include "comms.h"
#include "drawing.h"
#include "eeprom.h"
#include "hal.h"
#include "powermgt.h"
#include "../../../oepl-proto.h"
#include "../../../oepl-definitions.h"
#include "userinterface.h"
#include "wdt.h"

#define FW_LOC 0
#define FW_METADATA_LOC 196608
#define MAGIC1 0xABBAABBA
#define MAGIC2 0xDEADBEEF
struct fwmetadata {
    uint32_t fwsize;
    uint32_t magic1;
    uint32_t magic2;
};

#define EEPROM_SETTINGS_SIZE 4096

#define BLOCKSIZE_MS 240  // was 270

#define FWNRF
#define LEDSENABLED
#define PERSISTENTVAR 
#define HAL_TIMER_TICK 1
#define EEPROM_IMG_START 0


#define HAL_PacketRX commsRxUnencrypted
#define HAL_msDelay delay

void dump(const uint8_t *a, const uint16_t l);

static void saveUpdateMetadata(uint32_t size) {
    struct fwmetadata metadata;
    metadata.magic1 = MAGIC1;
    metadata.magic2 = MAGIC2;
    metadata.fwsize = size;
    eepromWrite(FW_METADATA_LOC, &metadata, sizeof(struct fwmetadata));
}

static bool validateEepromMD5(uint64_t ver, uint32_t eepromstart, uint32_t flen) {
    unsigned char hash[16];
    char chunk[512];
    MD5 md5;

    // Open the executable itself for reading
    md5.reset();
    for (uint32_t offset = 0; offset < flen; offset += 512) {
        uint32_t len = flen - offset;
        if (len > 512) len = 512;
        eepromRead(eepromstart + offset, chunk, 512);
        md5.update(chunk, len);
    }

    // Retrieve the final hash
    md5.finalize(hash);

    bool isValid = ver == *((uint64_t *)hash);
    if (!isValid) {
        printf("MD5 failed check! This is what we should get:\n");
        dump((const uint8_t *)&(ver), 8);
        printf("This is what we got:\n");
        dump(hash, 16);
    }
    return true;
    return isValid;
}

#include "../../common/oepl-protocol.cpp"