#include "tl_common.h"
#include "main.h"
#include "led.h"
#include "epd.h"
#include "uart.h"
#include "zigbee.h"
#include "nfc.h"

#include "proto.h"
#include "syncedproto.h"
#include "powermgt.h"
#include "comms.h"
#include "drawing.h"
#include "wdt.h"
#include "battery.h"

extern uint8_t our_ch;
RAM u8 ourmac[8] = {0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x02, 0x00, 0x00};
extern RAM uint8_t mSelfMac[8];
RAM char ownMacString[100] = "";
RAM bool noApShown = false;

uint8_t showChannelSelect()
{ // returns 0 if no accesspoints were found
	uint8_t result[sizeof(channelList)];
	memset(result, 0, sizeof(result));
	for (uint8_t i = 0; i < 4; i++)
	{
		for (uint8_t c = 0; c < sizeof(channelList); c++)
		{
			if (detectAP(channelList[c]))
			{
				if (mLastLqi > result[c])
					result[c] = mLastLqi;
				printf("Channel: %d - LQI: %d RSSI %d\r\n", channelList[c], mLastLqi, mLastRSSI);
			}
		}
	}

	uint8_t highestLqi = 0;
	uint8_t highestSlot = 0;
	for (uint8_t c = 0; c < sizeof(result); c++)
	{
		if (result[c] > highestLqi)
		{
			highestSlot = channelList[c];
			highestLqi = result[c];
		}
	}
	mLastLqi = highestLqi;
	return highestSlot;
}

uint8_t channelSelect()
{ // returns 0 if no accesspoints were found
	uint8_t result[16];
	memset(result, 0, sizeof(result));

	for (uint8_t i = 0; i < 2; i++)
	{
		for (uint8_t c = 0; c < sizeof(channelList); c++)
		{
			if (detectAP(channelList[c]))
			{
				if (mLastLqi > result[c])
					result[c] = mLastLqi;
			}
		}
	}

	uint8_t highestLqi = 0;
	uint8_t highestSlot = 0;
	for (uint8_t c = 0; c < sizeof(result); c++)
	{
		if (result[c] > highestLqi)
		{
			highestSlot = channelList[c];
			highestLqi = result[c];
		}
	}

	mLastLqi = highestLqi;
	return highestSlot;
}

int main(void)
{
	startup_state_e state = drv_platform_init();
	u8 isRetention = (state == SYSTEM_DEEP_RETENTION) ? 1 : 0;
	isRetention = 0; // keep the compiler happy
	noApShown = false;
	drv_enable_irq();
	init_led();
	init_uart();
	init_nfc();

	printf("\r\n\r\n\r\nSTARTING\r\n\r\n\r\n\r\n");

	u32 flash_mid = 0;
	u8 flash_uid[16] = {0};
	printf("Reading UID");
	int flag = flash_read_mid_uid_with_check(&flash_mid, flash_uid);
	if (flag)
	{
		for (int i = 0; i < 16; i++)
		{
			printf(" %02X", flash_uid[i]);
		}
		printf("\r\n");
		for (int i = 0; i < 8; i++)
		{
			mSelfMac[i] = flash_uid[i + 8];
		}
	}
	else
	{
		printf(" - Failed!\r\n");
	}

	sprintf(ownMacString, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);

	batteryVoltage = get_battery_mv();

	printf("Battery mv: %d Millis: %d\r\n", batteryVoltage, getMillis());

	// epd_display("Boot", batteryVoltage, ownMacString, 1);

	initializeProto();

	currentChannel = showChannelSelect();

	wdt10s();

	if (currentChannel)
	{
		our_ch = currentChannel;
		printf("AP Found\r\n");
		epd_display("AP Found", batteryVoltage, ownMacString, 1);
		initPowerSaving(INTERVAL_BASE);
		doSleep(5000UL);
	}
	else
	{
		printf("No AP found\r\n");
		epd_display("No AP Found", batteryVoltage, ownMacString, 1);
		initPowerSaving(INTERVAL_AT_MAX_ATTEMPTS);
		noApShown = true;
		doSleep(120000UL);
	}
	while (1)
	{
		batteryVoltage = get_battery_mv();
		uint32_t before = getMillis();
		printf("Battery mv: %d Millis before: %d Uptime: %d\r\n", batteryVoltage, before, (getMillis() / 1000) / 60);
		wdt10s();
		if (currentChannel)
		{
			struct AvailDataInfo *avail;
			avail = getAvailDataInfo();
			// avail = getShortAvailDataInfo();
			addAverageValue();

			if (avail == NULL)
			{
				set_led_color(1);
				printf("No data\r\n");
				set_led_color(0);
				// no data :(
				nextCheckInFromAP = 0; // let the power-saving algorithm determine the next sleep period
			}
			else
			{
				nextCheckInFromAP = avail->nextCheckIn;
				// got some data from the AP!
				if (avail->dataType != DATATYPE_NOUPDATE)
				{
					set_led_color(2);
					printf("Got data transfer\r\n");
					set_led_color(0);
					// data transfer
					if (processAvailDataInfo(avail))
					{
						// succesful transfer, next wake time is determined by the NextCheckin;
					}
					else
					{
						// failed transfer, let the algorithm determine next sleep interval (not the AP)
						nextCheckInFromAP = 0;
					}
				}
				else
				{
					set_led_color(3);
					printf("No data transfer\r\n");
					set_led_color(0);
					wakeUpReason = WAKEUP_REASON_TIMED; // Only one successfully AP communication we can have timed wakeups
														// no data transfer, just sleep.
				}
			}

			uint16_t nextCheckin = getNextSleep();
			longDataReqCounter += nextCheckin;
			if (nextCheckin == INTERVAL_AT_MAX_ATTEMPTS)
			{
				// disconnected, obviously...
				currentChannel = 0;
			}

			doSleep(40 * 1000UL);
			/*// if the AP told us to sleep for a specific period, do so.
			if (nextCheckInFromAP)
			{
				doSleep(nextCheckInFromAP * 60000UL);
			}
			else
			{
				doSleep(getNextSleep() * 1000UL);
			}*/
		}
		else
		{
			set_led_color(3);
			// We sacrifice 10ms here to show a basic LED status as the scan itself takes more than a second
			WaitMs(10);
			set_led_color(0);
			currentChannel = channelSelect();

			if (!currentChannel)
			{
				wdt60s();
				if (!noApShown)
				{
					noApShown = true;
					if (curImgSlot != 0xFF)
					{
						drawOnOffline(0);
						drawImageFromEeprom(curImgSlot);
						drawOnOffline(1);
					}
					else
					{
						epd_display("No AP", batteryVoltage, ownMacString, 1);
					}
				}
			}
			// did we find a working channel?
			if (currentChannel)
			{
				// now associated!
				noApShown = false;
				our_ch = currentChannel;
				printf("AP Found\r\n");
				if (curImgSlot != 0xFF)
				{
					drawOnOffline(1);
					drawImageFromEeprom(curImgSlot);
				}
				else
				{
					epd_display("AP Found", batteryVoltage, ownMacString, 1);
				}
				scanAttempts = 0;
				wakeUpReason = WAKEUP_REASON_NETWORK_SCAN;
				initPowerSaving(INTERVAL_BASE);
				doSleep(40 * 1000UL);
			}
			else
			{
				// still not associated
				doSleep(15 * 60 * 1000UL);
			}
		}
	}
	return 0;
}
