/********************************************************************************************************
 * @file    drv_spi.c
 *
 * @brief   This is the source file for drv_spi
 *
 * @author  Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/

#include "../tl_common.h"

#if defined(MCU_CORE_B91)
pspi_csn_pin_def_e drv_cs_pin;
#endif

/**
 * @brief     This function configures the clock and working mode for SPI interface
 * @param[in] spiClock - SPI module
 *            SPI clock = System clock / ((DivClock+1)*2)
 * @param[in] mode - the selected working mode of SPI module,mode 0~mode 3
 * @return    none
 */
void drv_spi_master_init(u32 spiClock, drv_spi_mode_type_def mode)
{
	u8 divClock = ((2 * spiClock) >= SPI_CLOCK_SOURCE) ? 0 : (u8)(SPI_CLOCK_SOURCE / (2 * spiClock) - 1);

#if	defined(MCU_CORE_826x)
	SPI_MasterInit(divClock, mode);
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	spi_master_init(divClock, mode);
#elif defined(MCU_CORE_B91)
	/* B91 supports normal SPI mode and SPI Plus mode,
	 * here we use the commonly used normal mode.
	 */
	spi_master_init(PSPI_MODULE, divClock, mode);
	spi_master_config(PSPI_MODULE, SPI_NOMAL);
#endif
}

/**
 * @brief     This function configures the clock and working mode for SPI interface
 * @param[in] mode - the selected working mode of SPI module,mode 0~mode 3
 * @return    none
 */
void drv_spi_slave_init(drv_spi_mode_type_def mode)
{
#if	defined(MCU_CORE_826x)
	SPI_SlaveInit(0, mode);
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	spi_slave_init(0, mode);
#elif defined(MCU_CORE_B91)
	spi_slave_init(PSPI_MODULE, mode);
#endif
}

/**
 * @brief     This function selects a pin port for the SPI master interface
 * @param[in] Pin Group or Pins
 * @return    none
 */
#if	defined(MCU_CORE_826x)
void drv_spi_master_pin_select(SPI_PinTypeDef pinGroup)
{
	SPI_MasterPinSelect(pinGroup);
}
#elif defined(MCU_CORE_8258)
void drv_spi_master_pin_select(SPI_GPIO_GroupTypeDef pinGroup)
{
	spi_master_gpio_set(pinGroup);
}
#elif defined(MCU_CORE_8278)
void drv_spi_master_pin_select(SPI_GPIO_SclkTypeDef sclk_pin, SPI_GPIO_CsTypeDef cs_pin, SPI_GPIO_SdoTypeDef sdo_pin, SPI_GPIO_SdiTypeDef sdi_pin)
{
	spi_master_gpio_set(sclk_pin, cs_pin, sdo_pin, sdi_pin);
}
#elif defined(MCU_CORE_B91)
void drv_spi_master_pin_select(pspi_clk_pin_def_e sclk_pin, pspi_csn_pin_def_e cs_pin, pspi_mosi_io0_pin_def_e mosi_pin, pspi_miso_io1_pin_def_e miso_pin)
{
	pspi_pin_config_t pspi_pin_config;

	pspi_pin_config.pspi_clk_pin = sclk_pin;
	pspi_pin_config.pspi_csn_pin = cs_pin;
	pspi_pin_config.pspi_mosi_io0_pin = mosi_pin;
	pspi_pin_config.pspi_miso_io1_pin = miso_pin;

	drv_cs_pin = cs_pin;

	pspi_set_pin(&pspi_pin_config);
}
#endif

/**
 * @brief     This function selects a pin port for the SPI slave interface
 * @param[in] Pin Group or Pins
 * @return    none
 */
#if	defined(MCU_CORE_826x)
void drv_spi_slave_pin_select(SPI_PinTypeDef pinGroup)
{
	SPI_SlavePinSelect(pinGroup);
}
#elif defined(MCU_CORE_8258)
void drv_spi_slave_pin_select(SPI_GPIO_GroupTypeDef pinGroup)
{
	spi_slave_gpio_set(pinGroup);
}
#elif defined(MCU_CORE_8278)
void drv_spi_slave_pin_select(SPI_GPIO_SclkTypeDef sclk_pin, SPI_GPIO_CsTypeDef cs_pin, SPI_GPIO_SdoTypeDef sdo_pin, SPI_GPIO_SdiTypeDef sdi_pin)
{
	spi_slave_gpio_set(sclk_pin, cs_pin, sdo_pin, sdi_pin);
}
#elif defined(MCU_CORE_B91)
void drv_spi_slave_pin_select(pspi_clk_pin_def_e sclk_pin, pspi_csn_pin_def_e cs_pin, pspi_mosi_io0_pin_def_e mosi_pin, pspi_miso_io1_pin_def_e miso_pin)
{
	pspi_pin_config_t pspi_pin_config;

	pspi_pin_config.pspi_clk_pin = sclk_pin;
	pspi_pin_config.pspi_csn_pin = cs_pin;
	pspi_pin_config.pspi_mosi_io0_pin = mosi_pin;
	pspi_pin_config.pspi_miso_io1_pin = miso_pin;

	drv_cs_pin = cs_pin;

	pspi_set_pin(&pspi_pin_config);
}
#endif

/**
 * @brief      This function serves to write a bulk of data to the SPI slave
 *             device specified by the CS pin
 * @param[in]  cmd - pointer to the command bytes needed written into the
 *             slave device first before the writing operation of actual data
 * @param[in]  cmdLen - length in byte of the command bytes
 * @param[in]  data - pointer to the data need to write
 * @param[in]  dataLen - length in byte of the data need to write
 * @param[in]  csPin - the CS pin specifing the slave device
 * @return     none
 */
void drv_spi_write(u8 *cmd, int cmdLen, u8 *data, int dataLen, u32 csPin)
{
#if	defined(MCU_CORE_826x)
	SPI_Write(cmd, cmdLen, data, dataLen, csPin);
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	spi_write(cmd, cmdLen, data, dataLen, csPin);
#elif defined(MCU_CORE_B91)
	u8 *pBuf = (u8 *)ev_buf_allocate(cmdLen + dataLen);
	if(pBuf){
		if(drv_cs_pin != csPin){
			pspi_cs_pin_dis(drv_cs_pin);
			pspi_cs_pin_en(csPin);
			drv_cs_pin = csPin;
		}

		u8 *pData = pBuf;

		memcpy(pData, cmd, cmdLen);
		pData += cmdLen;
		memcpy(pData, data, dataLen);
		pData += dataLen;

		spi_master_write(PSPI_MODULE, pBuf, cmdLen + dataLen);

		ev_buf_free(pBuf);
	}
#endif
}


/**
 * @brief      This function serves to read a bulk of data from the SPI slave
 *             device specified by the CS pin
 * @param[in]  cmd - pointer to the command bytes needed written into the
 *             slave device first before the reading operation of actual data
 * @param[in]  cmdLen - length in byte of the command bytes
 * @param[out] data - pointer to the buffer that will cache the reading out data
 * @param[in]  dataLen - length in byte of the data need to read
 * @param[in]  csPin - the CS pin specifing the slave device
 * @return     none
 */
void drv_spi_read(u8 *cmd, int cmdLen, u8 *data, int dataLen, u32 csPin)
{
#if	defined(MCU_CORE_826x)
	SPI_Read(cmd, cmdLen, data, dataLen, csPin);
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	spi_read(cmd, cmdLen, data, dataLen, csPin);
#elif defined(MCU_CORE_B91)
	if(drv_cs_pin != csPin){
		pspi_cs_pin_dis(drv_cs_pin);
		pspi_cs_pin_en(csPin);
		drv_cs_pin = csPin;
	}
	spi_master_write_read(PSPI_MODULE, cmd, cmdLen, data, dataLen);
#endif
}

