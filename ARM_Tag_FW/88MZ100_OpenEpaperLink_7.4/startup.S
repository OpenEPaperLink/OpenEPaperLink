/* File: startup_ARMCM3.S
 * Purpose: startup file for Cortex-M3 devices. Should use with
 *   GCC for ARM Embedded Processors
 * Version: V2.0
 * Date: 16 August 2013
 *
/* Copyright (c) 2011 - 2013 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/
	.syntax	unified
	.arch	armv7-m

	.section .stack
	.align	4
#ifdef __STACK_SIZE
	.equ	Stack_Size, __STACK_SIZE
#else
	.equ	Stack_Size, 0x4000
#endif
	.globl	__StackTop
	.globl	__StackLimit
__StackLimit:
	.space	Stack_Size
	.size	__StackLimit, . - __StackLimit
__StackTop:
	.size	__StackTop, . - __StackTop

	.section .heap
	.align	4
#ifdef __HEAP_SIZE
	.equ	Heap_Size, __HEAP_SIZE
#else
	.equ	Heap_Size, 0
#endif
	.globl	__HeapBase
	.globl	__HeapLimit
__HeapBase:
	.if	Heap_Size
	.space	Heap_Size
	.endif
	.size	__HeapBase, . - __HeapBase
__HeapLimit:
	.size	__HeapLimit, . - __HeapLimit

	.section .isr_vector
	.align	2
	.globl	__isr_vector
__isr_vector:
	.long	__StackTop            /* Top of Stack */
	.long	Reset_Handler         /* Reset Handler */
	.long	NMIException           /* NMI Handler */
	.long	HardFaultException     /* Hard Fault Handler */
	.long	MemManageException     /* MPU Fault Handler */
	.long	BusFaultException      /* Bus Fault Handler */
	.long	UsageFaultException    /* Usage Fault Handler */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	SVCHandler           /* SVCall Handler */
	.long	DebugMonitor      /* Debug Monitor Handler */
	.long	0                     /* Reserved */
	.long	PendSVC        /* PendSV Handler */
	.long	SysTickHandler       /* SysTick Handler */
	.long	RTC_IRQHandler        /* RTC_IRQ Handler */
	.long	RC32K_IRQHandler                     /* Reserved */
	.long	XTAL32K_IRQHandler                     /* Reserved */
	.long	RC32M_IRQHandler      /* RC32M_IRQHandler */
	.long	XTAL64M_IRQHandler                     /* not Reserved */
	.long	RC32KCAL_IRQHandler                    /* Reserved */
	.long	BRNDET_IRQHandler                     /* Reserved */
	.long	LPCOMP_IRQHandler                     /* Reserved */
	.long	ADC_IRQHandler        /* ADC_IRQHandler */
	.long	DAC_IRQHandler                     /* Reserved */
	.long	ACOMP_IRQHandler                     /* Reserved */
	.long	CRC_IRQHandler                  /* Reserved */
	.long	AES_IRQHandler        /* AES_IRQHandler */
	.long	I2C1_IRQHandler                     /* Reserved */
	.long	I2C2_IRQHandler                   /* Reserved */
	.long	DMA_IRQHandler                    /* Reserved */
	.long	GPIO_IRQHandler       /* GPIO_IRQHandler */
	.long	SSP1_IRQHandler                     /* Reserved */
	.long	SSP2_IRQHandler                     /* Reserved */
	.long	QSPI_IRQHandler       /* QSPI_IRQHandler */
	.long	GPT1_IRQHandler                     /* Reserved */
	.long	GPT2_IRQHandler                    /* Reserved */
	.long	UART1_IRQHandler      /* UART0_IRQHandler */
	.long	UART2_IRQHandler      /* UART1_IRQHandler */
	.long	PHY1_IRQHandler    /* ZIGBEE_IRQ_Handler */
	.long	PHY2_IRQHandler                     /* Reserved */
	.long	PHY3_IRQHandler   /* ZIGBEE_IRQ1_Handler */
	.long	PHY4_IRQHandler                     /* Reserved */
	.long	THREEDG_IRQHandler                     /* Reserved */
	.long	KEYSCAN_IRQHandler                    /* Reserved */
	.long	IR_IRQHandler                     /* Reserved */
	.long	TRACKBALL_IRQHandler  /* GPIO0_WAKEUP_handler */
	.long	ExtPin1_IRQHandler  /* GPIO1_WAKEUP_handler */
	.long	ExtPin2_IRQHandler  /* GPIO2_WAKEUP_handler */
	.long	ExtPin3_IRQHandler  /* GPIO3_WAKEUP_handler */
	.long	ExtPin4_IRQHandler  /* GPIO4_WAKEUP_handler */
	.long	ExtPin5_IRQHandler  /* GPIO5_WAKEUP_handler */
	.long	ExtPin6_IRQHandler  /* GPIO6_WAKEUP_handler */
	.long	ExtPin7_IRQHandler  /* GPIO7_WAKEUP_handler */
	.long	ExtPin8_IRQHandler  /* GPIO8_WAKEUP_handler */
	.long	ExtPin9_IRQHandler  /* GPIO9_WAKEUP_handler */
	.long	ExtPin10_IRQHandler /* GPIO10_WAKEUP_handler */
	.long	ExtPin11_IRQHandler /* GPIO11_WAKEUP_handler */
	.long	ExtPin12_IRQHandler /* GPIO12_WAKEUP_handler */
	.long	ExtPin13_IRQHandler /* GPIO13_WAKEUP_handler */
	.long	ExtPin14_IRQHandler /* GPIO14_WAKEUP_handler */
	.long	ExtPin15_IRQHandler /* GPIO15_WAKEUP_handler */
	.long	ExtPin16_IRQHandler /* GPIO16_WAKEUP_handler */
	.long	ExtPin17_IRQHandler /* GPIO17_WAKEUP_handler */
	.long	ExtPin18_IRQHandler /* GPIO18_WAKEUP_handler */
	.long	ExtPin19_IRQHandler /* GPIO19_WAKEUP_handler */
	.long	ExtPin20_IRQHandler /* GPIO20_WAKEUP_handler */
	.long	ExtPin21_IRQHandler /* GPIO21_WAKEUP_handler */
	.long	ExtPin22_IRQHandler /* GPIO22_WAKEUP_handler */
	.long	ExtPin23_IRQHandler /* GPIO23_WAKEUP_handler */
	.long	ExtPin24_IRQHandler /* GPIO24_WAKEUP_handler */
	.long	ExtPin25_IRQHandler /* GPIO25_WAKEUP_handler */
	.long	ExtPin26_IRQHandler /* GPIO26_WAKEUP_handler */
	.long	ExtPin27_IRQHandler /* GPIO27_WAKEUP_handler */
	.long	ExtPin28_IRQHandler /* GPIO28_WAKEUP_handler */
	.long	ExtPin29_IRQHandler /* GPIO28_WAKEUP_handler */
	.long	ExtPin30_IRQHandler /* GPIO28_WAKEUP_handler */
	.long	ExtPin31_IRQHandler /* GPIO28_WAKEUP_handler */

	/* External interrupts */
	.long	Default_Handler
	.size	__isr_vector, . - __isr_vector

	.text
	.thumb
	.thumb_func
	.align	2
	.globl	Reset_Handler
	.type	Reset_Handler, %function
Reset_Handler:
/*  Firstly it copies data from read only memory to RAM. There are two schemes
 *  to copy. One can copy more than one sections. Another can only copy
 *  one section.  The former scheme needs more instructions and read-only
 *  data to implement than the latter.


/*  Single section scheme.
 *
 *  The ranges of copy from/to are specified by following symbols
 *    __etext: LMA of start of the section to copy from. Usually end of text
 *    __data_start__: VMA of start of the section to copy to
 *    __data_end__: VMA of end of the section to copy to
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
	ldr	r1, =__etext
	ldr	r2, =__data_start__
	ldr	r3, =__data_end__

.L_loop1:
	cmp	r2, r3
	ittt	lt
	ldrlt	r0, [r1], #4
	strlt	r0, [r2], #4
	blt	.L_loop1

	ldr	r3, =__StackTop
	mov	sp, r3

/*  This part of work usually is done in C library startup code. Otherwise,
 *  define this macro to enable it in this startup.
 *
 *  There are two schemes too. One can clear multiple BSS sections. Another
 *  can only clear one section. The former is more size expensive than the
 *  latter.
 *
 *  Define macro __STARTUP_CLEAR_BSS_MULTIPLE to choose the former.
 *  Otherwise efine macro __STARTUP_CLEAR_BSS to choose the later.
 */
/*  Single BSS section scheme.
 *
 *  The BSS section is specified by following symbols
 *    __bss_start__: start of the BSS section.
 *    __bss_end__: end of the BSS section.
 *
 *  Both addresses must be aligned to 4 bytes boundary.
 */
	ldr	r1, =__bss_start__
	ldr	r2, =__bss_end__

	movs	r0, 0
.L_loop3:
	cmp	r1, r2
	itt	lt
	strlt	r0, [r1], #4
	blt	.L_loop3

	bl	_start

	.pool
	.size	Reset_Handler, . - Reset_Handler

	.align	1
	.thumb_func
	.weak	Default_Handler
	.type	Default_Handler, %function
Default_Handler: // REPLACED IN MAIN.C to allow custom default irq
	b	.
	.size	Default_Handler, . - Default_Handler

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
	.macro	def_irq_handler	handler_name
	.weak	\handler_name
	.set	\handler_name, Default_Handler
	.endm

	def_irq_handler NMIException        
	def_irq_handler HardFaultException  
	def_irq_handler MemManageException  
	def_irq_handler BusFaultException   
	def_irq_handler UsageFaultException      
	def_irq_handler SVCHandler          
	def_irq_handler DebugMonitor       
	def_irq_handler PendSVC
	def_irq_handler SysTickHandler      
	def_irq_handler RTC_IRQHandler      
	def_irq_handler RC32K_IRQHandler    
	def_irq_handler XTAL32K_IRQHandler  
	def_irq_handler RC32M_IRQHandler    
	def_irq_handler XTAL64M_IRQHandler  
	def_irq_handler RC32KCAL_IRQHandler 
	def_irq_handler BRNDET_IRQHandler   
	def_irq_handler LPCOMP_IRQHandler   
	def_irq_handler ADC_IRQHandler      
	def_irq_handler DAC_IRQHandler      
	def_irq_handler ACOMP_IRQHandler    
	def_irq_handler CRC_IRQHandler      
	def_irq_handler AES_IRQHandler      
	def_irq_handler I2C1_IRQHandler     
	def_irq_handler I2C2_IRQHandler     
	def_irq_handler DMA_IRQHandler      
	def_irq_handler GPIO_IRQHandler     
	def_irq_handler SSP1_IRQHandler     
	def_irq_handler SSP2_IRQHandler     
	def_irq_handler QSPI_IRQHandler     
	def_irq_handler GPT1_IRQHandler     
	def_irq_handler GPT2_IRQHandler     
	def_irq_handler UART1_IRQHandler    
	def_irq_handler UART2_IRQHandler    
	def_irq_handler PHY1_IRQHandler
	def_irq_handler PHY2_IRQHandler     
	def_irq_handler PHY3_IRQHandler
	def_irq_handler PHY4_IRQHandler     
	def_irq_handler THREEDG_IRQHandler  
	def_irq_handler KEYSCAN_IRQHandler  
	def_irq_handler IR_IRQHandler       
	def_irq_handler TRACKBALL_IRQHandler
	def_irq_handler ExtPin1_IRQHandler  
	def_irq_handler ExtPin2_IRQHandler  
	def_irq_handler ExtPin3_IRQHandler  
	def_irq_handler ExtPin4_IRQHandler  
	def_irq_handler ExtPin5_IRQHandler  
	def_irq_handler ExtPin6_IRQHandler  
	def_irq_handler ExtPin7_IRQHandler  
	def_irq_handler ExtPin8_IRQHandler  
	def_irq_handler ExtPin9_IRQHandler  
	def_irq_handler ExtPin10_IRQHandler 
	def_irq_handler ExtPin11_IRQHandler 
	def_irq_handler ExtPin12_IRQHandler 
	def_irq_handler ExtPin13_IRQHandler 
	def_irq_handler ExtPin14_IRQHandler 
	def_irq_handler ExtPin15_IRQHandler 
	def_irq_handler ExtPin16_IRQHandler 
	def_irq_handler ExtPin17_IRQHandler 
	def_irq_handler ExtPin18_IRQHandler 
	def_irq_handler ExtPin19_IRQHandler 
	def_irq_handler ExtPin20_IRQHandler 
	def_irq_handler ExtPin21_IRQHandler 
	def_irq_handler ExtPin22_IRQHandler 
	def_irq_handler ExtPin23_IRQHandler 
	def_irq_handler ExtPin24_IRQHandler 
	def_irq_handler ExtPin25_IRQHandler 
	def_irq_handler ExtPin26_IRQHandler 
	def_irq_handler ExtPin27_IRQHandler 
	def_irq_handler ExtPin28_IRQHandler 
	def_irq_handler ExtPin29_IRQHandler 
	def_irq_handler ExtPin30_IRQHandler 
	def_irq_handler ExtPin31_IRQHandler 

	def_irq_handler	DEF_IRQHandler
	.end
