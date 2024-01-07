/********************************************************************************************************
 * @file    i2c.h
 *
 * @brief   This is the header file for B86
 *
 * @author  Driver & Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *			All rights reserved.
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

#ifndef I2C_H
#define I2C_H

#include "register.h"
#include "gpio.h"

/**
 *  @brief  Define I2C pin port
 */
typedef enum {
    I2C_PIN_GPIOA = 0,
    I2C_PIN_GPIOB,
    I2C_PIN_GPIOC,
} I2C_GPIO_GroupTypeDef;

/**
 *  @brief when device as I2C slave
 *         select slave mode:1.DMA;2.MAPPING
 */
typedef enum {
	I2C_SLAVE_DMA = 0,
	I2C_SLAVE_MAP,
}I2C_SlaveMode;



/**
 * @brief      This function selects a pin port for I2C interface.
 * @param[in]  PinGrp - the pin port selected as I2C interface pin port.
 * @return     none
 */
extern void I2C_PinSelect(I2C_GPIO_GroupTypeDef PinGrp);

/**
 * @brief This function serves to set id of I2C module.
 * @param[in] SlaveID - this id is fixed id for slave device.For master device, this id is set to access different slave devices.
 * @return none
 */
extern void I2C_SetId(unsigned char SlaveID);

/**
 * @brief      This function set the id of slave device and the speed of I2C interface
 * @param[in]  DivClock - the division factor of I2C clock, 
 *             I2C clock = System clock / (4*DivClock);if the datasheet you look at is 2*,pls modify it.
 * @return     none
 */
extern void I2C_MasterInit(unsigned char DivClock);

/**
 *	@brief      the function config the ID of slave and mode of slave.
 *	@param[in]  device_ID - it contains write or read bit,the lsb is write or read bit.
 *			    ID|0x01 indicate read. ID&0xfe indicate write.
 *	@param[in]  mode - set slave mode. slave has two modes, one is DMA mode, the other is MAPPING mode.
 *	@param[in]  pBuf - if slave mode is MAPPING, set the first address of buffer master write or read slave.
 *	@return     none
 */
extern void I2C_SlaveInit(unsigned char device_ID,I2C_SlaveMode mode,unsigned char * pBuf);

/**
 * @brief      This function writes one byte to the slave device at the specified address
 * @param[in]  Addr - pointer to the address where the one byte data will be written
 * @param[in]  AddrLen - length in byte of the address, which makes this function is  
 *             compatible for slave device with both one-byte address and two-byte address
 * @param[in]  Data - the one byte data will be written via I2C interface
 * @return     none
 */
extern void I2C_WriteByte(unsigned int Addr, unsigned int AddrLen, unsigned char Data);

/**
 * @brief      This function reads one byte from the slave device at the specified address
 * @param[in]  Addr - pointer to the address where the one byte data will be read
 * @param[in]  AddrLen - length in byte of the address, which makes this function is  
 *             compatible for slave device with both one-byte address and two-byte address
 * @return     the one byte data read from the slave device via I2C interface
 */
extern unsigned char I2C_ReadByte(unsigned int Addr, unsigned int AddrLen);

/**
 *  @brief      This function writes a bulk of data to the slave device at the specified address
 *  @param[in]  Addr - the register that master write data to slave in. support one byte and two bytes. i.e param2 AddrLen may be 1 or 2.
 *  @param[in]  AddrLen - the length of register. enum 1 or 2. based on the spec of i2c slave.
 *  @param[in]  dataBuf - the first SRAM buffer address to write data to slave in.
 *  @param[in]  dataLen - the length of data master write to slave.
 *  @return     none
 */
extern void I2C_WriteSeries(unsigned short Addr, int AddrLen, unsigned char * dataBuf, int dataLen);

/**
 * @brief      This function reads a bulk of data from the slave device at the specified address
 * @param[in]  Addr - the register master read data from slave in. support one byte and two bytes.
 * @param[in]  AddrLen - the length of register. two data: 1 or 2.
 * @param[in]  dataBuf - the first address of SRAM buffer master store data in.
 * @param[in]  dataLen - the length of data master read from slave.
 * @return     none.
 */
extern void I2C_ReadSeries(unsigned short Addr, int AddrLen, unsigned char * dataBuf, int dataLen);

/**
 *   @brief      master write data to slave that is mapping mode.
 *   @param[in]  pbuf - the first address of data master write to slave.
 *   @param[in]  bufLen - the length of data to write.
 *   @return     none
 */
extern void I2C_WriteDataSlaveMapping(unsigned char * pbuf,unsigned int bufLen);
/**
 *  @brief      read data from slave that is mapping mode.
 *  @param[in]  pbuf - the first address of SRAM buffer to store data.
 *  @param[in]  bufLen - the length of data read from slave(byte).
 *  @return     none
 */
extern void I2C_ReadDataSlaveMapping(unsigned char * pbuf,unsigned int bufLen);


#endif 
