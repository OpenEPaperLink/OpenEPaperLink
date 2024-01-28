#include "mz100_aon_ram.h"

#include <string.h>

#include "stdint.h"

__attribute__((section(".aon"))) volatile uint32_t aonChecksum;
__attribute__((section(".aon"))) volatile uint8_t aonShadow[AONSHADOW_SIZE];

bool aonRamValid = false;

void clearAonRam() {
    memset((void *)0x130000, 0, 4096);
    //memcpy((void *)(0x128000 - AONSHADOW_SIZE), (uint8_t *)aonShadow, AONSHADOW_SIZE);
}

bool loadValidateAonRam() {
    uint32_t testchecksum = aonChecksum;
    aonChecksum = 0x00000000;
    uint32_t checksum = 0xABBA5FF7;
    for (uint32_t c = 0x130000; c < 0x131000; c += 4) {
        checksum += *(uint32_t *)c;
    }
    if (checksum == testchecksum) {
        // immediately invalidate the checksum; if we reboot, we want a clean reboot
        aonChecksum = 0x5445A00A;
        memcpy((void *)(0x128000 - AONSHADOW_SIZE), (uint8_t *)aonShadow, AONSHADOW_SIZE);
        return true;
    } else {
        clearAonRam();
        memset((void *)(0x128000 - AONSHADOW_SIZE), 0, AONSHADOW_SIZE);
        return false;
    }
}

void saveAonRam() {
    memcpy((uint8_t *)aonShadow, (void *)(0x128000 - AONSHADOW_SIZE), AONSHADOW_SIZE);
    aonChecksum = 0x00000000;
    uint32_t checksum = 0xABBA5FF7;
    for (uint32_t c = 0x130000; c < 0x131000; c += 4) {
        checksum += *(uint32_t *)c;
    }
    aonChecksum = checksum;
}
