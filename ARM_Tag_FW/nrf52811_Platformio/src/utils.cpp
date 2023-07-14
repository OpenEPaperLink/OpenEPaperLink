#include <Arduino.h>
#include "main.h"
#include "utils.h"

int8_t startHFCLK(void)
{
	if (!isHFCLKstable())
	{
		NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
		NRF_CLOCK->TASKS_HFCLKSTART = 1U;

		while (!isHFCLKstable())
			;

		return 0;
	}
	else
	{
		return -1;
	}
}

uint8_t isHFCLKstable(void)
{
	uint32_t stable = (uint32_t)(CLOCK_HFCLKSTAT_STATE_Running << CLOCK_HFCLKSTAT_STATE_Pos) | (CLOCK_HFCLKSTAT_SRC_Xtal << CLOCK_HFCLKSTAT_SRC_Pos);

	if (NRF_CLOCK->HFCLKSTAT == stable)
	{
		NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;

		return 1;
	}
	else
	{
		return 0;
	}
}

void boardGetOwnMac(uint8_t *mac)
{
	mac[0] = (NRF_FICR->DEVICEADDR[0]) & 0xFF;
	mac[1] = (NRF_FICR->DEVICEADDR[0] >> 8) & 0xFF;
	mac[2] = (NRF_FICR->DEVICEADDR[0] >> 16) & 0xFF;
	mac[3] = (NRF_FICR->DEVICEADDR[0] >> 24);
	mac[4] = (NRF_FICR->DEVICEADDR[1]) & 0xFF;
	mac[5] = (NRF_FICR->DEVICEADDR[1] >> 8) & 0xFF;
	mac[6] = 0;
	mac[7] = 0;
}