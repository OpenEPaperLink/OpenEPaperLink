#define __packed
#include "settings.h"

#include <flash.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


#include "asmUtil.h"
#include "powermgt.h"
#include "printf.h"
#include "syncedproto.h"
#include "../oepl-definitions.h"
#include "../oepl-proto.h"

struct tagsettings __xdata tagSettings = {0};
extern uint8_t __xdata blockXferBuffer[];
uint8_t* __xdata infopageTempBuffer = 1024 + blockXferBuffer;

#define INFOPAGE_SETTINGS_OFFSET 0x50

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
    pr("SETTINGS: received settings from AP\n");
    switch (*p) {
        case SETTINGS_STRUCT_VERSION:  // the current tag struct
            pr("SETTINGS: received matching version\n");
            memcpy((void*)tagSettings, (void*)p, sizeof(struct tagsettings));
            break;
        default:
            pr("SETTINGS: received something we couldn't really process, version %d\n");
        break;
    }
    tagSettings.fastBootCapabilities = capabilities;
    writeSettings();
}

static bool compareSettings() {
    // check if the settings match the settings in the infopage
    flashRead(FLASH_INFOPAGE_ADDR + INFOPAGE_SETTINGS_OFFSET, (void*)infopageTempBuffer, sizeof(struct tagsettings));
    if (memcmp((void*)infopageTempBuffer, (void*)tagSettings, sizeof(struct tagsettings)) == 0) {
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
    flashRead((FLASH_INFOPAGE_ADDR + INFOPAGE_SETTINGS_OFFSET), (void*)infopageTempBuffer, sizeof(struct tagsettings));
    xMemCopy((void*)tagSettings, (void*)infopageTempBuffer, sizeof(struct tagsettings));
    if (tagSettings.settingsVer == 0xFF) {
        // settings not set. load the defaults
        loadDefaultSettings();
        pr("SETTINGS: Loaded default settings\n");
    } else {
        if (tagSettings.settingsVer < SETTINGS_STRUCT_VERSION) {
            // upgrade
            upgradeSettings();
            pr("SETTINGS: Upgraded from previous version\n");
        } else {
            // settings are valid
            pr("SETTINGS: Loaded from infopage\n");
        }
    }
}

void writeSettings() {
    if (compareSettings()) {
        pr("SETTINGS: Settings matched current settings\n");
        return;
    }
    flashRead(FLASH_INFOPAGE_ADDR, (void*)infopageTempBuffer, 1024);
    xMemCopy((void*)(infopageTempBuffer + INFOPAGE_SETTINGS_OFFSET), (void*)tagSettings, sizeof(tagSettings));
    flashErase(FLASH_INFOPAGE_ADDR + 1);
    flashWrite(FLASH_INFOPAGE_ADDR, (void*)infopageTempBuffer, 1024, false);
    pr("SETTINGS: Updated settings in infopage\n");
}