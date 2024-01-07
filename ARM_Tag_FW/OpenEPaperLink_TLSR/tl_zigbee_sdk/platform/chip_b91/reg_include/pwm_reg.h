/********************************************************************************************************
 * @file	pwm_reg.h
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
#ifndef PWM_REG_H
#define PWM_REG_H

#include "soc.h"



/*******************************      pwm registers: 0x140400      ******************************/
#define reg_pwm_data_buf_adr        0x80140448


/**
 * Represents the base address register of the PWM
 */
#define 	REG_PWM_BASE		    0x140400


/**
 * This register is used to enable PWM0~PWM5.
 */
#define reg_pwm_enable			REG_ADDR16(REG_PWM_BASE)
typedef enum{
	FLD_PWM1_EN = BIT(1),
	FLD_PWM2_EN = BIT(2),
	FLD_PWM3_EN = BIT(3),
	FLD_PWM4_EN = BIT(4),
	FLD_PWM5_EN = BIT(5),
	FLD_PWM0_EN = BIT(8),
}pwm_en_e;



/**
 * This register is used to set the division factor of the PWM clock.
 */
#define reg_pwm_clkdiv			REG_ADDR8(REG_PWM_BASE+0x02)


/**
 * BIT[3:0] of this register is used to set the working mode of PWM0.Only PWM0 supports 5 modes.
 */
#define reg_pwm0_mode           REG_ADDR8(REG_PWM_BASE+0x03)


/**
 * This register is used to set the inversion of the output state of PWM5 ~ PWM0.
 * PWM and PWM_N can be inverted independently
 */
#define reg_pwm_invert          REG_ADDR8(REG_PWM_BASE+0x04)
enum{
	FLD_PWM0_OUT_INVERT  = 		BIT(0),
	FLD_PWM1_OUT_INVERT  = 		BIT(1),
	FLD_PWM2_OUT_INVERT  = 		BIT(2),
	FLD_PWM3_OUT_INVERT  = 		BIT(3),
	FLD_PWM4_OUT_INVERT  = 		BIT(4),
	FLD_PWM5_OUT_INVERT  = 		BIT(5),
};


/**
 * This register is used to set the inversion of the output state of PWM5_N ~ PWM0_N.
 * PWM and PWM_N can be inverted independently
 */
#define reg_pwm_n_invert        REG_ADDR8(REG_PWM_BASE+0x05)
enum{
	FLD_PWM0_INV_OUT_INVERT  = 		BIT(0),
	FLD_PWM1_INV_OUT_INVERT  = 		BIT(1),
	FLD_PWM2_INV_OUT_INVERT  = 		BIT(2),
	FLD_PWM3_INV_OUT_INVERT  = 		BIT(3),
	FLD_PWM4_INV_OUT_INVERT  = 		BIT(4),
	FLD_PWM5_INV_OUT_INVERT  = 		BIT(5),
};


/*
 * This register represents Signal frame polarity of PWM5~PWM0.By default, PWM outputs high level under Count status and low level under Remaining status.
 * If the corresponding bit is set to 1, the high and low levels in different states will be reversed.
 * BIT(0):pwm0 out low level first. ~BIT(0):pwm0 out high level first.
 * BIT(1):pwm1 out low level first. ~BIT(1):pwm1 out high level first.
 * BIT(2):pwm2 out low level first. ~BIT(2):pwm2 out high level first.
 * BIT(3):pwm3 out low level first. ~BIT(3):pwm3 out high level first.
 * BIT(4):pwm4 out low level first. ~BIT(4):pwm4 out high level first.
 * BIT(5):pwm5 out low level first. ~BIT(5):pwm5 out high level first.
 */
#define reg_pwm_pol			    REG_ADDR8(REG_PWM_BASE+0x06)
enum{
	FLD_PWM0_FIRST_OUT_LEVEL  = 	BIT(0),
	FLD_PWM1_FIRST_OUT_LEVEL  = 	BIT(1),
	FLD_PWM2_FIRST_OUT_LEVEL  = 	BIT(2),
	FLD_PWM3_FIRST_OUT_LEVEL  = 	BIT(3),
	FLD_PWM4_FIRST_OUT_LEVEL  = 	BIT(4),
	FLD_PWM5_FIRST_OUT_LEVEL  = 	BIT(5),
};


/*
 * This register represents 32K clock source with PWM5 ~ PWM0 enabled
 * If the system has a 32K clock source, whether it is 32K_RC or 32K_Crystal, as long as the corresponding
 * BIT of 0x140407 is configured, the corresponding PWM Channel can get 32K clock source.
 */
#define reg_pwm_mode32k         REG_ADDR8(REG_PWM_BASE+0x07)
enum{
	FLD_PWM0_32K_ENABLE       = 	BIT(0),
	FLD_PWM1_32K_ENABLE		  = 	BIT(1),
	FLD_PWM2_32K_ENABLE 	  = 	BIT(2),
	FLD_PWM3_32K_ENABLE		  = 	BIT(3),
	FLD_PWM4_32K_ENABLE		  = 	BIT(4),
	FLD_PWM5_32K_ENABLE       = 	BIT(5),
};


/**
 * This register configures the length of the capture segment of PWM5 ~ PWM0.
 * This value has a total of 16 bits, divided into lower 8 bits and higher 8 bits.
 */
#define reg_pwm_cmp(i)		    REG_ADDR16(REG_PWM_BASE+0x14 +(i << 2))


/**
 * This register is used to configure the period of the PWM waveform. There are 32 bits in total.
 * The lower 16 bits indicate the length of the CMP segment. The higher 16 bits indicate the length of the MAX segment.
 */
#define reg_pwm_cycle(i)		REG_ADDR32(REG_PWM_BASE+0x14 + (i << 2))
//enum{
//	FLD_PWM_CMP  = 				BIT_RNG(0,15),
//	FLD_PWM_MAX  = 				BIT_RNG(16,31),
//};
// in C99 FLD_PWM_MAX  = BIT_RNG(16,31) is error
#define	FLD_PWM_CMP  = 				BIT_RNG(0,15),
#define	FLD_PWM_MAX  = 				BIT_RNG(16,31),
/**
 * This register configures the length of the max segment of PWM5 ~ PWM0.
 * This value has a total of 16 bits, divided into lower 8 bits and higher 8 bits.
 */
#define reg_pwm_max(i)			REG_ADDR16(REG_PWM_BASE+0x16 + (i << 2))


/**
 * When PWM0 is in count mode or ir mode, the total number of pulse_number is set by the following two registers.
 */
#define reg_pwm0_pulse_num		REG_ADDR16(REG_PWM_BASE+0x2c)//0x2c[7:0] 0x2d[5:0]



/**
 * This register is used to configure the PWM interrupt function.
 * BIT[0]:If this bit is set, an interrupt will be generated after a set of pulses has been sent. When this interrupt is enabled, you can capture an interrupt after a pulse is sent by detecting whether bit[0] of 0x140431 is set.
 * BIT[1]:Enable ir dma fifo mode interrupt.This bit is usually used with 0x140431BIT[1].
 * BIT[2]:Enable pwm0 frame interrupt.
 * BIT[3]:Enable pwm1 frame interrupt.
 * BIT[4]:Enable pwm2 frame interrupt.
 * BIT[5]:Enable pwm3 frame interrupt.
 * BIT[6]:Enable pwm4 frame interrupt.
 * BIT[7]:Enable pwm5 frame interrupt.
 * BIT[16]:The Bit is to enable the mask_lvl(This level specifically indicates the number of bytes in the FIFO that can trigger an interrupt) interrupt.
 */
#define reg_pwm_irq_mask(i)		REG_ADDR8(REG_PWM_BASE+0x30+i*2)


/**
 * The bits in this register are used to indicate the various interrupt states of the PWM.
 * BIT[0]:This bit is usually used with BIT[0] of 0x140430. If this bit is set to 1, it means that a pulse group of PWM has been sent.An interrupt was also generated. You can manually write 1 to clear the interrupt flag.
 * BIT[1]:In ir dma fifo mode. If this is set 1 Indicates that a set of pulse of group has been sent. You can manually write 1 to clear the interrupt flag.
 * BIT[2]:If this is set 1 Indicates that a signal frame interrupt has been generated. You can manually write 1 to clear the interrupt flag.
 * BIT[3]:If this is set 1 Indicates that a signal frame interrupt has been generated. You can manually write 1 to clear the interrupt flag.
 * BIT[4]:If this is set 1 Indicates that a signal frame interrupt has been generated. You can manually write 1 to clear the interrupt flag.
 * BIT[5]:If this is set 1 Indicates that a signal frame interrupt has been generated. You can manually write 1 to clear the interrupt flag.
 * BIT[6]:If this is set 1 Indicates that a signal frame interrupt has been generated. You can manually write 1 to clear the interrupt flag.
 * BIT[7]:If this is set 1 Indicates that a signal frame interrupt has been generated. You can manually write 1 to clear the interrupt flag.
 * BIT[16]:When the FIFO value is less than the set value, an interrupt is generated(The premise is that this interrupt has been enabled by register 0x140432 previous).
 *         The user can know whether this interrupt is generated by reading the status of this register.If BIT(16):1 Indicates that this interrupt has been generated.
 */
#define reg_pwm_irq_sta(i)		        REG_ADDR8(REG_PWM_BASE+0x31+i*2)

typedef enum{
	FLD_PWM0_PNUM_IRQ 			         =		BIT(0),
	FLD_PWM0_IR_DMA_FIFO_IRQ 	  		 =		BIT(1),
	FLD_PWM0_FRAME_DONE_IRQ              =		BIT(2),
	FLD_PWM1_FRAME_DONE_IRQ              =		BIT(3),
	FLD_PWM2_FRAME_DONE_IRQ 		  	 =		BIT(4),
	FLD_PWM3_FRAME_DONE_IRQ             =		BIT(5),
	FLD_PWM4_FRAME_DONE_IRQ             =		BIT(6),
	FLD_PWM5_FRAME_DONE_IRQ             =		BIT(7),

	FLD_PWM0_IR_FIFO_IRQ 	  	        =        BIT(16),

}pwm_irq_e;




/**
 * PWM0~PWM5, counting from 0 at the beginning of each cycle, and adding 1 for each system clock.
 */
#define reg_pwm_cnt(i)		    REG_ADDR16(REG_PWM_BASE+0x34 +(i << 1))

/*
 * when update the duty cycle in 32K, this register bit(0) set 1.
 */
#define reg_pwm_cnt5_l      REG_ADDR8(REG_PWM_BASE+0x3e)

enum{
	FLD_PWM_32K_DUTY_CYCLE_UPDATE   =   BIT(0),
};

/**
 * In count mode, each period is incremented by one.
 */
#define reg_pwm_ncnt		    REG_ADDR16(REG_PWM_BASE+0x40)




/**
 * [7:0] bits 7-0 of PWM0's high time or low time(if pola[0]=1),if shadow bit(fifo data[14]) is 1 in ir fifo mode or dma fifo mode.
 */
#define reg_pwm_tcmp0_shadow	REG_ADDR16(REG_PWM_BASE+0x44)


/**
 * [15:8] bits 15-8 of PWM0's high time or low time(if pola[0]=1),if shadow bit(fifo data[14]) is 1 in ir fifo mode or dma fifo mode.
 */
#define reg_pwm_tmax0_shadow	REG_ADDR16(REG_PWM_BASE+0x46)


/**
 * PWM data fifo.0x140448~0x14044b.
 */
#define reg_pwm_ir_fifo_dat(i)			REG_ADDR16(REG_PWM_BASE+0x48+i*2)


/**
 * This register BIT[3:0] indicates the interrupt trigger level in ir_fifo mode.
 * When fifo numbers is less than this value.It's will take effect.
 */
#define reg_pwm_ir_fifo_irq_trig_level	REG_ADDR8(REG_PWM_BASE+0x4c)
enum{
	FLD_PWM0_FIFO_NUM_OF_TRIGGLE_LEVEL 	=		BIT_RNG(0,3),
};


/**
 * This register indicates the fifo data status in.
 * BIT[3:0]:Indicates the amount of data in the FIFO.
 * BIT[4]:if BIT[4]=1,Indicates the data fifo is empty.
 * BIT[5]:if BIT[5]=1,Indicates the data fifo is full.
 */
#define reg_pwm_ir_fifo_data_status		REG_ADDR8(0x14044d)
enum{
	FLD_PWM0_IR_FIFO_DATA_NUM 	=		BIT_RNG(0,3),
	FLD_PWM0_IR_FIFO_EMPTY 		=		BIT(4),
	FLD_PWM0_IR_FIFO_FULL 		=		BIT(5),
};


/**
 * This register can be configured to clear the data FIFO.
 * BIT[0]:if BIT[0]=1,Indicates the data FIFO is clear.
 */
#define reg_pwm_ir_clr_fifo_data		REG_ADDR8(0x14044e)
enum{
	FLD_PWM0_IR_FIFO_CLR_DATA 	=		BIT(0),

};





#endif









