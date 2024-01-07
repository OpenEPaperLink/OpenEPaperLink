/********************************************************************************************************
 * @file	pwm.h
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
#ifndef PWM_H_
#define PWM_H_
#include <reg_include/register.h>
#include "gpio.h"
#include "dma.h"

#define get_pwmid(gpio)     ((gpio==PWM_PWM0_PB4) ? 0 : (  \
                     (gpio==PWM_PWM0_PC0) 	? 0 : (  \
                     (gpio==PWM_PWM0_PE3) 	? 0 : (  \
                     (gpio==PWM_PWM0_N_PD0) ? 0 : (  \
                     (gpio==PWM_PWM1_PB5) 	? 1 : (  \
                     (gpio==PWM_PWM1_PE1) 	? 1 : (  \
                     (gpio==PWM_PWM1_N_PD1) ? 1 : (  \
                     (gpio==PWM_PWM2_PB7) 	? 2 : (  \
                     (gpio==PWM_PWM2_PE2) 	? 2 : (  \
                     (gpio==PWM_PWM2_N_PD2) ? 2 : (  \
                     (gpio==PWM_PWM2_N_PE6) ? 2 : (  \
                     (gpio==PWM_PWM3_PB1) 	? 3 : (  \
                     (gpio==PWM_PWM3_PE0) 	? 3 : (  \
                     (gpio==PWM_PWM3_N_PD3) ? 3 : (  \
                     (gpio==PWM_PWM3_N_PE7) ? 3 : (  \
                     (gpio==PWM_PWM4_PD7) 	? 4 : (  \
                     (gpio==PWM_PWM4_PE4) 	? 4 : (  \
                     (gpio==PWM_PWM4_N_PD4) ? 4 : (  \
                     (gpio==PWM_PWM5_PB0) 	? 5 : (  \
                     (gpio==PWM_PWM5_PE5) 	? 5 : (  \
                     (gpio==PWM_PWM5_N_PD5) ? 5 : 0  \
                    		 )))))))))))))))))))))

#define get_pwm_invert_val(gpio)     ((gpio==PWM_PWM0_N_PD0) ||    \
                     (gpio==PWM_PWM1_N_PD1) ||        \
                     (gpio==PWM_PWM2_N_PD2) ||        \
                     (gpio==PWM_PWM2_N_PE6) ||        \
                     (gpio==PWM_PWM3_N_PD3) ||        \
                     (gpio==PWM_PWM3_N_PE7) ||        \
                     (gpio==PWM_PWM4_N_PD4) ||        \
                     (gpio==PWM_PWM5_N_PD5))

/**
 * @brief  enum variable, the number of PWM channels supported
 */
typedef enum {
	PWM0_ID = 0,
	PWM1_ID,
	PWM2_ID,
	PWM3_ID,
	PWM4_ID,
	PWM5_ID,
}pwm_id_e;


/**
 * @brief  enum variable, represents the PWM mode.
 */
typedef enum{
	PWM_NORMAL_MODE   		= 0x00,
	PWM_COUNT_MODE    		= 0x01,
	PWM_IR_MODE       		= 0x03,
	PWM_IR_FIFO_MODE  		= 0x07,
	PWM_IR_DMA_FIFO_MODE  	= 0x0F,
}pwm_mode_e;


/**
 * @brief  enum variable, represents Selection of PWM pin.
 */
typedef enum{
	PWM_PWM0_PB4 = GPIO_PB4,
	PWM_PWM0_PC0 = GPIO_PC0,
	PWM_PWM0_PE3 = GPIO_PE3,
	PWM_PWM0_N_PD0 = GPIO_PD0,

	PWM_PWM1_PB5 = GPIO_PB5,
	PWM_PWM1_PE1 = GPIO_PE1,
	PWM_PWM1_N_PD1 = GPIO_PD1,

	PWM_PWM2_PB7 = GPIO_PB7,
	PWM_PWM2_PE2 = GPIO_PE2,
	PWM_PWM2_N_PD2 = GPIO_PD2,
	PWM_PWM2_N_PE6 = GPIO_PE6,

	PWM_PWM3_PB1 = GPIO_PB1,
	PWM_PWM3_PE0 = GPIO_PE0,
	PWM_PWM3_N_PD3 = GPIO_PD3,
	PWM_PWM3_N_PE7 = GPIO_PE7,

	PWM_PWM4_PD7 = GPIO_PD7,
	PWM_PWM4_PE4 = GPIO_PE4,
	PWM_PWM4_N_PD4 = GPIO_PD4,

	PWM_PWM5_PB0 = GPIO_PB0,
	PWM_PWM5_PE5 = GPIO_PE5,
	PWM_PWM5_N_PD5 = GPIO_PD5,
}pwm_pin_e;




/**
 * @brief  Select the 32K clock source of pwm.
 */
typedef enum {

	PWM_CLOCK_32K_CHN_PWM0 = 0x01,
	PWM_CLOCK_32K_CHN_PWM1 = 0x02,
	PWM_CLOCK_32K_CHN_PWM2 = 0x04,
	PWM_CLOCK_32K_CHN_PWM3 = 0x08,
	PWM_CLOCK_32K_CHN_PWM4 = 0x10,
	PWM_CLOCK_32K_CHN_PWM5 = 0x20

}pwm_clk_32k_en_chn_e;






/**
 * @brief     This function servers to set pwm clock frequency, when pwm clock source is pclk.
 * @param[in] pwm_clk_div - variable of the pwm clock.
 *            PWM frequency = System_clock / (pwm_clk_div+1).
 * @return	  none.
 */
static inline void pwm_set_clk(unsigned char pwm_clk_div){

    reg_pwm_clkdiv = pwm_clk_div;
}

/**
 * @brief     This function servers to set pwm clock source is 32K.
 *    	      If you use 32K as the PWM clock source, you can work in suspend mode.
 *    	      but the 32K clock source cannot be divided and can only work in continuous mode and counting mode
 * @param[in] pwm_32K_en_chn - This variable is used to select the specific PWM channel.
 * @return	  none.
 */

static inline void pwm_32k_chn_en(pwm_clk_32k_en_chn_e pwm_32K_en_chn){

	reg_pwm_mode32k |= pwm_32K_en_chn;

}


/**
 * @brief     This fuction servers to set pin as pwm0
 * @param[in] pin - selected pin
 * @return	  none.
 */
void pwm_set_pin(pwm_pin_e pin);




/**
 * @brief     This function servers to set pwm count status(CMP) time.
 * @param[in] id   - variable of enum to select the pwm number.
 * @param[in] tcmp - variable of the CMP.
 * @return	  none.
 */
static inline void pwm_set_tcmp(pwm_id_e id, unsigned short tcmp)
{
	reg_pwm_cmp(id) = tcmp;
}


/**
 * @brief     This function servers to set pwm cycle time.
 * @param[in] id   - variable of enum to select the pwm number.
 * @param[in] tmax - variable of the cycle time.
 * @return	  none.
 */
static inline void pwm_set_tmax(pwm_id_e id, unsigned short tmax){
	reg_pwm_max(id) = tmax;
}

/*
 *@brief    This function servers to update the duty cycle in 32K
 *@return	  none.
 */
static inline void pwm_32k_chn_update_duty_cycle(void){
	reg_pwm_cnt5_l |= FLD_PWM_32K_DUTY_CYCLE_UPDATE;
}

/**
 * @brief     This function servers to start the pwm,can have more than one PWM open at the same time.
 * @param[in] en - variable of enum to select the pwm number.
 * @return	  none.
 */
static inline void pwm_start(pwm_en_e en){

		BM_SET(reg_pwm_enable, en);

}


/**
 * @brief     This function servers to stop the pwm,can have more than one PWM stop at the same time.
 * @param[in] en - variable of enum to select the pwm number.
 * @return	  none.
 */
static inline void pwm_stop(pwm_en_e en){

		BM_CLR(reg_pwm_enable, en);

}


/**
 * @brief     This function servers to revert the PWMx.
 * @param[in] id - variable of enum to select the pwm number.
 * @return	  none.
 */
static inline void pwm_invert_en(pwm_id_e id){
	reg_pwm_invert |= BIT(id);
}


/**
 * @brief     This function servers to disable the PWM revert function.
 * @param[in] id - variable of enum to select the pwm number.
 * @return	  none.
 */
static inline void pwm_invert_dis(pwm_id_e id){
	BM_CLR(reg_pwm_invert, BIT(id));
}


/**
 * @brief     This function servers to revert the PWMx_N.
 * @param[in] id - variable of enum to select the pwm number.
 * @return	  none.
 */
static inline void pwm_n_invert_en(pwm_id_e id){
	reg_pwm_n_invert |= BIT(id);
}


/**
 * @brief     This function servers to disable the PWM revert function.
 * @param[in] id - variable of enum to select the pwm number.
 * @return	  none.
 */
static inline void pwm_n_invert_dis(pwm_id_e id){
	BM_CLR(reg_pwm_n_invert, BIT(id));
}


/**
 * @brief     This function servers to enable the pwm polarity.
 * @param[in] id - variable of enum to select the pwm number.
 * @return	  none.
 */
static inline void pwm_set_polarity_en(pwm_id_e id){
		BM_SET(reg_pwm_pol, BIT(id));
}


/**
 * @brief     This function servers to disable the pwm polarity.
 * @param[in] id - variable of enum to select the pwm number.
 * @return	  none.
 */
static inline void pwm_set_polarity_dis(pwm_id_e id){
		BM_CLR(reg_pwm_pol, BIT(id));
}


/**
 * @brief     This function servers to enable the pwm interrupt.
 * @param[in] mask - variable of enum to select the pwm interrupt source.
 * @return	  none.
 */
static inline void pwm_set_irq_mask(pwm_irq_e mask){

	if(mask==FLD_PWM0_IR_FIFO_IRQ)
	{
		BM_SET(reg_pwm_irq_mask(1), BIT(0));
	}
	else
	{
		BM_SET(reg_pwm_irq_mask(0), mask);
	}

}


/**
 * @brief     This function servers to disable the pwm interrupt function.
 * @param[in] mask - variable of enum to select the pwm interrupt source.
 * @return	  none.
 */
static inline void pwm_clr_irq_mask(pwm_irq_e mask){

	if(mask==FLD_PWM0_IR_FIFO_IRQ)
	{
		BM_CLR(reg_pwm_irq_mask(1), BIT(0));
	}
	else
	{
		BM_CLR(reg_pwm_irq_mask(0), mask);
	}

}


/**
 * @brief     This function servers to get the pwm interrupt status.
 * @param[in] status - variable of enum to select the pwm interrupt source.
 * @return	  none.
 */
static inline unsigned char pwm_get_irq_status(pwm_irq_e status){

	if(status==FLD_PWM0_IR_FIFO_IRQ)
	{
		return (reg_pwm_irq_sta(1) & BIT(0));
	}
	else
	{
		return (reg_pwm_irq_sta(0) & status);
	}

}


/**
 * @brief     This function servers to clear the pwm interrupt.When a PWM interrupt occurs, the corresponding interrupt flag bit needs to be cleared manually.
 * @param[in] status  - variable of enum to select the pwm interrupt source.
 * @return	  none.
 */
static inline void pwm_clr_irq_status(pwm_irq_e status){

	if(status==FLD_PWM0_IR_FIFO_IRQ)
	{
		reg_pwm_irq_sta(1) = status;
	}
	else
	{
		reg_pwm_irq_sta(0) = status;
	}

}


/**
 * @brief     This function servers to set pwm0 mode.
 * @param[in] mode - variable of enum to indicates the pwm mode.
 * @return	  none.
 */
static inline void pwm_set_pwm0_mode(pwm_mode_e  mode){
		reg_pwm0_mode = mode;  //only PWM0 has count/IR/fifo IR mode
}


/**
 * @brief     This function servers to set pwm cycle time & count status.
 * @param[in] max_tick - variable of the cycle time.
 * @param[in] cmp_tick - variable of the CMP.
 * @return	  none.
 */
static inline void pwm_set_pwm0_tcmp_and_tmax_shadow(unsigned short max_tick, unsigned short cmp_tick)
{
	reg_pwm_tcmp0_shadow = cmp_tick;
	reg_pwm_tmax0_shadow = max_tick;
}


/**
 * @brief     This function servers to set the pwm0 pulse number.
 * @param[in] pulse_num - variable of the pwm pulse number.The maximum bits is 14bits.
 * @return	  none.
 */
static inline void pwm_set_pwm0_pulse_num(unsigned short pulse_num){
		reg_pwm0_pulse_num = pulse_num;

}


/**
 * @brief     This function serves to set trigger level of interrupt for IR FiFo mode
 * @param[in] trig_level - FIFO  num int trigger level.When fifo numbers is less than this value.It's will take effect.
 * @return	  none
 */
static inline void pwm_set_pwm0_ir_fifo_irq_trig_level(unsigned char trig_level)
{
	reg_pwm_ir_fifo_irq_trig_level = trig_level;
}


/**
 * @brief     This function serves to clear data in fifo. Only when pwm is in not active mode,
 * 			  it is possible to clear data in fifo.
 * @return	  none
 */
static inline void pwm_clr_pwm0_ir_fifo(void)
{
	reg_pwm_ir_clr_fifo_data |= FLD_PWM0_IR_FIFO_CLR_DATA;
}


/**
 * @brief     This function serves to get the number of data in fifo.
 * @return	  the number of data in fifo
 */
static inline unsigned char pwm_get_pwm0_ir_fifo_data_num(void)//????TODO
{
	return (reg_pwm_ir_fifo_data_status & FLD_PWM0_IR_FIFO_DATA_NUM);
}


/**
 * @brief     This function serves to determine whether data in fifo is empty.
 * @return	  yes: 1 ,no: 0;
 */
static inline unsigned char pwm_get_pwm0_ir_fifo_is_empty(void)
{
	return (reg_pwm_ir_fifo_data_status & FLD_PWM0_IR_FIFO_EMPTY);
}


/**
 * @brief     This function serves to determine whether data in fifo is full.
 * @return	  yes: 1 ,no: 0;
 */
static inline unsigned char pwm_get_pwm0_ir_fifo_is_full(void)
{
	return (reg_pwm_ir_fifo_data_status&FLD_PWM0_IR_FIFO_FULL);
}


/**
 * @brief     This function serves to configure the pwm's dma wave form.
 * @param[in] pulse_num - the number of pulse.
 * @param[in] shadow_en - whether enable shadow mode.
 * @param[in] carrier_en - must 1 or 0.
 * @return	  none.
 */
static inline unsigned short pwm_cal_pwm0_ir_fifo_cfg_data(unsigned short pulse_num, unsigned char shadow_en, unsigned char carrier_en)
{
	return  ( carrier_en<<15 | (shadow_en<<14) | (pulse_num & 0x3fff) );
}


/**
 * @brief     This function serves to write data into FiFo
 * @param[in] pulse_num  - the number of pulse
 * @param[in] use_shadow - determine whether the configuration of shadow cmp and shadow max is used
 * 						   1: use shadow, 0: not use
 * @param[in] carrier_en - enable sending carrier, 1: enable, 0: disable
 * @return	  none
 */
static inline void pwm_set_pwm0_ir_fifo_cfg_data(unsigned short pulse_num, unsigned char use_shadow, unsigned char carrier_en)
{
	static unsigned char index=0;
	unsigned short cfg_data = pwm_cal_pwm0_ir_fifo_cfg_data(pulse_num,use_shadow,carrier_en);
	while(pwm_get_pwm0_ir_fifo_is_full());
	reg_pwm_ir_fifo_dat(index) = cfg_data;
	index++;
	index&=0x01;
}


/**
 * @brief     This function servers to configure DMA channel and some configures.
 * @param[in] chn - to select the DMA channel.
 * @return    none
 * @note      In the case that the DMA transfer is not completed(bit 0 of reg_dma_ctr0(chn): 1-the transmission has not been completed,0-the transmission is completed), re-calling the DMA-related functions may cause problems.
 *            If you must do this, you must perform the following sequence:
 *            1. dma_chn_dis(chn) 2.pwm_reset()3.pwm_set_dma_buf()/pwm_ir_dma_mode_start()
 */
void pwm_set_dma_config(dma_chn_e chn);


/**
 * @brief     This function servers to configure DMA channel address and length.
 * @param[in] chn - to select the DMA channel.
 * @param[in] buf_addr - the address where DMA need to get data from SRAM.
 * @param[in] len - the length of data in SRAM.
 * @return    none
 */
void pwm_set_dma_buf(dma_chn_e chn,unsigned int buf_addr,unsigned int len);


/**
 * @brief     This function servers to enable DMA channel.
 * @param[in] chn - to select the DMA channel.
 * @return    none
 */
void pwm_ir_dma_mode_start(dma_chn_e chn);



/**
 * @brief     This function servers to configure DMA head node.
 * @param[in] chn - to select the DMA channel.
 * @param[in] src_addr - to configure DMA source address.
 * @param[in] data_len - to configure DMA length.
 * @param[in] head_of_list - to configure the address of the next node configure.
 * @return    none
 */
void pwm_set_dma_chain_llp(dma_chn_e chn,unsigned short * src_addr, unsigned int data_len,dma_chain_config_t * head_of_list);




/**
 * @brief     This function servers to configure DMA cycle chain node.
 * @param[in] chn - to select the DMA channel.
 * @param[in] config_addr  - to servers to configure the address of the current node.
 * @param[in] llponit - to configure the address of the next node configure.
 * @param[in] src_addr - to configure DMA source address.
 * @param[in] data_len - to configure DMA length.
 * @return    none
 */
void pwm_set_tx_dma_add_list_element(dma_chn_e chn,dma_chain_config_t *config_addr,dma_chain_config_t *llponit ,unsigned short * src_addr,unsigned int data_len);


/**
 * @brief     This function servers to disable pwm clock source 32K.
 *    	      If you use 32K as the PWM clock source, you can work in suspend mode.
 *    	      but the 32K clock source cannot be divided and can only work in continuous mode and counting mode
 * @param[in] pwm_32K_en_chn - This variable is used to select the specific PWM channel.
 * @return	  none.
 */

static inline void pwm_32k_chn_dis(pwm_clk_32k_en_chn_e pwm_32K_en_chn)
{
    BM_CLR(reg_pwm_mode32k, pwm_32K_en_chn);
}

/**
 * @brief      This function reset pwm module,all pwm logic will be reset.
 * @return     none
 */
static inline void pwm_reset(void)
{
	reg_rst0 &= (~FLD_RST0_PWM );
	reg_rst0 |=FLD_RST0_PWM;
}

#endif





