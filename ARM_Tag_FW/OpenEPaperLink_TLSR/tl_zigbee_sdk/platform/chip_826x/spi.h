/********************************************************************************************************
 * @file    spi.h
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

#ifndef SPI_H
#define SPI_H

#include "gpio.h"

/**
 *  @brief  Define the mode for SPI interface
 */
typedef enum {
    SPI_MODE0 = 0,
    SPI_MODE2,
    SPI_MODE1,
    SPI_MODE3,
} SPI_ModeTypeDef;

/**
 *  @brief  Define the pin port for SPI interface 
 */
typedef enum {
    SPI_PIN_GPIOA = 0,
    SPI_PIN_GPIOB,
} SPI_PinTypeDef;

/**
 * @brief     This function selects a pin port for the SPI interface
 * @param[in] PinGrp - the selected pin port
 * @return    none
 */
extern void SPI_MasterPinSelect(SPI_PinTypeDef PinGrp);

/**
 * @brief     This function selects a GPIO pin as CS of SPI function.
 * @param[in] CSPin - the selected CS pin
 * @return    none
 */
extern void SPI_MasterCSPinSelect(GPIO_PinTypeDef CSPin);

/**
 * @brief     This function configures the clock and working mode for SPI interface
 * @param[in] DivClock - the division factor for SPI module
 *            SPI clock = System clock / ((DivClock+1)*2)
 * @param[in] Mode - the selected working mode of SPI module
 * @return    none
 */
extern void SPI_MasterInit(unsigned char DivClock, SPI_ModeTypeDef Mode);

/**
 * @brief      This function serves to write a bulk of data to the SPI slave
 *             device specified by the CS pin
 * @param[in]  Cmd - pointer to the command bytes needed written into the 
 *             slave device first before the writing operation of actual data
 * @param[in]  CmdLen - length in byte of the command bytes 
 * @param[in]  Data - pointer to the data need to write 
 * @param[in]  DataLen - length in byte of the data need to write
 * @param[in]  CSPin - the CS pin specifing the slave device
 * @return     none
 */
extern void SPI_Write(unsigned char *Cmd, int CmdLen, unsigned char *Data, int DataLen, GPIO_PinTypeDef CSPin);

/**
 * @brief      This function serves to read a bulk of data from the SPI slave
 *             device specified by the CS pin
 * @param[in]  Cmd - pointer to the command bytes needed written into the 
 *             slave device first before the reading operation of actual data
 * @param[in]  CmdLen - length in byte of the command bytes 
 * @param[out] Data - pointer to the buffer that will cache the reading out data 
 * @param[in]  DataLen - length in byte of the data need to read
 * @param[in]  CSPin - the CS pin specifing the slave device
 * @return     none
 */
extern void SPI_Read(unsigned char *Cmd, int CmdLen, unsigned char *Data, int DataLen, GPIO_PinTypeDef CSPin);

/**
 * @brief     This function configures the clock and working mode for SPI interface
 * @param[in] DivClock - the division factor for SPI module
 *            SPI clock = System clock / ((DivClock+1)*2)
 * @param[in] Mode - the selected working mode of SPI module
 * @return    none
 */
extern void SPI_SlaveInit(unsigned char DivClock, SPI_ModeTypeDef Mode);

/**
 * @brief     This function selects a pin port for the SPI interface
 * @param[in] PinGrp - the selected pin port
 * @return    none
 */
extern void SPI_SlavePinSelect(SPI_PinTypeDef PinGrp);

#endif

