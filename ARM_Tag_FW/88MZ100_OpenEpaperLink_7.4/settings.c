#include "settings.h"
#include <string.h>
#include "eeprom.h"
#include "util.h"
#include "mz100_flash.h"


#define EEPROM_NUM_SETTINGS_PAGES	(EEPROM_SETTINGS_AREA_LEN / EEPROM_PAGE_SIZE)
#define SETTINGS_MAGIC				(0x31415926)



static uint32_t mCurSettingsAddr;

static void settingsPrvDoWriteAtLocation(uint32_t addr, struct Settings* settings)
{
	settings->hdr.revision++;
	mCurSettingsAddr = addr;
	FLASH_Write(false, addr, (uint8_t*)settings, sizeof(struct Settings));
}

//this is impossible to call before calling read. thus mCurSettingsAddr will be set
void settingsPrvDoWrite(struct Settings* settings)
{
	struct SettingsHeader sh;
	uint32_t i, addr;
	uint8_t byte;
	
	
	//first we try to fit in the current page, after current (latest) settings
	if (mCurSettingsAddr) {
		
		FLASH_Read(0, mCurSettingsAddr, (uint8_t*)&sh, sizeof(struct SettingsHeader));
		addr = mCurSettingsAddr + sh.structSize;
		
		//is there space?
		if (addr % EEPROM_PAGE_SIZE != 0 && addr % EEPROM_PAGE_SIZE + sizeof(struct Settings) <= EEPROM_PAGE_SIZE) {
			
			//is it erased
			for (i = 0; i < sizeof(struct Settings); i++) {
				
				FLASH_Read(0, addr, &byte, 1);
				if (byte != 0xff)
					break;
			}
			
			if (i == sizeof(struct Settings)) {
				
				settingsPrvDoWriteAtLocation(addr, settings);
				return;
			}
		}
	}
	
	//we need to erase - use next page (or 0th page if no current page at all)
	if (mCurSettingsAddr) {
		
		addr = (mCurSettingsAddr + EEPROM_PAGE_SIZE - 1) / EEPROM_PAGE_SIZE * EEPROM_PAGE_SIZE;
		if (addr == EEPROM_SETTINGS_AREA_START + EEPROM_SETTINGS_AREA_LEN)
			addr = EEPROM_SETTINGS_AREA_START;
	}
	else
		addr = EEPROM_SETTINGS_AREA_START;
	
	qspiEraseRange(addr, EEPROM_PAGE_SIZE);
	settingsPrvDoWriteAtLocation(addr, settings);
}

void settingsRead(struct Settings* settings)
{
	uint32_t bestAddr = 0, page, ofst;
	uint64_t bestRevision = 0;
	struct SettingsHeader sh;
	bool doWrite = true;
	
	for (page = 0; page < EEPROM_NUM_SETTINGS_PAGES; page++) {
		
		for (ofst = 0; ofst < EEPROM_PAGE_SIZE - sizeof(struct SettingsHeader); ofst += sh.structSize) {
			
			uint32_t addr = EEPROM_SETTINGS_AREA_START + page * EEPROM_PAGE_SIZE + ofst;
			
			FLASH_Read(0, addr, (uint8_t*)&sh, sizeof(struct SettingsHeader));
			
			//sanity checks. struct is only allowed to grow in size...
			if (sh.magic != SETTINGS_MAGIC || ofst + sh.structSize > EEPROM_PAGE_SIZE || sh.structSize > sizeof(struct Settings))
				break;
			
			if (sh.revision > bestRevision) {
				bestRevision = sh.revision;
				bestAddr = addr;
			}
		}
	}
	
	if (bestAddr) {
		FLASH_Read(0, bestAddr, (uint8_t*)&sh, sizeof(struct SettingsHeader));	//to get size
		FLASH_Read(0, bestAddr, (uint8_t*)settings, sh.structSize);
		mCurSettingsAddr = bestAddr;
	}
	else {
		settings->hdr.structVersion = SETTINGS_VER_NONE;
		settings->hdr.revision = 1;
		mCurSettingsAddr = 0;
	}
	
	//migrate
	switch (settings->hdr.structVersion) {
		
		//current version here - mark as such
		case SETTINGS_CURRENT:
			doWrite = false;
			break;
		
		case SETTINGS_VER_NONE:	//migrate to v1
			memset(settings, 0, sizeof(*settings));
			settings->hdr.magic = SETTINGS_MAGIC;
			//fallthrough
		
		case SETTINGS_VER_1:	//migrate to v2
			settings->prevDlProgress = 0xffff;
			//fallthrough
		
		case SETTINGS_VER_2:	//migrate to v3
			settings->lastRxedRSSI = 0;
			//fallthrough
		
		//new migrations here in order from lower vers to higher vers
		
			settings->hdr.structVersion = SETTINGS_CURRENT;
			settings->hdr.structSize = sizeof(struct Settings);
			break;
	}
	
	if (doWrite)
		settingsPrvDoWrite(settings);
}

void settingsWrite(struct Settings* settings)
{
	struct Settings s;
	
	settingsRead(&s);
	if (memcmp(&s, settings, sizeof(struct Settings)))
		settingsPrvDoWrite(settings);
}