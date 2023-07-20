#include <stdarg.h>
#include <stdio.h>
#include "eeprom.h"
#include "timer.h"
#include "mz100.h"
#include "util.h"
#include "mz100_flash.h"
#include "mz100_adc.h"
#include "mz100_wdt.h"

void delay(int cnt)
{
	volatile unsigned int i;
	for (i = 107 * cnt; i; --i)
		;
}

void delay_us(unsigned int result)
{
	volatile unsigned int i;

	for (i = 0; i < result; ++i)
		;
}

uint16_t crc16(uint16_t cur_crc, uint8_t data)
{
	cur_crc ^= data;
	for (uint8_t i = 8; i > 0; i--)
	{
		if ((cur_crc & 0x001) != 0)
		{
			cur_crc >>= 1;
			cur_crc ^= 0x8005; // poly
		}
		else
		{
			cur_crc >>= 1;
		}
	}
	return cur_crc;
}

uint32_t measureTemp(void)
{
	uint32_t result = 0;
	ADC_CFG_Type adc_config;
	adc_config.adcResolution = ADC_RESOLUTION_14BIT;
	adc_config.adcVrefSource = ADC_VREF_INTERNAL; // 1.2V
	adc_config.adcGainSel = ADC_GAIN_1;
	adc_config.adcClockDivider = ADC_CLOCK_DIVIDER_4;
	adc_config.adcBiasMode = ADC_BIAS_FULL;

	ADC_Reset();
	ADC_ModeSelect(ADC_MODE_TSENSOR);
	ADC_TSensorConfig(ADC_TEMPP, ADC_SENSOR_INTERNAL);
	ADC_Init(&adc_config);
	ADC_Enable();
	for (int i = 0; i < 32; i++)
	{
		ADC_ConversionStart();
		ADC_IntClr(ADC_RDY);
		while (!ADC_GetStatus(ADC_STATUS_RDY))
			;
		ADC_ConversionStop();
	}
	for (int i = 0; i < 128; i++)
	{
		ADC_ConversionStart();
		ADC_IntClr(ADC_RDY);
		while (!ADC_GetStatus(ADC_STATUS_RDY))
			;
		ADC_ConversionStop();
		result += (ADC_GetConversionResult() - 458) / 1.7;
	}
	result /= 128;
	printf("Temp: %iC\r\n", result);
	return result;
}

uint32_t measureBattery(void)
{
	uint32_t result = 0;
	ADC_CFG_Type adc_config;
	adc_config.adcResolution = ADC_RESOLUTION_16BIT;
	adc_config.adcVrefSource = ADC_VREF_VCAU; // 1.8V
	adc_config.adcGainSel = ADC_GAIN_1;		  // range 0 - 1.8V
	adc_config.adcClockDivider = ADC_CLOCK_DIVIDER_4;
	adc_config.adcBiasMode = ADC_BIAS_FULL;

	ADC_Reset();
	ADC_ModeSelect(ADC_MODE_ADC);
	ADC_ChannelConfig(ADC_VBATS); // 0.33 of Actual Voltage
	ADC_Init(&adc_config);
	ADC_Enable();
	ADC_ConversionStart();
	ADC_IntClr(ADC_RDY);
	for (int i = 0; i < 32; i++)
	{
		ADC_ConversionStart();
		ADC_IntClr(ADC_RDY);
		while (!ADC_GetStatus(ADC_STATUS_RDY))
			;
		ADC_ConversionStop();
	}
	for (int i = 0; i < 128; i++)
	{
		ADC_ConversionStart();
		ADC_IntClr(ADC_RDY);
		while (!ADC_GetStatus(ADC_STATUS_RDY))
			;
		ADC_ConversionStop();
		result += ADC_GetConversionResult() * 5940 / 32768;
	}
	result /= 128;
	printf("Voltage: %imV\r\n", result);
	return result;
}

void qspiEraseRange(uint32_t addr, uint32_t len)
{
	uint64_t time;
	// round starting address down
	if (addr % EEPROM_PAGE_SIZE)
	{
		len += addr % EEPROM_PAGE_SIZE;
		addr = addr / EEPROM_PAGE_SIZE * EEPROM_PAGE_SIZE;
	}

	// round length up
	len = (len + EEPROM_PAGE_SIZE - 1) / EEPROM_PAGE_SIZE * EEPROM_PAGE_SIZE;

	while (len)
	{

		uint32_t now;
		bool ok;

		WDT_RestartCounter();
		if (!(addr % 0x10000) && len >= 0x10000)
		{
			ok = FLASH_Block64KErase(addr / 0x10000);
			now = 0x10000;
		}
		else if (!(addr % 0x8000) && len >= 0x8000)
		{
			ok = FLASH_Block32KErase(addr / 0x8000);
			now = 0x8000;
		}
		else
		{
			ok = FLASH_SectorErase(addr / 0x1000);
			now = 0x1000;
		}

		if (!ok)
			printf("ERZ fail at 0x%08x + %u\r\n", addr, now);

		addr += now;
		len -= now;
		if (len)
		{
			// let the caps recharge
			time = timerGet();
			while (timerGet() - time < TIMER_TICKS_PER_SEC / 10)
				;
		}
	}
	WDT_RestartCounter();
}

bool eepromWrite(uint32_t addr, const void *srcP, uint16_t len)
{
	FLASH_Write(false, addr, srcP, len);
	return true;
}

bool eepromErase(uint32_t addr, uint16_t nSec)
{
	qspiEraseRange(addr, nSec);
	return true;
}

void eepromRead(uint32_t addr, void *dstP, uint16_t len)
{
	uint8_t *dst = (uint8_t *)dstP;
	FLASH_Read(0, addr, dst, len);
}
uint32_t eepromGetSize(void)
{
	return EEPROM_IMG_LEN;
}

void radioShutdown(void)
{
	// i have no idea what these do, determined by writing random registers and watching the current drawn
	*(volatile uint32_t *)0x4C000000 = 0;
	*(volatile uint32_t *)0x4C010000 = 0;
	*(volatile uint32_t *)0x4C010004 = 0x10000000;
}