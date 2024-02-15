#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "settings.h"
#include "proto.h"

extern "C" {
#include <string.h>
#include "mz100/eeprom.h"
#include "mz100/util.h"
#include "mz100/mz100_flash.h"
#include "powermgt.h"
#include "mz100/printf.h"
}

#include "oepl_fs.h"

__attribute__((section(".aonshadow"))) struct tagsettings tagSettings = {0};
__attribute__((section(".aonshadow"))) struct tagHardwareProfile tagProfile = {0};

void loadDefaultSettings() {
    tagSettings.settingsVer = SETTINGS_STRUCT_VERSION;
    tagSettings.enableFastBoot = DEFAULT_SETTING_FASTBOOT;
    tagSettings.enableRFWake = DEFAULT_SETTING_RFWAKE;
    tagSettings.enableTagRoaming = DEFAULT_SETTING_TAGROAMING;
    tagSettings.enableScanForAPAfterTimeout = DEFAULT_SETTING_SCANFORAP;
    tagSettings.enableLowBatSymbol = DEFAULT_SETTING_LOWBATSYMBOL;
    tagSettings.enableNoRFSymbol = DEFAULT_SETTING_NORFSYMBOL;
    tagSettings.customMode = 0;
    tagSettings.fastBootCapabilities = 0;
    tagSettings.minimumCheckInTime = INTERVAL_BASE;
    tagSettings.fixedChannel = 0;
    tagSettings.batLowVoltage = BATTERY_VOLTAGE_MINIMUM;
}

#define SETTINGS_OFFSET 8
#define TAGPROFILE_OFFSET 32

void invalidateSettings() {
    uint64_t buffer = 0x5555555555555555ull;
    FLASH_Write(FLASH_PROGRAM_NORMAL, EEPROM_SETTINGS_AREA_START, (uint8_t*)&buffer, 8);
}

static bool compareSettings() {
    // check if the values are already the same, to ensure we don't write the same values to flash
    uint8_t buffer[256];
    uint64_t magic;
    FLASH_Read(FLASH_NORMAL_READ, EEPROM_SETTINGS_AREA_START, (uint8_t*)&magic, 8);
    if (magic != MAGIC_NUMBER_SETTINGS) {
        return false;
    }
    FLASH_Read(FLASH_NORMAL_READ, EEPROM_SETTINGS_AREA_START + SETTINGS_OFFSET, buffer, sizeof(struct tagsettings) + 64);
    if (memcmp((void*)buffer, (void*)&tagSettings, sizeof(struct tagsettings)) != 0) {
        return false;
    }
    FLASH_Read(FLASH_NORMAL_READ, EEPROM_SETTINGS_AREA_START + TAGPROFILE_OFFSET, buffer, sizeof(struct tagHardwareProfile));
    if (memcmp((void*)buffer, (void*)&tagProfile, sizeof(struct tagHardwareProfile)) != 0) {
        return false;
    }
#ifdef DEBUG_SETTINGS
    printf("SETTINGS: Settings/profile match, not saving.\n");
#endif
    return true;
}

void writeSettings() {
    if (compareSettings()) return;
#ifdef DEBUG_SETTINGS
    printf("SETTINGS: Writing settings\n");
#endif
    FLASH_Erase(EEPROM_SETTINGS_AREA_START, EEPROM_SETTINGS_AREA_START + 4095);
    FLASH_Write(FLASH_PROGRAM_NORMAL, EEPROM_SETTINGS_AREA_START + SETTINGS_OFFSET, (uint8_t*)&tagSettings, sizeof(struct tagsettings));
    FLASH_Write(FLASH_PROGRAM_NORMAL, EEPROM_SETTINGS_AREA_START + TAGPROFILE_OFFSET, (uint8_t*)&tagProfile, sizeof(struct tagHardwareProfile));
    uint64_t magic = MAGIC_NUMBER_SETTINGS;
    FLASH_Write(FLASH_PROGRAM_NORMAL, EEPROM_SETTINGS_AREA_START, (uint8_t*)&magic, 8);
}

bool loadSettings() {
    uint64_t magic;
    FLASH_Read(FLASH_NORMAL_READ, EEPROM_SETTINGS_AREA_START, (uint8_t*)&magic, 8);
    if (magic == MAGIC_NUMBER_SETTINGS) {
#ifdef DEBUG_SETTINGS
        printf("SETTINGS: Succesfully loaded settings\n");
#endif
        FLASH_Read(FLASH_NORMAL_READ, EEPROM_SETTINGS_AREA_START + SETTINGS_OFFSET, (uint8_t*)&tagSettings, sizeof(struct tagsettings));
        FLASH_Read(FLASH_NORMAL_READ, EEPROM_SETTINGS_AREA_START + TAGPROFILE_OFFSET, (uint8_t*)&tagProfile, sizeof(struct tagHardwareProfile));
        return true;
    } else {
#ifdef DEBUG_SETTINGS
        printf("SETTINGS: Default settings loaded\n");
#endif
        loadDefaultSettings();
        return false;
    }
}

bool loadProfileFromFile(char* filename) {
    OEPLFile* file = fs->getFile(filename);
    if (!file) {
        return false;
    }
    file->getBlock(0, (uint8_t*)&tagProfile, sizeof(struct tagHardwareProfile));
    writeSettings();
    return true;
}
