/********************************************************************************************************
 * @file    pwm.h
 *
 * @brief   This is the header file for B85
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

#ifndef PWM_H_
#define PWM_H_

#include "register.h"
#include "bit.h"
#include "timer.h"

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
}pwm_id;

/**
 * @brief  enum variable used for PWM work mode setting
 */
typedef enum{
	PWM_NORMAL_MODE   = 0x00,
	PWM_COUNT_MODE    = 0x01,
	PWM_IR_MODE       = 0x03,
	PWM_IR_FIFO_MODE  = 0x07,
	PWM_IR_DMA_FIFO_MODE  = 0x0F,
}pwm_mode;

/**
 * pwm interrupt source
 */
typedef enum{
	PWM_IRQ_PWM0_PNUM =					BIT(0),
	PWM_IRQ_PWM0_IR_DMA_FIFO_DONE =		BIT(1),
	PWM_IRQ_PWM0_FRAME =				BIT(2),
	PWM_IRQ_PWM1_FRAME =				BIT(3),
	PWM_IRQ_PWM2_FRAME =				BIT(4),
	PWM_IRQ_PWM3_FRAME =				BIT(5),
	PWM_IRQ_PWM4_FRAME =				BIT(6),
	PWM_IRQ_PWM5_FRAME =				BIT(7),

	PWM_IRQ_PWM0_IR_FIFO =              BIT(16)
}PWM_IRQ;

typedef enum{
	PWM0_PULSE_NORMAL =		0,       // duty cycle and period from TCMP0/TMAX0 					 0x794~0x797
	PWM0_PULSE_SHADOW =		BIT(14), // duty cycle and period from TCMP0_SHADOW / TMAX0_SHADOW   0x7c4~0x7c7
}Pwm0Pulse_SelectDef;


/**
 * @brief     This fuction servers to set pwm mode.
 * @param[in] pwm_id - variable of enum to select the pwm number.
 * @param[in] mode - variable of enum to indicates the pwm mode.
 * @return	  none.
 */
static inline void pwm_set_mode(pwm_id id, pwm_mode mode){
	if(PWM0_ID == id){
		reg_pwm0_mode = mode;  //only PWM0 has count/IR/fifo IR mode
	}
}


/**
 * @brief     This fuction servers to set pwm clock frequency
 * @param[in] system_clock_hz - variable to set system clock hz.
 * @param[in] pwm_clk - variable of the pwm clock.
 * @return	  none.
 */
static inline void pwm_set_clk(int system_clock_hz, int pwm_clk){
	reg_pwm_clk = (int)system_clock_hz /pwm_clk - 1;
}

/**
 * @brief     This fuction servers to set pwm count status(CMP) time.
 * @param[in] pwm_id  - variable of enum to select the pwm number.
 * @param[in] cmp_tick - variable of the CMP.
 * @return	  none.
 */
static inline void pwm_set_cmp(pwm_id id, unsigned short cmp_tick)
{
	reg_pwm_cmp(id) = cmp_tick;
}

/**
 * @brief     This fuction servers to set pwm cycle time.
 * @param[in] pwm_id - variable of enum to select the pwm number.
 * @param[in] cycle_tick - variable of the cycle time.
 * @return	  none.
 */
static inline void pwm_set_cycle(pwm_id id, unsigned short cycle_tick){
	reg_pwm_max(id) = cycle_tick;
}

/**
 * @brief     This fuction servers to set pwm cycle time & count status.
 * @param[in] pwm_id - variable of enum to select the pwm number.
 * @param[in] cycle_tick - variable of the cycle time.
 * @param[in] cmp_tick - variable of the CMP.
 * @return	  none.
 */
static inline void pwm_set_cycle_and_duty(pwm_id id, unsigned short cycle_tick, unsigned short cmp_tick){
	reg_pwm_cycle(id) = MASK_VAL(FLD_PWM_CMP, cmp_tick, FLD_PWM_MAX, cycle_tick);
}


/**
 * @brief     This fuction servers to set pwm cycle time & count status.
 * @param[in] cycle_tick - variable of the cycle time.
 * @param[in] cmp_tick - variable of the CMP.
 * @return	  none.
 */
static inline void pwm_set_pwm0_shadow_cycle_and_duty(unsigned short cycle_tick, unsigned short cmp_tick)
{
	reg_pwm_tcmp0_shadow = cmp_tick;
	reg_pwm_tmax0_shadow = cycle_tick;
}


/**
 * @brief     This fuction servers to set the pwm phase.
 * @param[in] pwm_id - variable of enum to select the pwm number.
 * @param[in] phase - variable of the pwm phase.
 * @return	  none.
 */
static inline void pwm_set_phase(pwm_id id, unsigned short phase){
	//PWM phase can not set in 825x now,  do nothing here
}

/**
 * @brief     This fuction servers to set the pwm pulse number.
 * @param[in] pwm_id - variable of enum to select the pwm number.
 * @param[in] pulse_num - variable of the pwm pulse number.
 * @return	  none.
 */
static inline void pwm_set_pulse_num(pwm_id id, unsigned short pulse_num){
	if(PWM0_ID == id){
		reg_pwm0_pulse_num = pulse_num;
	}

}

/**
 * @brief     This fuction servers to start the pwm.
 * @param[in] pwm_id - variable of enum to select the pwm number.
 * @return	  none.
 */
static inline void pwm_start(pwm_id id)
{

	if(PWM0_ID == id)
	{
		BM_SET(reg_pwm0_enable, BIT(0));
	}
	else{
		BM_SET(reg_pwm_enable, BIT(id));
	}
}

/**
 * @brief     This fuction servers to stop the pwm.
 * @param[in] pwm_id - variable of enum to select the pwm number.
 * @return	  none.
 */
static inline void pwm_stop(pwm_id id)
{
	if(PWM0_ID == id)
	{
		BM_CLR(reg_pwm0_enable, BIT(0));
	}
	else{
		BM_CLR(reg_pwm_enable, BIT(id));
	}
}

/**
 * @brief     This fuction servers to revert the PWMx.
 * @param[in] pwm_id - variable of enum to select the pwm number.
 * @return	  none.
 */
static inline void pwm_revert(pwm_id id)
{
	reg_pwm_invert |= BIT(id);
}

/**
 * @brief     This fuction servers to revert the PWMx_N.
 * @param[in] pwm_id - variable of enum to select the pwm number.
 * @return	  none.
 */
static inline void pwm_n_revert(pwm_id id)
{
	reg_pwm_n_invert |= BIT(id);
}

/**
 * @brief     This fuction servers to enable the pwm polarity.
 * @param[in] pwm_id - variable of enum to select the pwm number.
 * @param[in] en: 1 enable. 0 disable.
 * @return	  none.
 */
static inline void pwm_polo_enable(pwm_id id, int en)
{
	if(en){
		BM_SET(reg_pwm_pol, BIT(id));
	}else{
		BM_CLR(reg_pwm_pol, BIT(id));
	}
}

/**
 * @brief     This fuction servers to mask the pwm interrupt.
 * @param[in] irq - variable of enum to select the pwm interrupt source.
 * @return	  none.
 */
static inline void pwm_set_interrupt_enable(PWM_IRQ irq){
	if(irq == PWM_IRQ_PWM0_IR_FIFO){
		BM_SET(reg_pwm0_fifo_mode_irq_mask,BIT(0));
	}else{
	    BM_SET(reg_pwm_irq_mask, irq);
	}
}

/**
 * @brief     This fuction servers to clear the pwm mask.
 * @param[in] irq  - variable of enum to select the pwm interrupt source.
 * @return	  none.
 */
static inline void pwm_set_interrupt_disable(PWM_IRQ irq){
    if(irq == PWM_IRQ_PWM0_IR_FIFO){
		BM_CLR(reg_pwm0_fifo_mode_irq_mask,BIT(0));
	}else{
		BM_CLR(reg_pwm_irq_mask, irq);
	}
 }


/**
 * @brief     This fuction servers to clear the pwm interrupt status.
 * @param[in] irq  - variable of enum to select the pwm interrupt source.
 * @return	  none.
 */
static inline void pwm_clear_interrupt_status(PWM_IRQ status)
{
	if(status == PWM_IRQ_PWM0_IR_FIFO){
		reg_pwm0_fifo_mode_irq_sta = BIT(0);
	}else{
		reg_pwm_irq_sta = status;
	}
}

/**
 * @brief     This fuction serves to set trigger level of interrupt for IR FiFo mode
 * @param[in] trig_level - FIFO  num int trigger level
 * @return	  none
 */
static inline void pwm_ir_fifo_set_irq_trig_level(unsigned char trig_level)
{
	reg_pwm_ir_fifo_irq_trig_level = trig_level;
}

/**
 * @brief     This fuction serves to clear data in fifo. Only when pwm is in not active mode,
 * 			  it is possible to clear data in fifo.
 * @param[in] none
 * @return	  none
 */
static inline void pwm_ir_fifo_clr_data(void)
{
	reg_pwm_ir_clr_fifo_data |= FLD_PWM0_IR_FIFO_CLR_DATA;
}

/**
 * @brief     This fuction serves to get the number of data in fifo.
 * @param[in] none
 * @return	  the number of data in fifo
 */
static inline unsigned char pwm_ir_fifo_get_data_num(void)
{
	return (reg_pwm_ir_fifo_data_status&FLD_PWM0_IR_FIFO_DATA_NUM);
}

/**
 * @brief     This fuction serves to determine whether data in fifo is empty.
 * @param[in] none
 * @return	  yes: 1 ,no: 0;
 */
static inline unsigned char pwm_ir_fifo_is_empty(void)
{
	return (reg_pwm_ir_fifo_data_status&FLD_PWM0_IR_FIFO_EMPTY);
}

/**
 * @brief     This fuction serves to determine whether data in fifo is full.
 * @param[in] none
 * @return	  yes: 1 ,no: 0;
 */
static inline unsigned char pwm_ir_fifo_is_full(void)
{
	return (reg_pwm_ir_fifo_data_status&FLD_PWM0_IR_FIFO_FULL);
}

/**
 * @brief     This fuction serves to write data into FiFo
 * @param[in] pulse_num  - the number of pulse
 * @param[in] use_shadow - determine whether the configuration of shadow cmp and shadow max is used
 * 						   1: use shadow, 0: not use
 * @param[in] carrier_en - enable sending carrier, 1: enable, 0: disable
 * @return	  none
 */
static inline void pwm_ir_fifo_set_data_entry(unsigned short pulse_num, unsigned char use_shadow, unsigned char carrier_en)
{
	static unsigned char index=0;
	unsigned short cfg_data = pulse_num + ((use_shadow&BIT(0))<<14) + ((carrier_en&BIT(0))<<15);
	while(pwm_ir_fifo_is_full());
	reg_pwm_ir_fifo_dat(index) = cfg_data;
	index++;
	index&=0x01;
}

/**
 * @brief     This fuction serves to config the pwm's dma wave form.
 * @param[in] carrier_en - must 1 or 0.
 * @param[in] pulse_type - type of pwm0's pulse.
 * @param[in] pulse_num - the number of pulse.
 * @return	  none.
 */
static inline unsigned short pwm_config_dma_fifo_waveform(int carrier_en, Pwm0Pulse_SelectDef pulse_type,  unsigned short pulse_num)
{
	return  ( carrier_en<<15 | pulse_type | (pulse_num & 0x3fff) );
}

/**
 * @brief     This fuction servers to set the pwm's dma address.
 * @param[in] pdat - variable of pointer to indicate the address.
 * @return	  none.
 * @note	  The maximum length that the PWM can send is 511bytes
 */
static inline void pwm_set_dma_address(void * pdat)
{
	reg_dma_pwm_addr = (unsigned short)((unsigned int)pdat);
	reg_dma7_addrHi = 0x04;
	reg_dma_pwm_mode  &= ~FLD_DMA_WR_MEM;
	//In the PWM ir_dma_fifo model, the reg_dma7_size default is 0x14(20*16 = 320 bytes) (160 group configuration),
	//when the pwm_dma send byte length is greater than 320 bytes, can appear abnormal,
    //abnormal phenomenon: when after sending the first 160 group configuration waveform,
	//waveform will send 160th group configuration, and not interrupt,
	//so set reg_dma7_size to the maximum, it has been guaranteed that the maximum length supported by the hardware (511bytes) can be sent.
	//The maximum length that dma hardware can send is limited to the setting range of the first four bytes in ram (the actual length of dma sent)
	reg_dma7_size = 0xff;    
}

/**
 * @brief     This fuction servers to start the pwm's IRQ sending.
 * @param[in] none.
 * @return	  none.
 */
static inline void pwm_start_dma_ir_sending(void)
{
	reg_dma_chn_en |= FLD_DMA_CHN_PWM;
	reg_dma_tx_rdy0 |= FLD_DMA_CHN_PWM;
}

/**
 * @brief     This fuction servers to stop the pwm's IRQ sending.
 * @param[in] none.
 * @return	  none.
 */
static inline void pwm_stop_dma_ir_sending(void)
{
	reg_rst0 = FLD_RST0_PWM;
	sleep_us(20);  //1us <-> 4 byte
	reg_rst0 = 0;
}

/**
 * @brief     This fuction servers to get the pwm interrupt status.
 * @param[in] status - variable of enum to select the pwm interrupt source.
 * @return	  none.
 */
static inline unsigned char pwm_get_interrupt_status(PWM_IRQ status){

	if(status == PWM_IRQ_PWM0_IR_FIFO){
		return (reg_pwm0_fifo_mode_irq_sta & BIT(0));
	}else{
		return (reg_pwm_irq_sta & status);
	}

}

#endif /* PWM_H_ */
