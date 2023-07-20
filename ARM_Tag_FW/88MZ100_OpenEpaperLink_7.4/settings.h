#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <stdint.h>

#define SETTINGS_VER_NONE		(0x00000000)
#define SETTINGS_VER_1			(0x00000001)
#define SETTINGS_VER_2			(0x00000002)
#define SETTINGS_VER_3			(0x00000003)

struct SettingsHeader {
	uint32_t magic;
	uint64_t revision;
	uint8_t structVersion;
	uint8_t structSize;			//incl this header
} __attribute__((packed));

enum SettingsThingType {
	SettingsThingTypeNone,
	SettingsThingTypeImage,
	SettingsThingTypeUpdate,
};

#define SETTING_CHANNEL_OFFSET		11

struct Settings {				//V1
	struct SettingsHeader hdr;
	
	//master address
	uint8_t masterMac[8];
	
	//encryption things
	uint32_t encrKey[4];
	uint32_t nextIV;
	
	//checkin tracking
	uint32_t checkinDelay;					//space between checkins, in msec
	uint32_t retryDelay;
	uint16_t failedCheckinsTillBlank;		//how many fails till we go blank
	uint16_t failedCheckinsTillDissoc;		//how many fails till we dissociate
	uint16_t numFailedCheckins;
	
	//state
	uint8_t lastRxedLQI;
	uint8_t isPaired				: 1;
	uint8_t channel					: 4;	//minus SETTING_CHANNEL_OFFSET
	uint8_t reserved				: 3;
	
	uint16_t prevDlProgress;
	
	int8_t lastRxedRSSI;
	uint32_t helperInit;
	
} __attribute__((packed));

#define SETTINGS_CURRENT		SETTINGS_VER_3


void settingsRead(struct Settings* settings);
void settingsWrite(struct Settings* settings);




#endif
