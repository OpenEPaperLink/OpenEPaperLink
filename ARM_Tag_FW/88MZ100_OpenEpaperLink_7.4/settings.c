#include "settings.h"
#include <string.h>
#include "eeprom.h"
#include "util.h"
#include "mz100_flash.h"
#include "powermgt.h"

 __attribute__((section(".aonshadow"))) struct tagsettings tagSettings = {0};
extern uint8_t  blockXferBuffer[];
uint8_t*  infopageTempBuffer = 1024 + blockXferBuffer;


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