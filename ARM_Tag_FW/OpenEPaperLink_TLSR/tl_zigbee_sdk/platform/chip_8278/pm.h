/********************************************************************************************************
 * @file    pm.h
 *
 * @brief   This is the header file for B87
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

#pragma once

#include "bit.h"
#include "gpio.h"
#include "clock.h"


#define XTAL_READY_CHECK_TIMING_OPTIMIZE	1

#define RAM_CRC_EN							1	//if use RAM_CRC func, retention ldo will turn down to 0.6V in A1, A0 is 0.8V.

//when timer wakeup,the DCDC delay time is accurate,but other wake-up sources wake up,
//this time is ((PM_DCDC_DELAY_CYCLE+1)*2-1)*32us ~ (PM_DCDC_DELAY_CYCLE+1)*2*32us
#define PM_DCDC_DELAY_DURATION     			187  // delay_time_us = (PM_DCDC_DELAY_CYCLE+1)*2*32us
												 // 2 * 1/16k = 125 uS, 3 * 1/16k = 187.5 uS  4*1/16k = 250 uS

#define PM_XTAL_MANUAL_MODE_DELAY		    200  //150  200

#if(PM_DCDC_DELAY_DURATION == 62)
#define PM_DCDC_DELAY_CYCLE		0
#elif(PM_DCDC_DELAY_DURATION == 125)
#define PM_DCDC_DELAY_CYCLE		1
#elif(PM_DCDC_DELAY_DURATION == 187)
#define PM_DCDC_DELAY_CYCLE		2
#elif(PM_DCDC_DELAY_DURATION == 250)
#define PM_DCDC_DELAY_CYCLE		3
#endif

#define EARLYWAKEUP_TIME_US_SUSPEND 		(PM_DCDC_DELAY_DURATION + PM_XTAL_MANUAL_MODE_DELAY + 200)  //100: code running time margin//154  //175
#define EARLYWAKEUP_TIME_US_DEEP_RET    	(PM_DCDC_DELAY_DURATION + 64)//(PM_DCDC_DELAY_DURATION + 32)
#define EMPTYRUN_TIME_US       	    		(EARLYWAKEUP_TIME_US_SUSPEND + 200)

#define EARLYWAKEUP_TIME					19
#define	tick_32k_tick_per_ms				32
#define PM_EMPTYRUN_TIME_US					25


/**
 * @brief analog register below can store information when MCU in deepsleep mode
 * 	      store your information in these ana_regs before deepsleep by calling analog_write function
 * 	      when MCU wakeup from deepsleep, read the information by by calling analog_read function
 * 	      Reset these analog registers only by power cycle
 */
#define DEEP_ANA_REG0                       0x3a //initial value =0x00
#define DEEP_ANA_REG1                       0x3b //initial value =0x00
#define DEEP_ANA_REG2                       0x3c //initial value =0x0f

/**
 * @brief these analog register can store data in deepsleep mode or deepsleep with SRAM retention mode.
 * 	      Reset these analog registers by watchdog, chip reset, RESET Pin, power cycle
 */
#define DEEP_ANA_REG6                       0x35 //initial value =0x20
#define DEEP_ANA_REG7                       0x36 //initial value =0x00
#define DEEP_ANA_REG8                       0x37 //initial value =0x00
#define DEEP_ANA_REG9                       0x38 //initial value =0x00
#define DEEP_ANA_REG10                      0x39 //initial value =0xff


#define SYS_NEED_REINIT_EXT32K			    BIT(0)
#define SYS_DEEP_SLEEP_FLAG					BIT(1)


//ana3b system used, user can not use
#define SYS_DEEP_ANA_REG 					DEEP_ANA_REG1
#define WAKEUP_STATUS_TIMER_CORE     	    ( WAKEUP_STATUS_TIMER | WAKEUP_STATUS_CORE)
#define WAKEUP_STATUS_TIMER_PAD		        ( WAKEUP_STATUS_TIMER | WAKEUP_STATUS_PAD)

#define	ZB_POWER_DOWN						1 //weather to power down the RF before suspend
#define	AUDIO_POWER_DOWN					1 //weather to power down the AUDIO before suspend
#define	USB_POWER_DOWN						1 //weather to power down the USB before suspend  //PA5/PA6 pad low wakeup need USB power on

/**
 * @brief sleep mode.
 */
typedef enum {
	//available mode for customer
	SUSPEND_MODE						= 0x00,

	DEEPSLEEP_MODE						= 0x30,		//when use deep mode pad wakeup(low or high level), if the high(low) level always in
													//the pad, system will not enter sleep and go to below of pm API, will reboot by core_6f = 0x20
													//deep retention also had this issue, but not to reboot.
	DEEPSLEEP_MODE_RET_SRAM_LOW16K  	= 0x21,  //for boot from sram
	DEEPSLEEP_MODE_RET_SRAM_LOW32K  	= 0x03,  //for boot from sram

	//not available mode
	DEEPSLEEP_RETENTION_FLAG			= 0x0F,
}SleepMode_TypeDef;


/**
 * @brief   wakeup source
 */
typedef enum {
	 //available wake-up source for customer
	 PM_WAKEUP_PAD   		= BIT(3),
	 PM_WAKEUP_CORE  		= BIT(4),
	 PM_WAKEUP_TIMER 		= BIT(5),
	 PM_WAKEUP_COMPARATOR 	= BIT(6),
	 PM_WAKEUP_MDEC		 	= BIT(7),
}SleepWakeupSrc_TypeDef;

/**
 * @brief   wakeup status
 */
enum {
	 WAKEUP_STATUS_COMPARATOR  		= BIT(0),
	 WAKEUP_STATUS_TIMER  			= BIT(1),
	 WAKEUP_STATUS_CORE 			= BIT(2),
	 WAKEUP_STATUS_PAD    			= BIT(3),
	 WAKEUP_STATUS_MDEC    			= BIT(4),

	 STATUS_GPIO_ERR_NO_ENTER_PM  	= BIT(7),
	 STATUS_ENTER_SUSPEND  			= BIT(30),
};

/**
 * @brief   mcu status
 */
typedef enum{
	MCU_STATUS_BOOT,
	MCU_STATUS_DEEPRET_BACK,
	MCU_STATUS_DEEP_BACK,
}pm_mcu_status;

/**
 * @brief   deepsleep wakeup status
 */
typedef struct{
	unsigned char is_pad_wakeup;
	unsigned char wakeup_src;
	unsigned char mcu_status;
}pm_para_t;


typedef int (*suspend_handler_t)(void);
typedef unsigned int (*pm_tim_recover_handler_t)(unsigned int);
typedef int (*cpu_pm_handler_t)(SleepMode_TypeDef sleep_mode,  SleepWakeupSrc_TypeDef wakeup_src, unsigned int  wakeup_tick);

extern suspend_handler_t func_before_suspend;
extern pm_tim_recover_handler_t pm_tim_recover;
extern cpu_pm_handler_t cpu_sleep_wakeup;
extern pm_para_t pmParam;

extern unsigned char PA5_PA6_DEEPSLEEP_LOW_LEVEL_WAKEUP_EN;
extern unsigned int RAM_CRC_EN_16KRAM_TIME;
extern unsigned int RAM_CRC_EN_32KRAM_TIME;
extern unsigned char SOFT_START_DLY;
extern unsigned int EARLYWAKEUP_TIME_US_DEEP;

void bls_pm_registerFuncBeforeSuspend(suspend_handler_t func);

/**
 * @brief      This function serves to enable dp and dm deep gpio low level wakeup. if enable, current will
 * 						add about 0.1uA
 * @param[in]  none.
 * @return     none.
 */
static inline void deepsleep_dp_dm_gpio_low_wake_enable(void)
{
	PA5_PA6_DEEPSLEEP_LOW_LEVEL_WAKEUP_EN = 0;
}
/**
 * @brief      This function serves to disable dp and dm deep gpio low level wakeup.
 * @param[in]  none.
 * @return     none.
 */
static inline void deepsleep_dp_dm_gpio_low_wake_disable(void)
{
	PA5_PA6_DEEPSLEEP_LOW_LEVEL_WAKEUP_EN = 1;
}

/**
 * @brief      This function serves to change the timing of enable ram crc.
 * @param[in]  none.
 * @return     none.
 */
static inline void ram_crc_en_timing(unsigned int RAM_CRC_16K_Timing, unsigned int RAM_CRC_32K_Timing)
{
	RAM_CRC_EN_16KRAM_TIME = RAM_CRC_16K_Timing;
	RAM_CRC_EN_32KRAM_TIME = RAM_CRC_32K_Timing;
}

/**
 * @brief      This function serves to change the timing of soft start delay.
 * @param[in]  none.
 * @return     none.
 */
static inline void soft_start_dly_time(unsigned char soft_start_time)
{
	SOFT_START_DLY = soft_start_time;
	EARLYWAKEUP_TIME_US_DEEP = PM_DCDC_DELAY_DURATION + 32 + ((SOFT_START_DLY)*62);
}

/**
 * @brief      This function serves to get mcu status.
 * @param[in]  none.
 * @return     pm_mcu_status
 */
static inline unsigned char pm_get_mcu_status(void)
{
	return pmParam.mcu_status;
}

/**
 * @brief      This function serves to determine whether mcu is waked up by pad.
 * @param[in]  none.
 * @return     1- yes , 0- no.
 */
static inline unsigned char pm_is_padWakeup(void)
{
	return pmParam.is_pad_wakeup;
}

/**
 * @brief      This function serves to get the source of wake-up.
 * @param[in]  none.
 * @return     wakeup source.
 */
static inline unsigned char pm_get_wakeup_src(void)
{
	return pmParam.wakeup_src;
}

/**
 * @brief   This function serves to wake up cpu from stall mode by timer0.
 * @param   tick - capture value of timer0.
 * @return  none.
 */
void cpu_stall_wakeup_by_timer0(unsigned int tick);

/**
 * @brief   This function serves to wake up cpu from stall mode by timer1.
 * @param   tick - capture value of timer1.
 * @return  none.
 */
void cpu_stall_wakeup_by_timer1(unsigned int tick);

/**
 * @brief   This function serves to wake up cpu from stall mode by timer2.
 * @param   tick - capture value of timer2.
 * @return  none.
 */
void cpu_stall_wakeup_by_timer2(unsigned int tick);

/**
 * @brief      This function configures a GPIO pin as the wakeup pin.
 * @param[in]  pin - the pin needs to be configured as wakeup pin
 * @param[in]  pol - the wakeup polarity of the pad pin(0: low-level wakeup, 1: high-level wakeup)
 * @param[in]  en  - enable or disable the wakeup function for the pan pin(1: Enable, 0: Disable)
 * @return     none
 */
void cpu_set_gpio_wakeup(GPIO_PinTypeDef pin, GPIO_LevelTypeDef pol, int en);

/**
 * @brief     This function servers to set the match value for MDEC wakeup.
 * @param[in] value - the MDEC match value for wakeup.
 * @return    none
 */
void cpu_set_mdec_value_wakeup(unsigned char value);

/**
 * @brief   This function serves to get the 32k tick.
 * @param   none
 * @return  tick of 32k .
 */
extern unsigned int pm_get_32k_tick(void);

/**
 * @brief   This function serves to initialize MCU
 * @param   power mode- set the power mode(LOD mode, DCDC mode, DCDC_LDO mode)
 * @param   xtal- set this parameter based on external crystal
 * @return  none
 */
void cpu_wakeup_init(POWER_MODE_TypeDef power_mode, XTAL_TypeDef xtal);

/**
 * @brief   This function serves to recover system timer from tick of internal 32k RC.
 * @param   none.
 * @return  none.
 */
unsigned int pm_tim_recover_32k_rc(unsigned int now_tick_32k);

/**
 * @brief   This function serves to recover system timer from tick of external 32k crystal.
 * @param   none.
 * @return  none.
 */
unsigned int pm_tim_recover_32k_xtal(unsigned int now_tick_32k);

/**
 * @brief      This function serves to set the working mode of MCU based on 32k crystal,e.g. suspend mode, deepsleep mode, deepsleep with SRAM retention mode and shutdown mode.
 * @param[in]  sleep_mode - sleep mode type select.
 * @param[in]  wakeup_src - wake up source select.
 * @param[in]  wakeup_tick - the time of short sleep, which means MCU can sleep for less than 5 minutes.
 * @return     indicate whether the cpu is wake up successful.
 */
int cpu_sleep_wakeup_32k_rc(SleepMode_TypeDef sleep_mode, SleepWakeupSrc_TypeDef wakeup_src, unsigned int wakeup_tick);

/**
 * @brief      This function serves to set the working mode of MCU based on 32k crystal,e.g. suspend mode, deepsleep mode, deepsleep with SRAM retention mode and shutdown mode.
 * @param[in]  sleep_mode - sleep mode type select.
 * @param[in]  wakeup_src - wake up source select.
 * @param[in]  wakeup_tick - the time of short sleep, which means MCU can sleep for less than 5 minutes.
 * @return     indicate whether the cpu is wake up successful.
 */
int cpu_sleep_wakeup_32k_xtal(SleepMode_TypeDef sleep_mode, SleepWakeupSrc_TypeDef wakeup_src, unsigned int wakeup_tick);

/**
 * @brief      This function serves to set the working mode of MCU,e.g. suspend mode, deepsleep mode, deepsleep with SRAM retention mode and shutdown mode.
 * @param[in]  sleep_mode - sleep mode type select.
 * @param[in]  wakeup_src - wake up source select.
 * @param[in]  wakeup_tick - the time of short sleep, which means MCU can sleep for more than 1 day.
 * @return     indicate whether the cpu is wake up successful.
 */
int pm_long_sleep_wakeup(SleepMode_TypeDef sleep_mode, SleepWakeupSrc_TypeDef wakeup_src, unsigned int wakeup_tick);

/**
 * @brief      This function serves to determine whether wake up source is internal 32k RC.
 * @param[in]  none.
 * @return     none.
 */
static inline void pm_select_internal_32k_rc(void)
{
	cpu_sleep_wakeup 	 	= cpu_sleep_wakeup_32k_rc;
	pm_tim_recover  	 	= pm_tim_recover_32k_rc;
}

/**
 * @brief      This function serves to determine whether wake up source is external 32k RC.
 * @param[in]  none.
 * @return     none.
 */
static inline void pm_select_external_32k_crystal(void)
{
	cpu_sleep_wakeup 	 	= cpu_sleep_wakeup_32k_xtal;
	pm_tim_recover		 	= pm_tim_recover_32k_xtal;
}


/**********************************  Internal APIs (not for user)***************************************************/
extern  unsigned char 		    tl_multi_addr_L;
extern  unsigned char 		    tl_multi_addr_H;
extern  unsigned char 		    tl_24mrc_cal;
extern 	unsigned int 			tick_32k_calib;
extern  unsigned int 			tick_cur;
extern  unsigned int 			tick_32k_cur;
extern  unsigned char       	pm_long_suspend;

void sleep_start(void);

unsigned int pm_get_info0(void);

unsigned int pm_get_info1(void);

unsigned int cpu_get_32k_tick(void);

void cpu_set_32k_tick(unsigned int tick);

