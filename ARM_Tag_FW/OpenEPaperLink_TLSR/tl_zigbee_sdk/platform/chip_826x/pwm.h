/********************************************************************************************************
 * @file    pwm.h
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

#ifndef PWM_H
#define PWM_H

//pwm0 gpio config
#define    PWM0_CFG_GPIO_A0()    do{\
									*(volatile unsigned char  *)0x800586 &= 0xfe;\
									*(volatile unsigned char  *)0x8005b0 &= 0xfe;\
								 }while(0) 
#define    PWM0_CFG_GPIO_C0()    do{\
									*(volatile unsigned char  *)0x800596 &= 0xfe;\
									*(volatile unsigned char  *)0x8005b2 &= 0xfe;\
								 }while(0) 
#define    PWM0_CFG_GPIO_D5()    do{\
									*(volatile unsigned char  *)0x80059e &= 0xdf;\
								 }while(0) 
#define    PWM0_CFG_GPIO_E0()    do{\
									*(volatile unsigned char  *)0x8005a6 &= 0xfe;\
									*(volatile unsigned char  *)0x8005b4 |= 0x01;\
								 }while(0) 

//pwm1 gpio config
#define    PWM1_CFG_GPIO_A3()    do{\
									*(volatile unsigned char  *)0x800586 &= 0xf7;\
									*(volatile unsigned char  *)0x8005b0 &= 0xfb;\
								 }while(0) 
#define    PWM1_CFG_GPIO_C1()    do{\
									*(volatile unsigned char  *)0x800596 &= 0xfd;\
									*(volatile unsigned char  *)0x8005b2 &= 0xfd;\
								 }while(0) 
#define    PWM1_CFG_GPIO_D6()    do{\
									*(volatile unsigned char  *)0x80059e &= 0xbf;\
								 }while(0) 
#define    PWM1_CFG_GPIO_E1()    do{\
									*(volatile unsigned char  *)0x8005a6 &= 0xfd;\
									*(volatile unsigned char  *)0x8005b4 |= 0x02;\
								 }while(0) 

//pwm2 gpio config
#define    PWM2_CFG_GPIO_B0()    do{\
									*(volatile unsigned char  *)0x80058e &= 0xfe;\
									*(volatile unsigned char  *)0x8005b1 |= 0x01;\
								 }while(0) 
#define    PWM2_CFG_GPIO_C2()    do{\
									*(volatile unsigned char  *)0x800596 &= 0xfb;\
									*(volatile unsigned char  *)0x8005b2 &= 0xfb;\
								 }while(0) 
#define    PWM2_CFG_GPIO_D7()    do{\
									*(volatile unsigned char  *)0x80059e &= 0x7f;\
								 }while(0) 

//pwm3 gpio config
#define    PWM3_CFG_GPIO_B2()    do{\
									*(volatile unsigned char  *)0x80058e &= 0xfb;\
									*(volatile unsigned char  *)0x8005b1 &= 0xfb;\
								 }while(0) 
#define    PWM3_CFG_GPIO_C3()    do{\
									*(volatile unsigned char  *)0x800596 &= 0xf7;\
									*(volatile unsigned char  *)0x8005b2 &= 0xf7;\
								 }while(0) 

//pwm4 gpio config
#define    PWM4_CFG_GPIO_B4()    do{\
									*(volatile unsigned char  *)0x80058e &= 0xef;\
									*(volatile unsigned char  *)0x8005b1 &= 0xef;\
								 }while(0) 
#define    PWM4_CFG_GPIO_C4()    do{\
									*(volatile unsigned char  *)0x800596 &= 0xef;\
									*(volatile unsigned char  *)0x8005b2 &= 0xef;\
								 }while(0)

//pwm5 gpio config
#define    PWM5_CFG_GPIO_B6()    do{\
									*(volatile unsigned char  *)0x80058e &= 0xbf;\
									*(volatile unsigned char  *)0x8005b1 &= 0xbf;\
								 }while(0) 
#define    PWM5_CFG_GPIO_C5()    do{\
									*(volatile unsigned char  *)0x800596 &= 0xdf;\
									*(volatile unsigned char  *)0x8005b2 &= 0xdf;\
								 }while(0) 

//pwm0_n gpio config
#define    PWM0N_CFG_GPIO_A2()   do{\
									*(volatile unsigned char  *)0x800586 &= 0xfb;\
									*(volatile unsigned char  *)0x8005b0 &= 0xfd;\
								 }while(0) 

//pwm1_n gpio config
#define    PWM1N_CFG_GPIO_A4()   do{\
									*(volatile unsigned char  *)0x800586 &= 0xef;\
									*(volatile unsigned char  *)0x8005b0 &= 0xef;\
								 }while(0) 

//pwm2_n gpio config
#define    PWM2N_CFG_GPIO_A5()   do{\
									*(volatile unsigned char  *)0x800586 &= 0xdf;\
									*(volatile unsigned char  *)0x8005b0 &= 0xdf;\
								 }while(0) 
#define    PWM2N_CFG_GPIO_B1()    do{\
									*(volatile unsigned char  *)0x80058e &= 0xfd;\
									*(volatile unsigned char  *)0x8005b1 &= 0xfd;\
								 }while(0)

//pwm3_n gpio config
#define    PWM3N_CFG_GPIO_B3()    do{\
									*(volatile unsigned char  *)0x80058e &= 0xf7;\
									*(volatile unsigned char  *)0x8005b1 &= 0xf7;\
								 }while(0)

//pwm4_n gpio config
#define    PWM4N_CFG_GPIO_B5()    do{\
									*(volatile unsigned char  *)0x80058e &= 0xdf;\
									*(volatile unsigned char  *)0x8005b1 &= 0xdf;\
								 }while(0)

//pwm5_n gpio config
#define    PWM5N_CFG_GPIO_B7()    do{\
									*(volatile unsigned char  *)0x80058e &= 0x7f;\
									*(volatile unsigned char  *)0x8005b1 &= 0x7f;\
								 }while(0)


//Enable the specified pwm, v is the PWMN
#define		EN_PWM(v)			(*(volatile unsigned char  *)0x800780 |= (1<<v))

//Disable the specified pwm, v is the PWMN
#define 	DIS_PWM(v)			(*(volatile unsigned char  *)0x800780 &= ~(1<<v))

/*************************************************************
*	@brief		enum variable, the number of PWM channels supported
*
*/
enum PWMN{
	PWM0 = 0,
	PWM1,
	PWM2,
	PWM3,
	PWM4,
	PWM5,
};


/*************************************************************
*	@brief		enum variable used for PWM work mode setting
*
*/
enum PWMWM{
	NORMAL,
	COUNT,
	IR = 0x03,
};

/*************************************************************
*	@brief		enum variable of interrupt source
*

enum INTSOURCE{
	SFNIRQ,//completed a pulse group IRQ, only counting mode and IR mode
	FDIRQ,//Frame done IRQ
};
*/
/********************************************************
*
*	pwm interrupt source 
*/
enum PWMIRQSOURCE{
	NOIRQ = 0xff,
	PWM0PNUM = 0,
	PWM1PNUM,
	PWM0CYCLEDONE,
	PWM1CYCLEDONE,
	PWM2CYCLEDONE,
	PWM3CYCLEDONE,
	PWM4CYCLEDONE,
	PWM5CYCLEDONE,
	PWMIRQSOURCENUM,
};


/************************************************************
*	@brief	Initiate the PWM function include seting and enabling the PWM clock. 
*			For test pin number 22 - 27 is the PWM output pin.
*
*	@param	None
*
*	@return	None
*/
extern void pwm_Init(unsigned char pwmCLKdiv);

/********************************************************************************
*	@brief	open a pwm channel and setting the parameters
*
*	@param	pwmNumber - enum variable of pwm channel number
*			pwmWorkM - enum variable of pwm work mode
*			phaseTime - delay time of the pwm channel before enter into the count status
*			cscValue - count status value
*			cycVlaue - cycle value
*			sfValue - signal frame value for the COUNT and IR mode
*	
*	@return	'1' set success; '0' set error, if you set pwm2 - pwm5 to work on the other not normal mode, the function will return error.
*
*/
extern unsigned char pwm_Open(enum PWMN pwmNumber, enum PWMWM pwmWorkM, unsigned short phaseTime, unsigned short cscValue, unsigned short cycValue, unsigned short sfValue );

/*******************************************************************************
*
*	@brief	close a pwm channel and reset the settings to default
*	
*	@param	pwmNumber - enum variable of pwm channel number

*
*	@return	None
*/
extern void pwm_Close(enum PWMN pwmNumber);

/*******************************************************************************
*
*	@brief	start a pwm channel and enable the output
*	
*	@param	pwmNumber - enum variable of pwm channel number

*
*	@return	None
*/
extern void pwm_Start(enum PWMN pwmNumber);

/*******************************************************************************
*
*	@brief	stop a pwm channel and disable the output
*	
*	@param	pwmNumber - enum variable of pwm channel number

*
*	@return	None
*/
extern void pwm_Stop(enum PWMN pwmNumber);

/*******************************************************************************
*
*	@brief	set the duty cycle of a pwm channel
*	
*	@param	pwmNumber - enum variable of pwm channel number
*			cscValue - count status value
*			cycVlaue - cycle value

*
*	@return	'1' set success; '0' set error, if csValue>cycValue
*/
extern unsigned char pwm_DutyCycleSet(enum PWMN pwmNumber, unsigned short csValue,unsigned short cycValue);

/********************************************************
*	@brief	enable pwm interrupt
*
*	@param	pwmNumber - enum variable of pwm channel number
*			pwmINTM - enum variable of pwm channel's interrupt mode
*
*	@return	None
*/
extern unsigned char pwm_InterruptEnable(enum PWMIRQSOURCE pwmIrqSrc);

/********************************************************
*	@brief	disable pwm interrupt
*
*	@param	pwmNumber - enum variable of pwm channel number
*			pwmM - enum variable of pwm work mode
*
*	@return	None
*/
extern unsigned char pwm_InterruptDisable(enum PWMIRQSOURCE pwmIrqSrc);
/**********************************************************************
*
*	@brief	get interrupt source
*
*	@param	none
*	
*	@return	irq_source - interrupt source, enum variable
*/
extern enum PWMIRQSOURCE pwm_InterruptSourceGet(void );

/********************************************************
*	@brief	clear interrupt status
*
*	@param	pwmNumber - enum variable of pwm channel number
*			pwmINTM - enum variable of pwm channel's interrupt mode
*
*	@return	None
*/
extern void pwm_InterruptStatusClr(enum PWMIRQSOURCE irq_src);

/********************************************************************
*
*	@brief	Invert PWM-INV output (would generate the same output waveform as PWMN)
*
*	@param	pwmNumber - enum variable of pwm number
*
*	@return	None
*/
extern void pwm_INVInvert(enum PWMN pwmNumber);

/********************************************************************
*
*	@brief	Invert PWM output (would generate the same output waveform as PWMN)
*
*	@param	pwmNumber - enum variable of pwm number
*
*	@return	None
*/
extern void pwm_Invert(enum PWMN pwmNumber);

#endif
