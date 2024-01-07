/********************************************************************************************************
 * @file    app_cfg.h
 *
 * @brief   This is the header file for app_cfg
 *
 * @author  Zigbee Group
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

#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C"
{
#endif

#define ON 1
#define OFF 0

#ifndef MCU_CORE_8258
#define MCU_CORE_8258 1
#endif

#define RAM _attribute_data_retention_ // short version, this is needed to keep the values in ram after sleep

/**********************************************************************
 * Version configuration
 */
#include "version_cfg.h"

/* PM */
#define PM_ENABLE ON


/* Board ID */
#define BOARD_8258_DIY 10

/* Board define */
#if (CHIP_TYPE == TLSR_8258_1M)
#define FLASH_CAP_SIZE_1M 1
#endif
#define BOARD BOARD_8258_DIY // BOARD_8258_DONGLE //BOARD_TB_04 //BOARD_8258_EVK //
#define CLOCK_SYS_CLOCK_HZ 24000000
/************************* For 512K Flash only ***************************************/
/* Flash map:
    0x00000 Old Firmware bin
    0x34000 NV_1
    0x40000 OTA New bin storage Area
    0x76000 MAC address
    0x77000 C_Cfg_Info
    0x78000 U_Cfg_Info
    0x7A000 NV_2
    0x80000 End Flash
 */
#define USER_DATA_SIZE 0x34000
#define BEGIN_USER_DATA1 0x00000
#define END_USER_DATA1 (BEGIN_USER_DATA1 + USER_DATA_SIZE)
#define BEGIN_USER_DATA2 0x40000
#define END_USER_DATA2 (BEGIN_USER_DATA2 + USER_DATA_SIZE)
    //    #define GEN_USER_CFG_DATA           END_USER_DATA2
#define NV_ITEM_APP_USER_CFG (NV_ITEM_APP_GP_TRANS_TABLE + 1) // see sdk/proj/drivers/drv_nv.h

/* Board include */
#include "board_8258_diy.h"

/* Voltage detect module */
/* If VOLTAGE_DETECT_ENABLE is set,
 * 1) if MCU_CORE_826x is defined, the DRV_ADC_VBAT_MODE mode is used by default,
 * and there is no need to configure the detection IO port;
 * 2) if MCU_CORE_8258 or MCU_CORE_8278 is defined, the DRV_ADC_VBAT_MODE mode is used by default,
 * we need to configure the detection IO port, and the IO must be in a floating state.
 * 3) if MCU_CORE_B91 is defined, the DRV_ADC_BASE_MODE mode is used by default,
 * we need to configure the detection IO port, and the IO must be connected to the target under test,
 * such as VCC.
 */
#define VOLTAGE_DETECT_ENABLE OFF

#if defined(MCU_CORE_826x)
#define VOLTAGE_DETECT_ADC_PIN 0
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
#define VOLTAGE_DETECT_ADC_PIN GPIO_PB7
#elif defined(MCU_CORE_B91)
#define VOLTAGE_DETECT_ADC_PIN ADC_GPIO_PB0
#endif

/* Watch dog module */
#define MODULE_WATCHDOG_ENABLE OFF

/* UART module */
#define MODULE_UART_ENABLE OFF

    /**********************************************************************
     * EV configuration
     */
    typedef enum
    {
        EV_POLL_ED_DETECT,
        EV_POLL_HCI,
        EV_POLL_IDLE,
        EV_POLL_MAX,
    } ev_poll_e;

enum{
	CLOCK_SYS_CLOCK_1S = CLOCK_SYS_CLOCK_HZ,
	CLOCK_SYS_CLOCK_1MS = (CLOCK_SYS_CLOCK_1S / 1000),
	CLOCK_SYS_CLOCK_1US = (CLOCK_SYS_CLOCK_1S / 1000000),
};
/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
