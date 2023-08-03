/*-------------------------------------------------------------------------
   ds400rom.c

   Copyright (C) 2003, Dallas Semiconductor Corporation

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this library; see the file COPYING. If not, write to the
   Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.

   As a special exception, if you link this library with other files,
   some of which are compiled with SDCC, to produce an executable,
   this library does not by itself cause the resulting executable to
   be covered by the GNU General Public License. This exception does
   not however invalidate any other reasons why the executable file
   might be covered by the GNU General Public License.
-------------------------------------------------------------------------*/
// Interface to the DS80C400 ROM functions. Largely based on code released
// by Dallas, hence the following copyright.

// ---------------------------------------------------------------------------
//  Copyright (C) 2003 Dallas Semiconductor Corporation, All Rights Reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included
//  in all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//  MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//  IN NO EVENT SHALL DALLAS SEMICONDUCTOR BE LIABLE FOR ANY CLAIM, DAMAGES
//  OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
//  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
//  OTHER DEALINGS IN THE SOFTWARE.
//
//  Except as contained in this notice, the name of Dallas Semiconductor
//  shall not be used except as stated in the Dallas Semiconductor
//  Branding Policy.
// ---------------------------------------------------------------------------

#include <stdio.h>
#include <ds400rom.h>

// ROM globals, taken from startup400.a51
__data unsigned char __at (0x68) DSS_wos_crit_count;
__data unsigned int  __at (0x6b) DSS_timerReload;
__data unsigned char __at (0x6d) DSS_curr_pc[3];
__data unsigned char __at (0x72) DSS_sched[3];
__data unsigned char __at (0x74) DSS_ms_count[5];
__data unsigned char __at (0x7b) DSS_hb_chandle[5];

// Register bank 3 equates.
#define R0_B3     0x18
#define R1_B3     0x19
#define R2_B3     0x1A
#define R3_B3     0x1B
#define R4_B3     0x1C
#define R5_B3     0x1D
#define R6_B3     0x1E
#define R7_B3     0x1F


// The top of the redirect function table in RAM.
#define CALL_TABLE_TOP  256

// The bank the ROM is stored in.  Should be 0FFh for production
// 400's.  Change this value when running with a debug ROM.

#define ROM_BANK        0xFF

// The address of the ROM export table is stored
// at (ROM_BANK << 16) | ROM_EXPORTTABLE_OFFS

#define ROM_EXPORTTABLE_OFFS    2

//
// Each entry in the ROM export table is 3 bytes.
//
#define ROMXT_ENTRYSIZE      3

//
// The number of functions in the ROM export table is stored
// first in the export table.
//
#define ROMXT_NUMFUNCTIONS     (0 * ROMXT_ENTRYSIZE)

//
// ROM EXPORT TABLE FUNCTIONS (denoted with ROMXT)
//

// UTILITY functions
#define ROMXT_CRC16                     (1 * ROMXT_ENTRYSIZE) //
#define ROMXT_MEM_CLEAR_16              (2 * ROMXT_ENTRYSIZE) //
#define ROMXT_MEM_COPY_16               (3 * ROMXT_ENTRYSIZE) //
#define ROMXT_MEM_COMPARE               (4 * ROMXT_ENTRYSIZE) //
#define ROMXT_ADD_DPTR1_16              (5 * ROMXT_ENTRYSIZE) // not implemented for C compiler
#define ROMXT_ADD_DPTR2_16              (6 * ROMXT_ENTRYSIZE) // not implemented for C compiler
#define ROMXT_SUB_DPTR1_16              (7 * ROMXT_ENTRYSIZE) // not implemented for C compiler
#define ROMXT_SUB_DPTR2_16              (8 * ROMXT_ENTRYSIZE) // not implemented for C compiler
#define ROMXT_GETPSEUDORANDOM           (9 * ROMXT_ENTRYSIZE) //

// MEMORY MGR
#define ROMXT_KERNELMALLOC              (10 * ROMXT_ENTRYSIZE) // not exposed
#define ROMXT_KERNELFREE                (11 * ROMXT_ENTRYSIZE) // not exposed
#define ROMXT_MM_MALLOC                 (12 * ROMXT_ENTRYSIZE) // exposed as redirected function
#define ROMXT_MM_MALLOC_DIRTY           (13 * ROMXT_ENTRYSIZE) // exposed as redirected function
#define ROMXT_MM_FREE                   (14 * ROMXT_ENTRYSIZE) // exposed as redirected function
#define ROMXT_MM_DEREF                  (15 * ROMXT_ENTRYSIZE) // exposed as redirected function
#define ROMXT_GETFREERAM                (16 * ROMXT_ENTRYSIZE) // exposed as redirected function

// SOCKET functions
#define ROMXT_ROM_SOCKET                (17 * ROMXT_ENTRYSIZE) //
#define ROMXT_ROM_CLOSESOCKET           (18 * ROMXT_ENTRYSIZE) //
#define ROMXT_ROM_SENDTO                (19 * ROMXT_ENTRYSIZE) //
#define ROMXT_ROM_RECVFROM              (20 * ROMXT_ENTRYSIZE) //
#define ROMXT_ROM_CONNECT               (21 * ROMXT_ENTRYSIZE) //
#define ROMXT_ROM_BIND                  (22 * ROMXT_ENTRYSIZE) //
#define ROMXT_ROM_LISTEN                (23 * ROMXT_ENTRYSIZE) //
#define ROMXT_ROM_ACCEPT                (24 * ROMXT_ENTRYSIZE) //
#define ROMXT_ROM_RECV                  (25 * ROMXT_ENTRYSIZE) //
#define ROMXT_ROM_SEND                  (26 * ROMXT_ENTRYSIZE) //
#define ROMXT_ROM_GETSOCKOPT            (27 * ROMXT_ENTRYSIZE) //
#define ROMXT_ROM_SETSOCKOPT            (28 * ROMXT_ENTRYSIZE) //
#define ROMXT_ROM_GETSOCKNAME           (29 * ROMXT_ENTRYSIZE) //
#define ROMXT_ROM_GETPEERNAME           (30 * ROMXT_ENTRYSIZE) //
#define ROMXT_ROM_CLEANUP               (31 * ROMXT_ENTRYSIZE) //
#define ROMXT_ROM_AVAIL                 (32 * ROMXT_ENTRYSIZE) //
#define ROMXT_ROM_JOIN                  (33 * ROMXT_ENTRYSIZE) //
#define ROMXT_ROM_LEAVE                 (34 * ROMXT_ENTRYSIZE) //
#define ROMXT_ROM_PING                  (35 * ROMXT_ENTRYSIZE) //
#define ROMXT_ROM_GETNETWORKPARAMS      (36 * ROMXT_ENTRYSIZE) //
#define ROMXT_ROM_SETNETWORKPARAMS      (37 * ROMXT_ENTRYSIZE) //
#define ROMXT_ROM_GETIPV6PARAMS         (38 * ROMXT_ENTRYSIZE) //
#define ROMXT_ROM_GETETHERNETSTATUS     (39 * ROMXT_ENTRYSIZE) //
#define ROMXT_ROM_GETTFTPSERVER         (40 * ROMXT_ENTRYSIZE) //
#define ROMXT_ROM_SETTFTPSERVER         (41 * ROMXT_ENTRYSIZE) //
#define ROMXT_ETH_PROCESSINTERRUPT      (42 * ROMXT_ENTRYSIZE) // not implemented for C compiler
#define ROMXT_ARP_GENERATEREQUEST       (43 * ROMXT_ENTRYSIZE) // not implemented for C compiler
#define ROMXT_NET_ETH0_MAC_ID           (44 * ROMXT_ENTRYSIZE) //

// DHCP functions
#define ROMXT_DHCP_INIT                 (45 * ROMXT_ENTRYSIZE) //
#define ROMXT_DHCP_SETUP                (46 * ROMXT_ENTRYSIZE) // not implemented for C compiler
#define ROMXT_DHCP_STARTUP              (47 * ROMXT_ENTRYSIZE) // not implemented for C compiler
#define ROMXT_DHCP_RUN                  (48 * ROMXT_ENTRYSIZE) // not implemented for C compiler
#define ROMXT_DHCP_STATUS               (49 * ROMXT_ENTRYSIZE) //
#define ROMXT_DHCP_STOP                 (50 * ROMXT_ENTRYSIZE) //
#define ROMXT_DHCPNOTIFY                (51 * ROMXT_ENTRYSIZE) // empty redirect stub, not implemented

// TFTP functions
#define ROMXT_TFTP_INIT                 (52 * ROMXT_ENTRYSIZE) //
#define ROMXT_TFTP_FIRST                (53 * ROMXT_ENTRYSIZE) //
#define ROMXT_TFTP_NEXT                 (54 * ROMXT_ENTRYSIZE) //
#define ROMXT_TFTP_MSG                  (55 * ROMXT_ENTRYSIZE) //

// SCHEDULER functions
#define ROMXT_TASK_GENESIS              (56 * ROMXT_ENTRYSIZE) //
#define ROMXT_TASK_GETCURRENT           (57 * ROMXT_ENTRYSIZE) //
#define ROMXT_TASK_GETPRIORITY          (58 * ROMXT_ENTRYSIZE) //
#define ROMXT_TASK_SETPRIORITY          (59 * ROMXT_ENTRYSIZE) //
#define ROMXT_TASK_FORK                 (60 * ROMXT_ENTRYSIZE) //
#define ROMXT_TASK_KILL                 (61 * ROMXT_ENTRYSIZE) //
#define ROMXT_TASK_SUSPEND              (62 * ROMXT_ENTRYSIZE) //
#define ROMXT_TASK_SLEEP                (63 * ROMXT_ENTRYSIZE) //
#define ROMXT_TASK_SIGNAL               (64 * ROMXT_ENTRYSIZE) //
#define ROMXT_ROM_TASK_SWITCH_IN        (65 * ROMXT_ENTRYSIZE) // empty redirect stub, not implemented
#define ROMXT_ROM_TASK_SWITCH_OUT       (66 * ROMXT_ENTRYSIZE) // empty redirect stub, not implemented
#define ROMXT_ENTERCRITSECTION          (67 * ROMXT_ENTRYSIZE) //
#define ROMXT_LEAVECRITSECTION          (68 * ROMXT_ENTRYSIZE) //

// INIT functions
#define ROMXT_ROM_INIT                  (69 * ROMXT_ENTRYSIZE) //
#define ROMXT_ROM_COPYIVT               (70 * ROMXT_ENTRYSIZE) //
#define ROMXT_ROM_REDIRECT_INIT         (71 * ROMXT_ENTRYSIZE) //
#define ROMXT_MM_INIT                   (72 * ROMXT_ENTRYSIZE) //
#define ROMXT_KM_INIT                   (73 * ROMXT_ENTRYSIZE) //
#define ROMXT_OW_INIT                   (74 * ROMXT_ENTRYSIZE) //
#define ROMXT_NETWORK_INIT              (75 * ROMXT_ENTRYSIZE) //
#define ROMXT_ETH_INIT                  (76 * ROMXT_ENTRYSIZE) //
#define ROMXT_INIT_SOCKETS              (77 * ROMXT_ENTRYSIZE) //
#define ROMXT_TICK_INIT                 (78 * ROMXT_ENTRYSIZE) //

// Timer Interrupt vectors
#define ROMXT_WOS_TICK                  (79 * ROMXT_ENTRYSIZE) // not implemented for C compiler
#define ROMXT_BLOB                      (80 * ROMXT_ENTRYSIZE) // not implemented for C compiler

// Maintenance functions
#define ROMXT_WOS_IOPOLL                (81 * ROMXT_ENTRYSIZE) // not implemented for C compiler
#define ROMXT_IP_PROCESSRECEIVEQUEUES   (82 * ROMXT_ENTRYSIZE) // not implemented for C compiler
#define ROMXT_IP_PROCESSOUTPUT          (83 * ROMXT_ENTRYSIZE) // not implemented for C compiler
#define ROMXT_TCP_RETRYTOP              (84 * ROMXT_ENTRYSIZE) // not implemented for C compiler
#define ROMXT_ETH_PROCESSOUTPUT         (85 * ROMXT_ENTRYSIZE) // not implemented for C compiler
#define ROMXT_IGMP_GROUPMAINTAINENCE    (86 * ROMXT_ENTRYSIZE) // not implemented for C compiler
#define ROMXT_IP6_PROCESSRECEIVEQUEUES  (87 * ROMXT_ENTRYSIZE) // not implemented for C compiler
#define ROMXT_IP6_PROCESSOUTPUT         (88 * ROMXT_ENTRYSIZE) // not implemented for C compiler
#define ROMXT_PARAMBUFFER               (89 * ROMXT_ENTRYSIZE) // not implemented for C compiler
#define ROMXT_RAM_TOP                   (90 * ROMXT_ENTRYSIZE) // not implemented for C compiler
#define ROMXT_BOOT_MEMBEGIN             (91 * ROMXT_ENTRYSIZE) // not implemented for C compiler
#define ROMXT_BOOT_MEMEND               (92 * ROMXT_ENTRYSIZE) // not implemented for C compiler

// 1-Wire
#define ROMXT_OWM_FIRST                 (93 * ROMXT_ENTRYSIZE) //
#define ROMXT_OWM_NEXT                  (94 * ROMXT_ENTRYSIZE) //
#define ROMXT_OWM_RESET                 (95 * ROMXT_ENTRYSIZE) //
#define ROMXT_OWM_BYTE                  (96 * ROMXT_ENTRYSIZE) //
#define ROMXT_OWM_SEARCH                (97 * ROMXT_ENTRYSIZE) // not implemented for C compiler
#define ROMXT_OW_ROMID                  (98 * ROMXT_ENTRYSIZE) //

// Misc, extras, late additions
#define ROMXT_AUTOBAUD                  (99 * ROMXT_ENTRYSIZE)
#define ROMXT_TFTP_CLOSE                (100 * ROMXT_ENTRYSIZE)


#define ROMRT_ENTRYSIZE  3

//
// ROM REDIRECT TABLE FUNCTIONS (denoted with ROMRT)
//
#define ROMRT_KERNELMALLOC                  ( 1 * ROMRT_ENTRYSIZE)
#define ROMRT_KERNELFREE                    ( 2 * ROMRT_ENTRYSIZE)
#define ROMRT_MALLOC                        ( 3 * ROMRT_ENTRYSIZE)
#define ROMRT_FREE                          ( 4 * ROMRT_ENTRYSIZE)
#define ROMRT_MALLOCDIRTY                   ( 5 * ROMRT_ENTRYSIZE)
#define ROMRT_DEREF                         ( 6 * ROMRT_ENTRYSIZE)
#define ROMRT_GETFREERAM                    ( 7 * ROMRT_ENTRYSIZE)
#define ROMRT_GETTIMEMILLIS                 ( 8 * ROMRT_ENTRYSIZE)
#define ROMRT_GETTHREADID                   ( 9 * ROMRT_ENTRYSIZE)
#define ROMRT_THREADRESUME                  (10 * ROMRT_ENTRYSIZE)
#define ROMRT_THREADIOSLEEP                 (11 * ROMRT_ENTRYSIZE)
#define ROMRT_THREADIOSLEEPNC               (12 * ROMRT_ENTRYSIZE)
#define ROMRT_THREADSAVE                    (13 * ROMRT_ENTRYSIZE)
#define ROMRT_THREADRESTORE                 (14 * ROMRT_ENTRYSIZE)
#define ROMRT_SLEEP                         (15 * ROMRT_ENTRYSIZE)
#define ROMRT_GETTASKID                     (16 * ROMRT_ENTRYSIZE)
#define ROMRT_INFOSENDCHAR                  (17 * ROMRT_ENTRYSIZE)
#define ROMRT_IP_COMPUTECHECKSUM_SOFTWARE   (18 * ROMRT_ENTRYSIZE)
#define ROMRT_0                             (19 * ROMRT_ENTRYSIZE) // undefined
#define ROMRT_DHCPNOTIFY                    (20 * ROMRT_ENTRYSIZE)
#define ROMRT_ROM_TASK_CREATE               (21 * ROMRT_ENTRYSIZE)
#define ROMRT_ROM_TASK_DUPLICATE            (22 * ROMRT_ENTRYSIZE)
#define ROMRT_ROM_TASK_DESTROY              (23 * ROMRT_ENTRYSIZE)
#define ROMRT_ROM_TASK_SWITCH_IN            (24 * ROMRT_ENTRYSIZE)
#define ROMRT_ROM_TASK_SWITCH_OUT           (25 * ROMRT_ENTRYSIZE)
#define ROMRT_OWIP_READCONFIG               (26 * ROMRT_ENTRYSIZE)
#define ROMRT_SETMACID                      (27 * ROMRT_ENTRYSIZE)
#define ROMRT_UNDEREF                       (28 * ROMRT_ENTRYSIZE)


#define GETC            \
    clr  a              \
    movc a, @a+dptr


// expects function number in R6_B3 (low byte) & R7_B3 (high byte)
void _romcall(void) __naked
{
__asm
      push  dpx                               ; dptr0 preserved here
      push  dph
      push  dpl

      ; point to the address of the table
      mov   dptr, #(ROM_BANK << 16 | ROM_EXPORTTABLE_OFFS)

      push  acc                               ; acc preserved here
      push  b                                 ; b preserved here
      inc   dptr
      GETC                                    ; get the address of the table
      push  acc
      inc   dptr
      GETC
      add   a, R6_B3                          ; add function offset to the table
      mov   dpl, a
      pop   acc
      addc  a, R7_B3
      mov   dph, a

      ;
      ; dpx is the same, all in the same bank
      ;
      inc   dptr                              ; get the target address of the function we want
      GETC
      mov   b, a
      inc   dptr
      GETC
      mov   R3_B3, a
      mov   R4_B3, b
      mov   R5_B3, dpx                        ; high byte does not change
      pop   b                                 ; b restored here
      pop   acc                               ; acc restored here
      pop   dpl                               ; dptr0 preserved here
      pop   dph
      pop   dpx
      push  R3_B3                             ; push the target address
      push  R4_B3
      push  R5_B3
      ret                                     ; this is not a ret, it is a call!

      ; the called function ends with a ret which will return to our original caller.
__endasm ;
}

// expects function number in R6_B3 (low byte) & R7_B3 (high byte)
void _romredirect(void) __naked
{
__asm
      push  dpx
      push  dph
      push  dpl
      push  acc
      ; dptr = CALL_TABLE_TOP + function offset.
      mov   a, #(CALL_TABLE_TOP & 0xff)
      add   a, R6_B3                          ; add function offset to the table
      mov   dpl, a
      mov   a, #((CALL_TABLE_TOP >> 8) & 0xff)
      addc  a, R7_B3
      mov   dph, a
      mov   dpx, #((CALL_TABLE_TOP >> 16) & 0xff)
      movx  a, @dptr                      ; read high byte
      mov   R5_B3, a
      inc   dptr
      movx  a, @dptr                      ; read mid byte
      mov   R4_B3, a
      inc   dptr
      movx  a, @dptr                      ; read low byte
      mov   R3_B3, a
      pop   acc                 ; restore acc and dptr
      pop   dpl
      pop   dph
      pop   dpx
      push  R3_B3               ; push low byte of target address
      push  R4_B3
      push  R5_B3               ; push high byte of target address
      ret                       ; this is not a ret, it is a call!

      ; the called function ends with a ret which will return to our original caller.
__endasm;
}


// This macro is invalid for the standard C preprocessor, since it
// includes a hash character in the expansion, hence the SDCC specific
// pragma.
#pragma sdcc_hash +
#define ROMCALL(x) \
        mov     R6_B3, #(x & 0xff)              \
        mov     R7_B3, #((x >> 8) & 0xff)       \
        lcall   __romcall

#define ROMREDIRECT(x) \
        mov     R6_B3, #(x & 0xff)              \
        mov     R7_B3, #((x >> 8) & 0xff)       \
        lcall   __romredirect


// init_rom: the ds400 ROM_INIT ROM function.
unsigned char init_rom(void __xdata *loMem,
                       void __xdata *hiMem) __naked
{
    // shut compiler up about unused parameters.
    loMem;
    hiMem;

__asm
        ; load params.
        ; loMem is already in DPTR.
        mov     r2, dpx
        mov     r1, dph
        mov     r0, dpl
        ; hiMem is in _init_rom_PARM_2
        mov     dptr, #_init_rom_PARM_2
        mov     r5, dpx
        mov     r4, dph
        mov     r3, dpl

        ROMCALL(ROMXT_ROM_INIT)

        ; result is in acc, move to dpl for C convention.
        mov     dpl, a
        ret
__endasm        ;
}

// DSS_gettimemillis: note that the ROM actually returns 5 bytes of time,
// we're discarding the high byte here.
unsigned long task_gettimemillis_long(void) __naked
{
__asm
   ; no parameters to load.
   ROMREDIRECT(ROMRT_GETTIMEMILLIS)
   ; results in r4 - r0, return in DPTR/B
   mov dpl, r0
   mov dph, r1
   mov dpx, r2
   mov b, r3
   ret
__endasm;
}

unsigned char task_getthreadID(void) __naked
{
__asm
   ; no parameters to load.
   ROMREDIRECT(ROMRT_GETTHREADID)
   ; results in acc, return in dpl
   mov dpl, a
   ret
__endasm;
}

unsigned int task_gettickreload(void)
{
  return DSS_timerReload;
}

void task_settickreload(unsigned int rl)
{
  DSS_timerReload = rl;
}
