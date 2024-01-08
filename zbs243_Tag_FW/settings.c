#define __packed
#include "settings.h"

// #include <flash.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "asmUtil.h"
#include "powermgt.h"
#include "printf.h"
#include "syncedproto.h"
#include "eeprom.h"
#include "../oepl-definitions.h"
#include "../oepl-proto.h"

#define SETTINGS_MAGIC 0xABBA5AA5

struct tagsettings __xdata tagSettings = {0};
extern uint8_t __xdata blockbuffer[];
uint8_t* __xdata settingsTempBuffer = 1024 + blockbuffer;

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

void loadSettingsFromBuffer(uint8_t* p) {
#ifdef DEBUGSETTINGS
    pr("SETTINGS: received settings from AP\n");
#endif
    switch (*p) {
        case SETTINGS_STRUCT_VERSION:  // the current tag struct
            memcpy((void*)tagSettings, (void*)p, sizeof(struct tagsettings));
            break;
        default:
#ifdef DEBUGSETTINGS
            pr("SETTINGS: received something we couldn't really process, version %d\n");
#endif
            break;
    }
    tagSettings.fastBootCapabilities = capabilities;
    writeSettings();
}

static bool compareSettings() {
    // check if the settings match the settings in the eeprom
    eepromRead(EEPROM_SETTINGS_AREA_START, (void*)settingsTempBuffer, sizeof(struct tagsettings));
    if (memcmp((void*)settingsTempBuffer, (void*)tagSettings, sizeof(struct tagsettings)) == 0) {
        // same
        return true;
    }
    // different
    return false;
}

static void upgradeSettings() {
    // add an upgrade strategy whenever you update the struct version
}

void loadSettings() {
    eepromRead(EEPROM_SETTINGS_AREA_START + 4, (void*)settingsTempBuffer, sizeof(struct tagsettings));
    memcpy((void*)&tagSettings, (void*)settingsTempBuffer, sizeof(struct tagsettings));
    uint32_t __xdata valid = 0;
    eepromRead(EEPROM_SETTINGS_AREA_START, (void*)&valid, 4);
    xMemCopy((void*)tagSettings, (void*)settingsTempBuffer, sizeof(struct tagsettings));
    if (tagSettings.settingsVer == 0xFF || valid != SETTINGS_MAGIC) {
        // settings not set. load the defaults
        loadDefaultSettings();
#ifdef DEBUGSETTINGS
        pr("SETTINGS: Loaded default settings\n");
#endif
    } else {
        if (tagSettings.settingsVer < SETTINGS_STRUCT_VERSION) {
            // upgrade
            upgradeSettings();
#ifdef DEBUGSETTINGS
            pr("SETTINGS: Upgraded from previous version\n");
#endif
        } else {
            // settings are valid
#ifdef DEBUGSETTINGS
            pr("SETTINGS: Loaded from EEPROM\n");
#endif
        }
    }
}

void writeSettings() {
    if (compareSettings()) {
#ifdef DEBUGSETTINGS
        pr("SETTINGS: Settings matched current settings\n");
#endif
        return;
    }
    eepromErase(EEPROM_SETTINGS_AREA_START, 1);
    uint32_t __xdata valid = SETTINGS_MAGIC;
    eepromWrite(EEPROM_SETTINGS_AREA_START, (void*)&valid, 4);
    eepromWrite(EEPROM_SETTINGS_AREA_START + 4, (void*)&tagSettings, sizeof(tagSettings));
#ifdef DEBUGSETTINGS
    pr("SETTINGS: Updated settings in EEPROM\n");
#endif
}

void invalidateSettingsEEPROM() {
    int32_t __xdata valid = 0x0000;
    eepromWrite(EEPROM_SETTINGS_AREA_START, (void*)&valid, 4);
}