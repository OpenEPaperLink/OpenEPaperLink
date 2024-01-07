/********************************************************************************************************
 * @file	core.h
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
#ifndef CORE_H
#define CORE_H
#include "lib/include/sys.h"
#include "nds_intrinsic.h"
	/* Machine mode MHSP_CTL */
typedef enum{

	MHSP_CTL_OVF_EN     = BIT(0),//Enable bit for the stack overflow protection and recording mechanism.
	MHSP_CTL_UDF_EN     = BIT(1),//Enable bit for the stack underflow protection mechanism
	MHSP_CTL_SCHM_SEL   = BIT(2),//0-Stack overflow/underflow,1-Top-of-stack recording
	MHSP_CTL_U_EN       = BIT(3),//Enables the SP protection and recording mechanism in User mode
	MHSP_CTL_S_EN       = BIT(4),//Enables the SP protection and recording mechanism in Supervisor  mode
	MHSP_CTL_M_EN       = BIT(5),//Enables the SP protection and recording mechanism in Machine mode
} mhsp_ctl_e;

typedef enum
{
	FLD_MSTATUS_MIE = BIT(3),//M-mode interrupt enable bit
}mstatus_e;

typedef enum
{
	FLD_MIE_MSIE     = BIT(3),//M-mode software interrupt enable bit.
	FLD_MIE_MTIE     = BIT(7),//M-mode timer interrupt enable bit
	FLD_MIE_MEIE     = BIT(11),//M-mode external interrupt enable bit
}mie_e;



#define  read_csr(reg)		         __nds__csrr(reg)
#define  write_csr(reg, val)	      __nds__csrw(val, reg)
#define  swap_csr(reg, val)	          __nds__csrrw(val, reg)
#define set_csr(reg, bit)	         __nds__csrrs(bit, reg)
#define clear_csr(reg, bit)	         __nds__csrrc(bit, reg)

/*
 * Inline nested interrupt entry/exit macros
 */
/* Svae/Restore macro */
#define save_csr(r)             long __##r = read_csr(r);
#define restore_csr(r)           write_csr(r, __##r);
/* Support PowerBrake (Performance Throttling) feature */


#define save_mxstatus()         save_csr(NDS_MXSTATUS)
#define restore_mxstatus()      restore_csr(NDS_MXSTATUS)

 /* Nested IRQ entry macro : Save CSRs and enable global interrupt. */
#define core_save_nested_context()                              \
	 save_csr(NDS_MEPC)                              \
	 save_csr(NDS_MSTATUS)                           \
	 save_mxstatus()                                 \
	 set_csr(NDS_MSTATUS, FLD_MSTATUS_MIE);

/* Nested IRQ exit macro : Restore CSRs */
#define core_restore_nested_context()                               \
	 clear_csr(NDS_MSTATUS, FLD_MSTATUS_MIE);            \
	 restore_csr(NDS_MSTATUS)                        \
	 restore_csr(NDS_MEPC)                           \
	 restore_mxstatus()

#define fence_iorw	      	__nds__fence(FENCE_IORW,FENCE_IORW)

typedef enum{
	FLD_FEATURE_PREEMPT_PRIORITY_INT_EN = BIT(0),
	FLD_FEATURE_VECTOR_MODE_EN 			= BIT(1),
}
feature_e;


/**
 * @brief Disable interrupts globally in the system.
 * @return  r - the MIE bit value of machine status register(MSTATUS).
 * @note  this function must be used when the system wants to disable all the interrupt.
 * @return     none
 */
static inline unsigned int core_interrupt_disable(void){
	unsigned int r = read_csr (NDS_MSTATUS)&FLD_MSTATUS_MIE;
	if(r)
	{
		clear_csr(NDS_MSTATUS,FLD_MSTATUS_MIE);//global interrupts disable
	}
	return r;
}

/**
 * @brief restore interrupts globally in the system.
 * @param[in]  en - the MIE bit value of machine status register(MSTATUS).
 * this  parameter must be the return value of the core_interrupt_disable function.
 * @return     0
 * @note this function must be used when the system wants to restore all the interrupt.
 */
static inline unsigned int core_restore_interrupt(unsigned int en){
	if(en)
	{
		set_csr(NDS_MSTATUS,en);//global interrupts enable
	}
	return 0;
}

/**
 * @brief enable interrupts globally in the system.
 * @return  none
 */
static inline void core_interrupt_enable(void)
{
	set_csr(NDS_MSTATUS,FLD_MSTATUS_MIE);//global interrupts enable
	set_csr(NDS_MIE,FLD_MIE_MSIE |FLD_MIE_MTIE|FLD_MIE_MEIE);//machine Interrupt enable selectively
}

/**
 * @brief This function serves to set mhsp control registers.
 * @param[in]  ctl - the value of mhsp_ctl, refer to mhsp_ctl_e for the definition of MHSP_CTL
 * @return     none
 */
static inline void core_set_mhsp_ctr(mhsp_ctl_e ctl)
{
	write_csr(NDS_MHSP_CTL,(unsigned int)ctl);
}

/**
 * @brief This function serves to set hsp bound registers.
 *  ---Stack overflow---
 When the SP overflow detection mechanism is properly selected and enabled, any updated value to
 the SP register (via any instruction) is compared with the msp_bound register If the updated value
 to the SP register is smaller than the msp_bound register, a stack overflow exception is generated
 a stack overflow exception is generated.
 The stack overflow exception has an exception code of 32 in the mcause register
 ---Top-of-stack recording---
 When the top of stack recording mechanism is properly selected and enabled, any updated value to
 the SP register on any instruction is compared with the msp_bound register. If the updated value
 to the SP register is smaller than the msp_bound register, the msp_bound register is updated with this updated value
 * @param[in]  bound - the value of  hsp_bound .
 * @return     none
 */
static inline void core_set_msp_bound(unsigned int bound)
{
	write_csr(NDS_MSP_BOUND, bound);
}

/**
 * @brief This function serves to set hsp base registers.
 When the SP underflow detection mechanism is properly selected and enabled, any updated value to
 the SP register (via any instruction) is compared with the msp_base register. If the updated value to
 the SP register is greater than the msp_base register, a stack underflow exception is generated. The
 stack underflow exception has an exception code of 33 in the mcause register.
 * @param[in]  base - the value of hsp_base .
 * @return     none
 */
static inline void core_set_msp_base(unsigned int base)
{
	write_csr(NDS_MSP_BASE, base);
}

/**
 * @brief This function serves to get hsp bound registers.
 * ---Top-of-stack recording---
 When the top of stack recording mechanism is properly selected and enabled, any updated value to
 the SP register on any instruction is compared with the msp_bound register. If the updated value
 to the SP register is smaller than the msp_bound register, the msp_bound register is updated with this updated value,you can read this value.
 * @return     none
 */
static inline unsigned int core_get_msp_bound(void)
{
	return read_csr(NDS_MSP_BOUND);
}

/**
 * @brief This function serves to get hsp base registers.there is no recording.
 * @return     none
 */
static inline unsigned int core_get_msp_base(void)
{
	return read_csr(NDS_MSP_BASE);
}

/**
 * @brief This function serves to get current sp(Stack pointer).
 * @return     none
 */
static inline unsigned int core_get_current_sp(void)
{
	return __nds__get_current_sp();
}

/**
 * @brief This function serves to get mcause(Machine Cause) value.
 * This register indicates the cause of trap, reset, NMI or the interrupt source ID of a vector interrupt.
   This register is updated when a trap, reset, NMI or vector interrupt occurs
 * @return     none
 */
static inline unsigned int core_get_mcause(void)
{
	return read_csr(NDS_MCAUSE);
}

/**
 * @brief This function serves to get mepc(Machine Exception Program Counter) value.
 * When entering an exception, the hardware will automatically update the value of the mepc register
 * to the value of the instruction pc currently encountered with the exception
 * @return     none
 */
static inline unsigned int core_get_mepc(void)
{
	return read_csr(NDS_MEPC);
}
/**
 * @brief 	 This function serves to mcu entry wfi(Wait-For-Interrupt) mode similar to stall mcu.
 * @return	 none
 * @Note:	 there are two awoke modes by interrupt:
 *           1).When the processor is awoken by a pending interrupt and global interrupts enable(mstatus.MIE is enabled),it will resume and start to execute from the corresponding interrupt service routine.
 *           2).When the core is awoken by a pending interrupt and global interrupts enable (mstatus.MIE is disabled),it will resume and start to execute from the instruction after the WFI instruction.
 */
static inline void  core_entry_wfi_mode(void)
{
 /* Interrupts disabled by the mie CSR will not be able to wake up the processor.
   However,the processor can be awoken by these interrupts regardless the value of the global interrupt enable bit (mstatus.MIE)*/
   __asm__ __volatile__("wfi");

}

/**
 * @brief     This function serves to get current pc.
 * @return    current pc
 */
static inline  unsigned int core_get_current_pc(void)
{
	unsigned int current_pc=0;
	__asm__ ("auipc %0, 0":"=r"(current_pc)::"a0");
	return current_pc;
}
#endif
