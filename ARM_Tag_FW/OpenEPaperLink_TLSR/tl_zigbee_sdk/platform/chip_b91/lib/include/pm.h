/********************************************************************************************************
 * @file	pm.h
 *
 * @brief	This is the header file for B91
 *
 * @author	Driver Group
 * @date	2019
 *
 * @par     Copyright (c) 2019, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#pragma once

#include <reg_include/register.h>
#include "compiler.h"
#include "gpio.h"
#include "clock.h"

/********************************************************************************************************
 *											internal
 *******************************************************************************************************/

/********************************************************************************************************
 * 				This is currently included in the H file for compatibility with other SDKs.
 *******************************************************************************************************/

//When the watchdog comes back, the Eagle chip does not clear 0x7f[0]. To avoid this problem, this macro definition is added.
#ifndef WDT_REBOOT_RESET_ANA7F_WORK_AROUND
#define WDT_REBOOT_RESET_ANA7F_WORK_AROUND	1
#endif

/********************************************************************************************************
 *											external
 *******************************************************************************************************/


/**
 * @brief these analog register can store data in deepsleep mode or deepsleep with SRAM retention mode.
 * 	      Reset these analog registers by watchdog, chip reset, RESET Pin, power cycle
 */
#define PM_ANA_REG_WD_CLR_BUF0 			0x38 // initial value 0xff. [Bit0] is already occupied. The customer cannot change!

/**
 * @brief analog register below can store information when MCU in deepsleep mode or deepsleep with SRAM retention mode.
 * 	      Reset these analog registers only by power cycle
 */
#define PM_ANA_REG_POWER_ON_CLR_BUF0 	0x39 // initial value 0x00. [Bit0][Bit1] is already occupied. The customer cannot change!
#define PM_ANA_REG_POWER_ON_CLR_BUF1 	0x3a // initial value 0x00
#define PM_ANA_REG_POWER_ON_CLR_BUF2 	0x3b // initial value 0x00
#define PM_ANA_REG_POWER_ON_CLR_BUF3 	0x3c // initial value 0x00
#define PM_ANA_REG_POWER_ON_CLR_BUF4 	0x3d // initial value 0x00
#define PM_ANA_REG_POWER_ON_CLR_BUF5 	0x3e // initial value 0x00
#define PM_ANA_REG_POWER_ON_CLR_BUF6	0x3f // initial value 0x0f

/**
 * @brief	gpio wakeup level definition
 */
typedef enum{
	WAKEUP_LEVEL_LOW		= 0,
	WAKEUP_LEVEL_HIGH		= 1,
}pm_gpio_wakeup_level_e;

/**
 * @brief	wakeup tick type definition
 */
typedef enum {
	 PM_TICK_STIMER_16M		= 0,
	 PM_TICK_32K			= 1,
}pm_wakeup_tick_type_e;

/**
 * @brief	suspend power weather to power down definition
 */
typedef enum {
	 PM_POWER_BASEBAND  	= BIT(0),	//weather to power on the BASEBAND before suspend.
	 PM_POWER_USB  		= BIT(1),	//weather to power on the USB before suspend.
	 PM_POWER_NPE 		= BIT(2),	//weather to power on the NPE before suspend.
}pm_suspend_power_cfg_e;

/**
 * @brief	sleep mode.
 */
typedef enum {
	//available mode for customer
	SUSPEND_MODE						= 0x00, //The A0 version of the suspend execution process is abnormal and the program restarts.
	DEEPSLEEP_MODE						= 0x30,	//when use deep mode pad wakeup(low or high level), if the high(low) level always in
												//the pad, system will not enter sleep and go to below of pm API, will reboot by core_6f = 0x20
												//deep retention also had this issue, but not to reboot.
	DEEPSLEEP_MODE_RET_SRAM_LOW32K  	= 0x21, //for boot from sram
	DEEPSLEEP_MODE_RET_SRAM_LOW64K  	= 0x03, //for boot from sram
	//not available mode
	DEEPSLEEP_RETENTION_FLAG			= 0x0F,
}pm_sleep_mode_e;

/**
 * @brief	available wake-up source for customer
 */
typedef enum {
	 PM_WAKEUP_PAD   		= BIT(3),
	 PM_WAKEUP_CORE  		= BIT(4),
	 PM_WAKEUP_TIMER 		= BIT(5),
	 PM_WAKEUP_COMPARATOR 	= BIT(6),
	 PM_WAKEUP_MDEC		 	= BIT(7),
}pm_sleep_wakeup_src_e;

/**
 * @brief	wakeup status
 */
typedef enum {
	 WAKEUP_STATUS_COMPARATOR  		= BIT(0),
	 WAKEUP_STATUS_TIMER  			= BIT(1),
	 WAKEUP_STATUS_CORE 			= BIT(2),
	 WAKEUP_STATUS_PAD    			= BIT(3),
	 WAKEUP_STATUS_MDEC    			= BIT(4),

	 STATUS_GPIO_ERR_NO_ENTER_PM	= BIT(7),
	 STATUS_ENTER_SUSPEND  			= BIT(30),
}pm_wakeup_status_e;

/**
 * @brief	mcu status
 * 			In order to fix the problem that reboot returns to occasional crash when hclk = 1/2cclk, after each reboot,
 * 			it will immediately enter deep. Therefore, the user will not see the reboot status. Increase the REBOOT_DEEP
 * 			state to indicate this process.(add by weihua.zhang, confirmed by libiao and yangbin 20201211)
 */
typedef enum{
	MCU_STATUS_POWER_ON  		= BIT(0),
	MCU_STATUS_REBOOT_BACK    	= BIT(2),	//the user will not see the reboot status.
	MCU_STATUS_DEEPRET_BACK  	= BIT(3),
	MCU_STATUS_DEEP_BACK		= BIT(4),
	MCU_STATUS_REBOOT_DEEP_BACK	= BIT(5),	//reboot + deep
}pm_mcu_status;

/**
 * @brief	early wakeup time
 */
typedef struct {
	unsigned short  suspend_early_wakeup_time_us;	/**< suspend_early_wakeup_time_us = deep_ret_r_delay_us + xtal_stable_time + early_time*/
	unsigned short  deep_ret_early_wakeup_time_us;  /**< deep_ret_early_wakeup_time_us = deep_ret_r_delay_us + early_time*/
	unsigned short  deep_early_wakeup_time_us;		/**< deep_early_wakeup_time_us = suspend_ret_r_delay_us*/
	unsigned short  sleep_min_time_us;				/**< sleep_min_time_us = suspend_early_wakeup_time_us + 200*/
}pm_early_wakeup_time_us_s;

/**
 * @brief	hardware delay time
 */
typedef struct {
	unsigned short  deep_r_delay_cycle ;			/**< hardware delay time ,deep_ret_r_delay_us = deep_r_delay_cycle * 1/16k */
	unsigned short  suspend_ret_r_delay_cycle ;		/**< hardware delay time ,suspend_ret_r_delay_us = suspend_ret_r_delay_cycle * 1/16k */

}pm_r_delay_cycle_s;

/**
 * @brief   deepsleep wakeup status
 */
typedef struct{
	unsigned char is_pad_wakeup;
	unsigned char wakeup_src;	//The pad pin occasionally wakes up abnormally in A0. The core wakeup flag will be incorrectly set in A0.
	unsigned char mcu_status;
	unsigned char rsvd;
}pm_status_info_s;

/**
 * @brief   pm 32k rc calibration algorithm.
 */
typedef struct  pm_clock_drift
{
	unsigned int	ref_tick;
	unsigned int	ref_tick_32k;
	int				offset;
	int				offset_dc;
	int				offset_cur;
	int				tc;
	int				rc32;
	int				rc32_wakeup;
	int				rc32_rt;
	int				s0;
	unsigned char	calib;

} pm_clock_drift_t;


extern pm_clock_drift_t	pmcd;
extern _attribute_aligned_(4) pm_status_info_s g_pm_status_info;
extern _attribute_data_retention_sec_ unsigned char g_pm_vbat_v;

/**
 * @brief		This function servers to set the match value for MDEC wakeup.
 * @param[in]	value - the MDEC match value for wakeup.
 * @return		none.
 */
static inline void pm_set_mdec_value_wakeup(unsigned char value)
{
	analog_write_reg8(mdec_ctrl,((analog_read_reg8(mdec_ctrl) & (~0x0f)) | value));
}

/**
 * @brief		This function serves to get deep retention flag.
 * @return		1 deep retention, 0 deep.
 */
static inline unsigned char pm_get_deep_retention_flag(void)
{
	return !(analog_read_reg8(0x7f) & BIT(0));
}

/**
 * @brief		This function serves to get wakeup source.
 * @return		wakeup source.
 */
static inline pm_wakeup_status_e pm_get_wakeup_src(void)
{
	return analog_read_reg8(0x64);
}

/**
 * @brief		This function configures a GPIO pin as the wakeup pin.
 * @param[in]	pin	- the pin needs to be configured as wakeup pin.
 * @param[in]	pol - the wakeup polarity of the pad pin(0: low-level wakeup, 1: high-level wakeup).
 * @param[in]	en  - enable or disable the wakeup function for the pan pin(1: enable, 0: disable).
 * @return		none.
 */
void pm_set_gpio_wakeup (gpio_pin_e pin, pm_gpio_wakeup_level_e pol, int en);

/**
 * @brief		This function configures pm wakeup time parameter.
 * @param[in]	param - pm wakeup time parameter.
 * @return		none.
 */
void pm_set_wakeup_time_param(pm_r_delay_cycle_s param);

/**
 * @brief		this function servers to wait bbpll clock lock.
 * @return		none.
 */
_attribute_ram_code_sec_noinline_ void pm_wait_bbpll_done(void);

/**
 * @brief		This function serves to recover system timer.
 * 				The code is placed in the ram code section, in order to shorten the time.
 * @return		none.
 */
_attribute_ram_code_sec_noinline_ void pm_stimer_recover(void);

/**
 * @brief		This function serves to set the working mode of MCU based on 32k crystal,e.g. suspend mode, deepsleep mode, deepsleep with SRAM retention mode and shutdown mode.
 * @param[in]	sleep_mode 			- sleep mode type select.
 * @param[in]	wakeup_src 			- wake up source select.
 * 		A0	   	note: The reference current values under different configurations are as followsUnit (uA):
 * 					|	pad		|	32k rc	|	32k xtal	|	mdec	|	lpc	 	|
 * 	deep			|	0.7		|	1.3		|	1.7			|	1.4		|	1.6		|
 * 	deep ret 32k	|	1.8		|	2.4		|	2.8			|	2.6		|	2.8		|
 * 	deep ret 64k	|	2.7		|	3.2		|	3.7			|	3.4		|	3.7		|
 * 				A0 chip, the retention current will float up.
 * @param[in]	wakeup_tick_type	- tick type select. For long timer sleep.currently only 16M is supported(PM_TICK_STIMER_16M).
 * @param[in]	wakeup_tick			- the time of short sleep, which means MCU can sleep for less than 5 minutes.
 * @return		indicate whether the cpu is wake up successful.
 */
_attribute_text_sec_ int pm_sleep_wakeup(pm_sleep_mode_e sleep_mode,  pm_sleep_wakeup_src_e wakeup_src, pm_wakeup_tick_type_e wakeup_tick_type, unsigned int  wakeup_tick);

/**
 * @brief		Calculate the offset value based on the difference of 16M tick.
 * @param[in]	offset_tick	- the 16M tick difference between the standard clock and the expected clock.
 * @return		none.
 */
_attribute_ram_code_sec_noinline_ void pm_cal_32k_rc_offset (int offset_tick);

/**
 * @brief		When 32k rc sleeps, the calibration function is initialized.
 * @return		none.
 */
_attribute_ram_code_sec_noinline_ void pm_32k_rc_offset_init(void);

/**
 * @brief		This function serves to set baseband/usb/npe power on/off before suspend sleep,If power
 * 				on this module,the suspend curent will increase;power down this module will save current,
 * 				but you need to re-init this module after suspend wakeup.All module is power down default
 * 				to save current.
 * @param[in]	value - weather to power on/off the baseband/usb/npe.
 * @param[in]	on_off - select power on or off, 0 - power off; other value - power on.
 * @return		none.
 */
void pm_set_suspend_power_cfg(pm_suspend_power_cfg_e value, unsigned char on_off);

