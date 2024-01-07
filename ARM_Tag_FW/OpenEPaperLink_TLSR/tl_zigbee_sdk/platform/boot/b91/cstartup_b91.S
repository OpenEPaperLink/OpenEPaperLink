/********************************************************************************************************
 * @file    cstartup_b91.S
 *
 * @brief   This is the boot file for B91
 *
 * @author  Driver & Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#ifdef MCU_CORE_B91

#include "version_cfg.h"

    .section .vectors, "ax"

    //.org  and linker's relaxation (-flto) cannot be used at the same time
    //Pop corresponds to push. Before using .option norelax, use push to save the current .option configuration
    //and then modify .option. After using norelax, use pop to restore
	.option push
	.option norelax
    .org 0x0

	.extern FILE_VERSION
	.extern MANUFACTURER_CODE_TELINK
	.extern IMAGE_TYPE

    .global _RESET_ENTRY
 	.type _RESET_ENTRY,@function

    .align 2
_RESET_ENTRY:
	j 		 _START

	.word (FILE_VERSION)

	//free the 6th ~ 7th byte to store the crc type of the bin file

	.org 0x12
	.short (MANUFACTURER_CODE_TELINK)
	.short (IMAGE_TYPE)

    .org 0x18
    .word (BIN_SIZE)

    .org 0x20
    .word ('T'<<24 | 'L'<<16 | 'N'<<8 | 'K')

    .org 0x26
    //.short (0x0003)           //READ:    cmd:1x, addr:1x, data:1x, dummy cycle:0
    //.short (0x070B)           //FREAD:   cmd:1x, addr:1x, data:1x, dummy cycle:8
    .short (0x173B)           //DREAD:   cmd:1x, addr:1x, data:2x, dummy cycle:8
    //.short (0x53BB)           //X2READ:  cmd:1x, addr:2x, data:2x, dummy cycle:4
    //.short (0x276B)           //QREAD:   cmd:1x, addr:1x, data:4x, dummy cycle:8
    //.short (0x65EB)             //X4READ:  cmd:1x, addr:4x, data:4x, dummy cycle:6
    .option pop
    .align 2

_START:

#if 0
	// add debug, PB4 output 1
	lui		t0,0x80140 			//0x8014030a
	li		t1, 0xef
	li		t2, 0x10
	sb		t1 , 0x30a(t0)		 //0x8014030a  PB oen     =  0xef
	sb		t2 , 0x30b(t0)		 //0x8014030b  PB output  =  0x10
#endif
	/* Initialize global pointer */
    .option push
    .option norelax
    la     gp, __global_pointer$
    .option pop

/*
* swire usb enable default.when dp and dm use as GPIO, it has a risk to trigger the swire through usb function
* to do something unexcept when toggle dp pin
*/
_SWS_USB_DIS:
	lui		t0, 0x80100
	addi	t0 , t0 , 0x700
	li		t1, 0x40
	sb		t1, 0x501(t0)  //0x80100c01 -> 0x40  <7>: 0 swire_usb_dis

	/* Initialize stack pointer */
    la     t0, _STACK_TOP
    mv     sp, t0

#ifdef __nds_execit
	/* Initialize EXEC.IT table */
	la t0, _ITB_BASE_
	csrw uitb, t0
#endif

#ifdef __riscv_flen
	/* Enable FPU */
	li t0, 0x00006000
	csrrs t0, mstatus, t0
	/* Initialize FCSR */
	fscsr zero
#endif

    /* Initial machine trap-vector Base */
    la     t0, __vectors
    csrw   mtvec, t0

    /* Enable vectored external plic interrupt */
    csrsi  mmisc_ctl, 2

	/*vector mode enable bit (VECTORED) of the Feature Enable Register */
    lui		t0, 0xe4000
	li		t1, 0x02
	sw		t1, 0x0(t0)		 //(*(volatile unsigned long*)(0xe4000000))= 0x02

    /* Enable I/D-Cache */
    csrr   t0,  mcache_ctl
    ori    t0,  t0,  1  #/I-Cache
    ori    t0,  t0,  2  #/D-Cache
    csrw   mcache_ctl,  t0
    fence.i
	/* Move retention reset from flash to sram */
_RETENTION_RESET_INIT:

    la     t1, _RETENTION_RESET_LMA_START
    la     t2, _RETENTION_RESET_VMA_START
    la     t3, _RETENTION_RESET_VMA_END
_RETENTION_RESET_BEGIN:
    bleu    t3, t2, _ZERO_AES
    lw     t0, 0(t1)
    sw     t0, 0(t2)
    addi   t1, t1, 4
    addi   t2, t2, 4
    j      _RETENTION_RESET_BEGIN

	/* Zero .aes section in sram */
_ZERO_AES:
    lui    t0, 0
    la     t2, _AES_VMA_START
    la     t3, _AES_VMA_END
_ZERO_AES_BEGIN:
    bleu   t3, t2, _RAMCODE_INIT
    sw     t0, 0(t2)
    addi   t2, t2, 4
    j      _ZERO_AES_BEGIN

#if 0
	/* Move retention from flash to sram */
_RETENTION_DATA_INIT:

    la     t1, _RETENTION_DATA_LMA_START
    la     t2, _RETENTION_DATA_VMA_START
    la     t3, _RETENTION_DATA_VMA_END
_RETENTION_DATA_INIT_BEGIN:
    bleu   t3, t2, _RAMCODE_INIT
    lw     t0, 0(t1)
    sw     t0, 0(t2)
    addi   t1, t1, 4
    addi   t2, t2, 4
    j      _RETENTION_DATA_INIT_BEGIN
#endif

	/* Move ramcode from flash to sram */
_RAMCODE_INIT:

    la     t1, _RAMCODE_LMA_START
    la     t2, _RAMCODE_VMA_START
    la     t3, _RAMCODE_VMA_END
_RAMCODE_INIT_BEGIN:
    bleu    t3, t2, _DATA_INIT
    lw     t0, 0(t1)
    sw     t0, 0(t2)
    addi   t1, t1, 4
    addi   t2, t2, 4
    j      _RAMCODE_INIT_BEGIN


	/* Move Data from flash to sram */
_DATA_INIT:
    la     t1, _DATA_LMA_START
    la     t2, _DATA_VMA_START
    la     t3, _DATA_VMA_END
_DATA_INIT_BEGIN:
    bleu   t3, t2, _ZERO_BSS
    lw     t0, 0(t1)
    sw     t0, 0(t2)
    addi   t1, t1, 4
    addi   t2, t2, 4
    j      _DATA_INIT_BEGIN

	/* Zero .bss section in sram */
_ZERO_BSS:
    lui    t0, 0
    la     t2, _BSS_VMA_START
    la     t3, _BSS_VMA_END
_ZERO_BSS_BEGIN:
    bleu   t3, t2, _FILL_STK
    sw     t0, 0(t2)
    addi   t2, t2, 4
    j      _ZERO_BSS_BEGIN

	/* Fill the remaining section in sram */
_FILL_STK:
#if 0
    lui    t0, 0x55555
    addi   t0, t0, 0x555
    la     t2, _BSS_VMA_END
    la     t3, _STACK_TOP
_FILL_STK_BEGIN:
    bleu   t3, t2, _MAIN_FUNC
    sw     t0, 0(t2)
    addi   t2, t2, 4
    j      _FILL_STK_BEGIN
#endif
	/* Jump to the main function */
_MAIN_FUNC:
    nop

    la     t0, main
    jalr   t0

    nop
    nop
    nop
    nop
    nop
_END:
    j    _END


 .section .retention_reset, "ax"
    .option push
    .option norelax
    .global _IRESET_ENTRY
 	.type _IRESET_ENTRY,@function

    .align 2
_IRESET_ENTRY:
	/* Decide whether this is an NMI or cold reset */
	j 		_ISTART

	.org 0x22
_ISTART:
#if 0
	// add debug, PB4 output 1
	lui		t0,0x80140 			//0x8014030a
	li		t1, 0xef
	li		t2, 0x10
	sb		t1 , 0x30a(t0)		 //0x8014030a  PB oen     =  0xef
	sb		t2 , 0x30b(t0)		 //0x8014030b  PB output  =  0x10
#endif
	/* Initialize global pointer */

    la     gp, __global_pointer$
    .option pop

/*
* swire usb enable default.when dp and dm use as GPIO, it has a risk to trigger the swire through usb function
* to do something unexcept when toggle dp pin
*/
_ISWS_USB_DIS:
	lui		t0, 0x80100
	addi	t0 , t0 , 0x700
	li		t1, 0x40
	sb		t1, 0x501(t0)  //0x80100c01 -> 0x40  <7>: 0 swire_usb_dis

	/* Initialize stack pointer */
    la     t0, _STACK_TOP
    mv     sp, t0

#ifdef __nds_execit
	/* Initialize EXEC.IT table */
	la t0, _ITB_BASE_
	csrw uitb, t0
#endif

#ifdef __riscv_flen
	/* Enable FPU */
	li t0, 0x00006000
	csrrs t0, mstatus, t0
	/* Initialize FCSR */
	fscsr zero
#endif

    /* Initial machine trap-vector Base */
    la     t0, __vectors
    csrw   mtvec, t0

    /* Enable vectored external plic interrupt */
    csrsi  mmisc_ctl, 2

	/*vector mode enable bit (VECTORED) of the Feature Enable Register */
    lui		t0, 0xe4000
	li		t1, 0x02
	sw		t1, 0x0(t0)		 //(*(volatile unsigned long*)(0xe4000000))= 0x02

    /* Enable I/D-Cache */
    csrr   t0,  mcache_ctl
    ori    t0,  t0,  1  #/I-Cache
    ori    t0,  t0,  2  #/D-Cache
    csrw   mcache_ctl,  t0
    fence.i


/* flash wakeup */
_WAKEUP_FLASH:
	lui		t0 , 0x80140
	li		t1 , 0xff
	li		t2 , 0x0
	li		t3 , 0xab
	sb		t1 , 0x329(t0)			 //mspi ie enable	:0x140329:0x1f
	sb		t2 , 0x101(t0)			 //cs_low			:0x140101:0x00
	sb		t3 , 0x100(t0)			 //wakeup_cmd		:0x140100:0xab
_MSPI_WAIT:
	lui		t0 , 0x80140
	lb		t2 , 0x102(t0)			//read reg_mspi_status FLD_MSPI_BUSY(bit0)
	li		t3 , 0x1
	li		t4 , 0x10
	beq 	t3 , t2 ,_MSPI_WAIT
	sb		t4 , 0x101(t0)			//cs_high			:0x140101:0x10
#if 0
	// add debug, PB4 output 1
	lui		t0,0x80140 			//0x8014030a
	li		t1, 0x00
	sb		t1 , 0x30b(t0)		 //0x8014030b  PB output  =  0x00
#endif
/*efuse load need delay about 18us */
	li		t0 , 0
	li		t1 , 226
_WAIT_EFUSE_LOAD_FINISH:
	addi 	t0 , t0 , 1
	bgeu 	t1 , t0 , _WAIT_EFUSE_LOAD_FINISH

#if 0
	// add debug, PB4 output 1
	lui		t0,0x80140 			//0x8014030a
	li		t1, 0x10
	sb		t1 , 0x30b(t0)		 //0x8014030b  PB output  =  0x00
#endif
_MULTI_ADDRESS_BEGIN:
	lui		t0 , 0x80140
	la		t1 , g_pm_mspi_cfg
	lw		t2 , 0(t1)
	sw		t2 , 0x104(t0)			 //g_pm_mspi_cfg->0x80140104

	/* Zero .aes section in sram */
_IZERO_AES:
    lui    t0, 0
    la     t2, _AES_VMA_START
    la     t3, _AES_VMA_END
_IZERO_AES_BEGIN:
    bleu   t3, t2, _IDATA_INIT
    sw     t0, 0(t2)
    addi   t2, t2, 4
    j      _IZERO_AES_BEGIN

#if 0
	/* Move ramcode from flash to sram */
_IRAMCODE_INIT:

    la     t1, _RAMCODE_LMA_START
    la     t2, _RAMCODE_VMA_START
    la     t3, _RAMCODE_VMA_END
_IRAMCODE_INIT_BEGIN:
    bleu   t3, t2, _IDATA_INIT
    lw     t0, 0(t1)
    sw     t0, 0(t2)
    addi   t1, t1, 4
    addi   t2, t2, 4
    j      _IRAMCODE_INIT_BEGIN
#endif

	/* Move Data from flash to sram */
_IDATA_INIT:
#if 0
    la     t1, _DATA_LMA_START
    la     t2, _DATA_VMA_START
    la     t3, _DATA_VMA_END
_IDATA_INIT_BEGIN:
    bleu   t3, t2, _IZERO_BSS
    lw     t0, 0(t1)
    sw     t0, 0(t2)
    addi   t1, t1, 4
    addi   t2, t2, 4
    j      _IDATA_INIT_BEGIN

	/* Zero .bss section in sram */
_IZERO_BSS:
    lui    t0, 0
    la     t2, _BSS_VMA_START
    la     t3, _BSS_VMA_END
_IZERO_BSS_BEGIN:
    bleu   t3, t2, _IFILL_STK
    sw     t0, 0(t2)
    addi   t2, t2, 4
    j      _IZERO_BSS_BEGIN
#endif

	/* Fill the remaining section in sram */
_IFILL_STK:
#if 0
    lui    t0, 0x55555
    addi   t0, t0, 0x555
    la     t2, _BSS_VMA_END
    la     t3, _STACK_TOP
_IFILL_STK_BEGIN:
    bleu   t3, t2, _IMAIN_FUNC
    sw     t0, 0(t2)
    addi   t2, t2, 4
    j      _IFILL_STK_BEGIN
#endif
	/* Jump to the main function */
_IMAIN_FUNC:
    nop

    la     t0, main
    jalr   t0

    nop
    nop
    nop
    nop
    nop
_IEND:
    j    _IEND


.text
	.global default_irq_entry
	.align 2

default_irq_entry:
1:	j 1b

       .weak trap_handler

 trap_handler:
1:	j 1b

	.macro INTERRUPT num
	.weak entry_irq\num
	.set entry_irq\num, default_irq_entry
	.long entry_irq\num
	.endm

#define VECTOR_NUMINTRS         63

   	.section .ram_code, "ax"

	.global __vectors
	.balign 256


__vectors:
	/* Trap vector */
	.long trap_entry

	/* PLIC interrupt vector */
	.altmacro
	.set irqno, 1
	.rept VECTOR_NUMINTRS/*  .rept  .endr  */
	INTERRUPT %irqno
	.set irqno, irqno+1
	.endr

#endif /* MCU_CORE_B91 */
