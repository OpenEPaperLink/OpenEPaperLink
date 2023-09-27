#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdint.h>

#define FW_VERSION 22             // version number (max 2.5.5 :) )
#define FW_VERSION_SUFFIX "-BUT"  // suffix, like -RC1 or whatever.
// #define DEBUGBLOCKS                 // uncomment to enable extra debug information on the block transfers
// #define PRINT_LUT                   // uncomment if you want the tag to print the LUT for the current temperature bracket
#define ENABLE_GPIO_WAKE            // uncomment to enable GPIO wake
// #define ENABLE_RETURN_DATA  // enables the tag to send blocks of data back. Enabling this costs about 4 IRAM bytes

#define SETTINGS_STRUCT_VERSION 0x01

#define DEFAULT_SETTING_FASTBOOT 0
#define DEFAULT_SETTING_RFWAKE 0
#define DEFAULT_SETTING_TAGROAMING 0
#define DEFAULT_SETTING_SCANFORAP 1
#define DEFAULT_SETTING_LOWBATSYMBOL 1
#define DEFAULT_SETTING_NORFSYMBOL 1

/*
struct tagsettings {
    uint8_t settingsVer;                  // the version of the struct as written to the infopage
    uint8_t enableFastBoot;               // default 0; if set, it will skip splashscreen
    uint8_t enableRFWake;                 // default 0; if set, it will enable RF wake. This will add about ~0.9µA idle power consumption
    uint8_t enableTagRoaming;             // default 0; if set, the tag will scan for an accesspoint every few check-ins. This will increase power consumption quite a bit
    uint8_t enableScanForAPAfterTimeout;  // default 1; if a the tag failed to check in, after a few attempts it will try to find a an AP on other channels
    uint8_t enableLowBatSymbol;           // default 1; tag will show 'low battery' icon on screen if the battery is depleted
    uint8_t enableNoRFSymbol;             // default 1; tag will show 'no signal' icon on screen if it failed to check in for a longer period of time
    uint8_t fastBootCapabilities;         // holds the byte with 'capabilities' as detected during a normal tag boot; allows the tag to skip detecting buttons and NFC chip
    uint8_t customMode;                   // default 0; if anything else, tag will bootup in a different 'mode'
    uint16_t batLowVoltage;               // Low battery threshold voltage (2450 for 2.45v). defaults to BATTERY_VOLTAGE_MINIMUM from powermgt.h
    uint16_t minimumCheckInTime;          // defaults to BASE_INTERVAL from powermgt.h
    uint8_t fixedChannel;                 // default 0; if set to a valid channel number, the tag will stick to that channel
} __packed;
*/

extern struct tagsettings tagSettings;

void loadDefaultSettings();
void writeSettings();
void loadSettings();
void loadSettingsFromBuffer(uint8_t* p);
#endif