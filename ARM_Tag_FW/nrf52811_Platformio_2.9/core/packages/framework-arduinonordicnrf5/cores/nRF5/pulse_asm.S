/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*

The following function has been compiled to ASM with gcc

    unsigned long countPulseASM(const volatile uint32_t *port, uint32_t bit, uint32_t stateMask, unsigned long maxloops)
    {
      unsigned long width = 0;

      // wait for any previous pulse to end
      while ((*port & bit) == stateMask)
        if (--maxloops == 0)
          return 0;

      // wait for the pulse to start
      while ((*port & bit) != stateMask)
        if (--maxloops == 0)
          return 0;

      // wait for the pulse to stop
      while ((*port & bit) == stateMask) {
        if (++width == maxloops)
          return 0;
      }
      return width;
    }

using the command line:

    arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -c -Os -W -ffunction-sections -fdata-sections \
                      -nostdlib --param max-inline-insns-single=500 -fno-exceptions -MMD \
                      -DF_CPU=48000000L -DARDUINO=10602 -DARDUINO_SAMD_ZERO -DARDUINO_ARCH_SAMD \
                      -D__SAMD21G18A__ -DUSB_VID=0x2341 -DUSB_PID=0x004d -DUSBCON \
                      -DUSB_MANUFACTURER="Arduino LLC" -DUSB_PRODUCT="Arduino Zero" \
                      -I/Code/arduino/build/linux/work/hardware/tools/CMSIS/CMSIS/Include/ \
                      -I/Code/arduino/build/linux/work/hardware/tools/CMSIS/Device/ATMEL/ \
                      -I/Code/arduino/build/linux/work/hardware/arduino/samd/cores/arduino \
                      -I/Code/arduino/build/linux/work/hardware/arduino/samd/variants/arduino_zero \
                      count.c -Wa,-ahlmsd=output.lst -dp -fverbose-asm -S

The result has been slightly edited to increase readability.

*/

	.cpu cortex-m0plus
	.fpu softvfp
	.eabi_attribute 20, 1	@ Tag_ABI_FP_denormal
	.eabi_attribute 21, 1	@ Tag_ABI_FP_exceptions
	.eabi_attribute 23, 3	@ Tag_ABI_FP_number_model
	.eabi_attribute 24, 1	@ Tag_ABI_align8_needed
	.eabi_attribute 25, 1	@ Tag_ABI_align8_preserved
	.eabi_attribute 26, 1	@ Tag_ABI_enum_size
	.eabi_attribute 30, 4	@ Tag_ABI_optimization_goals
	.eabi_attribute 34, 0	@ Tag_CPU_unaligned_access
	.eabi_attribute 18, 4	@ Tag_ABI_PCS_wchar_t
	.file	"count.c"
@ GNU C (GNU Tools for ARM Embedded Processors (Arduino build)) version 4.8.3 20140228 (release) [ARM/embedded-4_8-branch revision 208322] (arm-none-eabi)
@	compiled by GNU C version 4.3.2, GMP version 4.3.2, MPFR version 2.4.2, MPC version 0.8.1
@ GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
@ options passed:
@ -I /Code/arduino/build/linux/work/hardware/tools/CMSIS/CMSIS/Include/
@ -I /Code/arduino/build/linux/work/hardware/tools/CMSIS/Device/ATMEL/
@ -I /Code/arduino/build/linux/work/hardware/arduino/samd/cores/arduino
@ -I /Code/arduino/build/linux/work/hardware/arduino/samd/variants/arduino_zero
@ -imultilib armv6-m
@ -iprefix /Code/arduino/build/linux/work/hardware/tools/gcc-arm-none-eabi-4.8.3-2014q1/bin/../lib/gcc/arm-none-eabi/4.8.3/
@ -isysroot /Code/arduino/build/linux/work/hardware/tools/gcc-arm-none-eabi-4.8.3-2014q1/bin/../arm-none-eabi
@ -MMD count.d -D__USES_INITFINI__ -D F_CPU=48000000L -D ARDUINO=10602
@ -D ARDUINO_SAMD_ZERO -D ARDUINO_ARCH_SAMD -D __SAMD21G18A__
@ -D USB_VID=0x2341 -D USB_PID=0x004d -D USBCON
@ -D USB_MANUFACTURER=Arduino LLC -D USB_PRODUCT=Arduino Zero count.c
@ -mcpu=cortex-m0plus -mthumb -Os -Wextra -ffunction-sections
@ -fdata-sections -fno-exceptions -fverbose-asm
@ --param max-inline-insns-single=500
@ options enabled:  -faggressive-loop-optimizations -fauto-inc-dec
@ -fbranch-count-reg -fcaller-saves -fcombine-stack-adjustments -fcommon
@ -fcompare-elim -fcprop-registers -fcrossjumping -fcse-follow-jumps
@ -fdata-sections -fdefer-pop -fdelete-null-pointer-checks -fdevirtualize
@ -fdwarf2-cfi-asm -fearly-inlining -feliminate-unused-debug-types
@ -fexpensive-optimizations -fforward-propagate -ffunction-cse
@ -ffunction-sections -fgcse -fgcse-lm -fgnu-runtime
@ -fguess-branch-probability -fhoist-adjacent-loads -fident -fif-conversion
@ -fif-conversion2 -findirect-inlining -finline -finline-atomics
@ -finline-functions -finline-functions-called-once
@ -finline-small-functions -fipa-cp -fipa-profile -fipa-pure-const
@ -fipa-reference -fipa-sra -fira-hoist-pressure -fira-share-save-slots
@ -fira-share-spill-slots -fivopts -fkeep-static-consts
@ -fleading-underscore -fmath-errno -fmerge-constants -fmerge-debug-strings
@ -fomit-frame-pointer -foptimize-register-move -foptimize-sibling-calls
@ -fpartial-inlining -fpeephole -fpeephole2 -fprefetch-loop-arrays
@ -freg-struct-return -fregmove -freorder-blocks -freorder-functions
@ -frerun-cse-after-loop -fsched-critical-path-heuristic
@ -fsched-dep-count-heuristic -fsched-group-heuristic -fsched-interblock
@ -fsched-last-insn-heuristic -fsched-pressure -fsched-rank-heuristic
@ -fsched-spec -fsched-spec-insn-heuristic -fsched-stalled-insns-dep
@ -fschedule-insns2 -fsection-anchors -fshow-column -fshrink-wrap
@ -fsigned-zeros -fsplit-ivs-in-unroller -fsplit-wide-types
@ -fstrict-aliasing -fstrict-overflow -fstrict-volatile-bitfields
@ -fsync-libcalls -fthread-jumps -ftoplevel-reorder -ftrapping-math
@ -ftree-bit-ccp -ftree-builtin-call-dce -ftree-ccp -ftree-ch
@ -ftree-coalesce-vars -ftree-copy-prop -ftree-copyrename -ftree-cselim
@ -ftree-dce -ftree-dominator-opts -ftree-dse -ftree-forwprop -ftree-fre
@ -ftree-loop-if-convert -ftree-loop-im -ftree-loop-ivcanon
@ -ftree-loop-optimize -ftree-parallelize-loops= -ftree-phiprop -ftree-pre
@ -ftree-pta -ftree-reassoc -ftree-scev-cprop -ftree-sink
@ -ftree-slp-vectorize -ftree-slsr -ftree-sra -ftree-switch-conversion
@ -ftree-tail-merge -ftree-ter -ftree-vect-loop-version -ftree-vrp
@ -funit-at-a-time -fverbose-asm -fzero-initialized-in-bss -mlittle-endian
@ -mpic-data-is-text-relative -msched-prolog -mthumb
@ -mvectorize-with-neon-quad

	.section	.text.countPulseASM,"ax",%progbits
	.align	1
	.global	countPulseASM
	.code	16
	.thumb_func
	.type	countPulseASM, %function
countPulseASM:
	push	{r4, r5, lr}	@			@ 112	*push_multi		[length = 2]
.L2:
	ldr	r4, [r0]	@ D.11539, *port_7(D)	@ 22	*thumb1_movsi_insn/7	[length = 2]
	and	r4, r1		@ D.11539, bit		@ 24	*thumb1_andsi3_insn	[length = 2]
	cmp	r4, r2		@ D.11539, stateMask	@ 25	cbranchsi4_insn/1	[length = 4]
	bne	.L5		@,
	sub	r3, r3, #1	@ maxloops,		@ 17	*thumb1_addsi3/2	[length = 2]
	cmp	r3, #0		@ maxloops,		@ 18	cbranchsi4_insn/1	[length = 4]
	bne	.L2		@,
	b	.L10		@			@ 127	*thumb_jump		[length = 2]
.L6:
	sub	r3, r3, #1	@ maxloops,		@ 30	*thumb1_addsi3/2	[length = 2]
	cmp	r3, #0		@ maxloops,		@ 31	cbranchsi4_insn/1	[length = 4]
	beq	.L10		@,
.L5:
	ldr	r4, [r0]	@ D.11539, *port_7(D)	@ 35	*thumb1_movsi_insn/7	[length = 2]
	and	r4, r1		@ D.11539, bit		@ 37	*thumb1_andsi3_insn	[length = 2]
	cmp	r4, r2		@ D.11539, stateMask	@ 38	cbranchsi4_insn/1	[length = 4]
	bne	.L6		@,
	mov	r4, #0		@ width,		@ 7	*thumb1_movsi_insn/2	[length = 2]
.L7:
	ldr	r5, [r0]	@ D.11539, *port_7(D)	@ 48	*thumb1_movsi_insn/7	[length = 2]
	and	r5, r1		@ D.11539, bit		@ 50	*thumb1_andsi3_insn	[length = 2]
	cmp	r5, r2		@ D.11539, stateMask	@ 51	cbranchsi4_insn/1	[length = 4]
	bne	.L13		@,
	add	r4, r4, #1	@ width,		@ 43	*thumb1_addsi3/1	[length = 2]
	cmp	r4, r3		@ width, maxloops	@ 44	cbranchsi4_insn/1	[length = 4]
	bne	.L7		@,
	mov	r0, #0		@ D.11539,		@ 11	*thumb1_movsi_insn/2	[length = 2]
	b	.L3		@			@ 130	*thumb_jump		[length = 2]
.L13:
	mov	r0, r4		@ D.11539, width	@ 9	*thumb1_movsi_insn/1	[length = 2]
	b	.L3		@			@ 132	*thumb_jump		[length = 2]
.L10:
	mov	r0, r3		@ D.11539, maxloops	@ 8	*thumb1_movsi_insn/1	[length = 2]
.L3:
	@ sp needed		@			@ 115	force_register_use	[length = 0]
	pop	{r4, r5, pc}
	.size	countPulseASM, .-countPulseASM
	.ident	"GCC: (GNU Tools for ARM Embedded Processors (Arduino build)) 4.8.3 20140228 (release) [ARM/embedded-4_8-branch revision 208322]"
