#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "core_cm3.h"
#include "main.h"
#include "epd.h"
#include "mz100_gpio.h"
#include "mz100_ssp.h"
#include "mz100_pinmux.h"
#include "mz100_clock.h"
#include "mz100_wdt.h"
#include "util.h"
#include "gpio.h"

void epd_reset()
{
	uint8_t v0 = 5;
	while (1)
	{
		GPIO_WritePinOutput(EPD_RESET, GPIO_IO_HIGH);
		delay(100);
		GPIO_WritePinOutput(EPD_RESET, GPIO_IO_LOW);
		delay(3000);
		GPIO_WritePinOutput(EPD_RESET, GPIO_IO_HIGH);
		delay(3000);
		if (GPIO_ReadPinLevel(EPD_BUSY))
			break;
		v0--;
		if (!v0)
		{
			printf("EPD reset failure\r\n");
			break;
		}
	}
	delay(5000);
}

void EPD_cmd(char a1)
{
	unsigned int v1 = 0;

	GPIO_WritePinOutput(EPD_CS, GPIO_IO_LOW);
	GPIO_WritePinOutput(EPD_DC, GPIO_IO_LOW);
	do
	{
		if ((a1 & 0x80) != 0)
			GPIO_WritePinOutput(EPD_MOSI, GPIO_IO_HIGH);
		else
			GPIO_WritePinOutput(EPD_MOSI, GPIO_IO_LOW);
		a1 *= 2;
		v1++;
		delay_us(1);
		GPIO_WritePinOutput(EPD_CLK, GPIO_IO_HIGH);
		delay_us(1);
		GPIO_WritePinOutput(EPD_CLK, GPIO_IO_LOW);
	} while (v1 < 8);
	GPIO_WritePinOutput(EPD_MOSI, GPIO_IO_LOW);
	GPIO_WritePinOutput(EPD_DC, GPIO_IO_HIGH);
	GPIO_WritePinOutput(EPD_CS, GPIO_IO_HIGH);
	delay(1000);
}

void EPD_data(char a1)
{
	unsigned int v1 = 0;

	GPIO_WritePinOutput(EPD_CS, GPIO_IO_LOW);
	do
	{
		if ((a1 & 0x80) != 0)
			GPIO_WritePinOutput(EPD_MOSI, GPIO_IO_HIGH);
		else
			GPIO_WritePinOutput(EPD_MOSI, GPIO_IO_LOW);
		a1 *= 2;
		v1++;
		delay_us(1);
		GPIO_WritePinOutput(EPD_CLK, GPIO_IO_HIGH);
		delay_us(1);
		GPIO_WritePinOutput(EPD_CLK, GPIO_IO_LOW);
	} while (v1 < 8);
	GPIO_WritePinOutput(EPD_MOSI, GPIO_IO_LOW);
	GPIO_WritePinOutput(EPD_CS, GPIO_IO_HIGH);
	delay(1000);
}

void spi_soft_send_byte(char a1)
{
	uint8_t v2 = 0;
	do
	{
		if ((a1 & 0x80) != 0)
			GPIO_WritePinOutput(EPD_MOSI, GPIO_IO_HIGH);
		else
			GPIO_WritePinOutput(EPD_MOSI, GPIO_IO_LOW);
		delay_us(1);
		GPIO_WritePinOutput(EPD_CLK, GPIO_IO_HIGH);
		delay_us(1);
		GPIO_WritePinOutput(EPD_CLK, GPIO_IO_LOW);
		a1 *= 2;
		v2++;
	} while (v2 < 8);
	GPIO_WritePinOutput(EPD_MOSI, GPIO_IO_LOW);
	delay_us(1);
}

void BUSY_wait(unsigned int a1)
{
	unsigned int v2 = 0;
	while (GPIO_ReadPinLevel(EPD_BUSY) == GPIO_IO_LOW)
	{
		delay(10000);
		v2++;
		if (v2 > a1)
			break;
		if (!(20 * (v2 % 1000 / 10)))
			WDT_RestartCounter();
	}
}

void spi_soft_read_buffer(char a1, uint16_t a2, uint8_t *a3, unsigned int a4)
{
	char v9;
	unsigned int v10;

	GPIO_WritePinOutput(EPD_CS, GPIO_IO_HIGH);
	GPIO_WritePinOutput(EPD_HLT_CTRL, GPIO_IO_LOW);
	spi_soft_send_byte(3);
	spi_soft_send_byte(a1);
	spi_soft_send_byte(a2 >> 8);
	spi_soft_send_byte(a2);
	delay_us(10);
	for (int i = 0; i < a4; i = (uint8_t)(i + 1))
	{
		v9 = 0;
		v10 = 0;
		do
		{
			v9 *= 2;
			GPIO_WritePinOutput(EPD_CLK, GPIO_IO_LOW);
			delay_us(5);
			if (GPIO_ReadPinLevel(EPD_MISO))
				v9 |= 1u;
			GPIO_WritePinOutput(EPD_CLK, GPIO_IO_HIGH);
			delay_us(5);
			v10++;
		} while (v10 < 8);
		delay_us(5);
		*a3++ = v9;
	}
	GPIO_WritePinOutput(EPD_CLK, GPIO_IO_LOW);
	GPIO_WritePinOutput(EPD_HLT_CTRL, GPIO_IO_HIGH);
}

int spi_soft_read_byte()
{
	int v0;
	unsigned int v1;

	v0 = 0;
	GPIO_WritePinOutput(EPD_BS, GPIO_IO_HIGH);
	GPIO_WritePinOutput(EPD_CS, GPIO_IO_LOW);
	delay_us(1);
	GPIO_WritePinOutput(EPD_MOSI, GPIO_IO_HIGH);
	GPIO_WritePinOutput(EPD_CLK, GPIO_IO_HIGH);
	delay_us(1);
	GPIO_WritePinOutput(EPD_CLK, GPIO_IO_LOW);
	delay_us(1);
	GPIO_SetPinDir(EPD_MOSI, GPIO_INPUT);
	GPIO_PinMuxFun(EPD_MOSI, 0);
	delay_us(3);
	v1 = 0;
	do
	{
		v0 = (uint8_t)(2 * v0);
		GPIO_WritePinOutput(EPD_CLK, GPIO_IO_HIGH);
		if (GPIO_ReadPinLevel(EPD_MOSI))
			v0 |= 1u;
		GPIO_WritePinOutput(EPD_CLK, GPIO_IO_LOW);
		delay_us(1);
		v1++;
	} while (v1 < 8);
	GPIO_SetPinDir(EPD_MOSI, GPIO_OUTPUT);
	GPIO_PinMuxFun(EPD_MOSI, 0);
	GPIO_WritePinOutput(EPD_MOSI, GPIO_IO_LOW);
	delay_us(1);
	GPIO_WritePinOutput(EPD_CS, GPIO_IO_HIGH);
	delay_us(1);
	GPIO_WritePinOutput(EPD_BS, GPIO_IO_LOW);
	return v0;
}

void epd_read_write_30()
{
	uint8_t v0;
	uint8_t v1;
	uint8_t v4;
	uint8_t v5[12];
	uint8_t v6[12];
	uint8_t v7[40];

	EPD_cmd(101);
	EPD_data(1);
	delay_us(1000);
	spi_soft_read_buffer(0, 25002, v7, 10);
	delay_us(1000);
	spi_soft_read_buffer(0, 25039, v6, 10);
	delay_us(1000);
	EPD_cmd(101);
	EPD_data(0);
	EPD_cmd(64);
	BUSY_wait(0xAu);
	v0 = spi_soft_read_byte();
	v1 = (uint8_t)(2 * v0) + ((uint8_t)spi_soft_read_byte() >> 7);
	for (int i = 0; i < 9; i++)
		v5[i] = (((char)v1 - (uint8_t)v7[i]) & 0x80) != 0;
	for (int j = 0; j < 9; j++)
	{
		v4 = v6[j];
		if (v5[j] == 1)
			break;
	}
	EPD_cmd(0x30);
	EPD_data(v4);
}

void epd_read_write_82()
{
	uint8_t v0;
	int v1;
	uint8_t v4;
	uint8_t v5[12];
	uint8_t v6[12];
	uint8_t v7[32];

	EPD_cmd(101);
	EPD_data(1);
	delay_us(1000);
	spi_soft_read_buffer(0, 25002, v7, 10);
	spi_soft_read_buffer(0, 25049, v6, 10);
	delay_us(1000);
	EPD_cmd(101);
	EPD_data(0);
	EPD_cmd(0x40);
	BUSY_wait(0xAu);
	v0 = spi_soft_read_byte();
	v1 = (char)(2 * v0 + ((unsigned int)spi_soft_read_byte() >> 7));
	for (int i = 0; i < 9; i++)
		v5[i] = ((v1 - (uint8_t)v7[i]) & 0x80) != 0;
	for (int j = 0; j < 9; j++)
	{
		v4 = v6[j];
		if (v5[j] == 1)
			break;
	}
	EPD_cmd(0x82);
	EPD_data(v4);
}
void epd_send_init()
{
	GPIO_WritePinOutput(EPD_CS, GPIO_IO_HIGH);
	delay(1000);
	EPD_cmd(4);
	BUSY_wait(0x32u);
	EPD_cmd(1);
	EPD_data(55);
	EPD_data(0);
	EPD_data(5);
	EPD_data(5);
	EPD_cmd(0);
	EPD_data(203);
	EPD_data(8);
	EPD_cmd(229);
	EPD_data(3);
	EPD_cmd(3);
	EPD_data(0);
	EPD_cmd(6);
	EPD_data(199);
	EPD_data(204);
	EPD_data(45);
	EPD_cmd(48);
	EPD_data(60);
	EPD_cmd(65);
	EPD_data(0);
	EPD_cmd(80);
	EPD_data(119);
	EPD_cmd(96);
	EPD_data(34);
	EPD_cmd(97);
	EPD_data(2);
	EPD_data(128);
	EPD_data(1);
	EPD_data(128);
	epd_read_write_82();
	EPD_cmd(2);
	BUSY_wait(0x32u);
}

void init_GPIO_EPD()
{
	SSP_CFG_Type v0;
	SPI_Param_Type spiParaStruct;
	GPIO_PinMuxFun(EPD_MOSI, 0);
	GPIO_SetPinDir(EPD_MOSI, GPIO_OUTPUT);
	GPIO_PinMuxFun(EPD_CLK, 0);
	GPIO_SetPinDir(EPD_CLK, GPIO_OUTPUT);
	GPIO_PinMuxFun(EPD_CS, 0);
	GPIO_SetPinDir(EPD_CS, GPIO_OUTPUT);
	GPIO_WritePinOutput(EPD_CS, GPIO_IO_HIGH);
	GPIO_PinMuxFun(EPD_BUSY, 0);
	GPIO_SetPinDir(EPD_BUSY, GPIO_INPUT);
	GPIO_PinModeConfig(EPD_BUSY, PINMODE_PULLUP);
	GPIO_PinMuxFun(EPD_RESET, 0);
	GPIO_SetPinDir(EPD_RESET, GPIO_OUTPUT);
	GPIO_WritePinOutput(EPD_RESET, GPIO_IO_HIGH);
	GPIO_PinMuxFun(EPD_DC, 0);
	GPIO_SetPinDir(EPD_DC, GPIO_OUTPUT);
	GPIO_WritePinOutput(EPD_DC, GPIO_IO_HIGH);
	GPIO_PinMuxFun(EPD_BS, 0);
	GPIO_SetPinDir(EPD_BS, GPIO_OUTPUT);
	GPIO_WritePinOutput(EPD_BS, GPIO_IO_LOW);
	GPIO_PinMuxFun(EPD_HLT_CTRL, 0);
	GPIO_SetPinDir(EPD_HLT_CTRL, GPIO_OUTPUT);
	GPIO_WritePinOutput(EPD_HLT_CTRL, GPIO_IO_HIGH);
	GPIO_PinMuxFun(EPD_MISO, 0);
	GPIO_SetPinDir(EPD_MISO, GPIO_INPUT);
	GPIO_PinModeConfig(EPD_MISO, PINMODE_DEFAULT);
	memset(&v0, 0, 9);
	v0.timeOutVal = 0;
	SSP_Init(SSP2_ID, &v0);
	spiParaStruct.spiClkPhase = SPI_SCPHA_1;
	spiParaStruct.spiClkPolarity = SPI_SCPOL_LOW;
	SPI_Config(SSP2_ID, &spiParaStruct);
	CLK_I2SClkSrc(CLK_I2S_XTAL32M);
	CLK_SSPClkSrc(CLK_SSP_ID_2, CLK_SSP_I2S);
	CLK_I2SClkDivider(1, 1);
}

void epd_refresh_and_sleep()
{
	EPD_cmd(0x12);
	delay(100000);
	do_sleeped_epd_refresh();
	init_GPIO_EPD();
	epd_reset();
	epd_reset();
	EPD_cmd(1);
	EPD_data(2);
	EPD_data(0);
	EPD_data(0);
	EPD_data(0);
	delay_us(500000);
	EPD_cmd(2);
	delay_us(1000000);
	BUSY_wait(0x32u);
	EPD_cmd(0x65);
	EPD_data(1);
	GPIO_WritePinOutput(EPD_HLT_CTRL, GPIO_IO_LOW);
	spi_soft_send_byte(0xB9);
	GPIO_WritePinOutput(EPD_HLT_CTRL, GPIO_IO_HIGH);
	EPD_cmd(0x65);
	EPD_data(0);
	EPD_cmd(7);
	EPD_data(0xA5);
}

void epd_pin_enable(int a1)
{
	if (a1)
	{
		GPIO_PinMuxFun(EPD_CLK, GPIO22_SSP2_SCK);
		GPIO_PinMuxFun(EPD_MOSI, GPIO12_SSP2_TXD);
		GPIO_PinMuxFun(EPD_MISO, GPIO13_SSP2_RXD);
		SSP_Enable(SSP2_ID);
	}
	else
	{
		SSP_Disable(SSP2_ID);
		GPIO_PinMuxFun(EPD_MOSI, 0);
		GPIO_SetPinDir(EPD_MOSI, GPIO_OUTPUT);
		GPIO_PinMuxFun(EPD_CLK, 0);
		GPIO_SetPinDir(EPD_CLK, GPIO_OUTPUT);
		GPIO_PinMuxFun(EPD_MISO, 0);
		GPIO_SetPinDir(EPD_MISO, GPIO_INPUT);
		GPIO_PinModeConfig(EPD_MISO, PINMODE_DEFAULT);
	}
}

void display_tx_byte(uint8_t data)
{
	SSP_SendData(SSP2_ID, data);
	for (int i = 0; i < 0xF; ++i)
		__ISB();
}

void display_send_start(uint8_t inverted)
{
	EPD_cmd(0);
	if(inverted)
	EPD_data(207);
	else
	EPD_data(203);
	EPD_cmd(0x10);
	epd_pin_enable(1);
	GPIO_WritePinOutput(EPD_CS, GPIO_IO_LOW);
}

void display_send_stop()
{
	GPIO_WritePinOutput(EPD_CS, GPIO_IO_HIGH);
	epd_pin_enable(0);
}

void init_epd(void)
{
	init_GPIO_EPD();
	epd_reset();
	epd_reset();
	EPD_cmd(0x65);
	EPD_data(1);
	GPIO_WritePinOutput(EPD_HLT_CTRL, GPIO_IO_LOW);
	spi_soft_send_byte(0xAB);
	GPIO_WritePinOutput(EPD_HLT_CTRL, GPIO_IO_HIGH);
	EPD_cmd(0x65);
	EPD_data(0);
	delay(1000);
	epd_send_init();
	delay(1000);
	EPD_cmd(4);
	BUSY_wait(0x32u);
	epd_read_write_30();
}
