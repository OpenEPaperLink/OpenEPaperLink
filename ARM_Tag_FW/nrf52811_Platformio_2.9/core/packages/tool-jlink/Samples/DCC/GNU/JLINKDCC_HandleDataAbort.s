/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*            (c) 2006 - 2019 SEGGER Microcontroller GmbH             *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
* All rights reserved.                                               *
*                                                                    *
* Redistribution and use in source and binary forms, with or         *
* without modification, are permitted provided that the following    *
* condition is met:                                                  *
*                                                                    *
* o Redistributions of source code must retain the above copyright   *
*   notice, this condition and the following disclaimer.             *
*                                                                    *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND             *
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,        *
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF           *
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
* DISCLAIMED. IN NO EVENT SHALL SEGGER Microcontroller BE LIABLE FOR *
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT  *
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;    *
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF      *
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE  *
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
* DAMAGE.                                                            *
*                                                                    *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File    : JLINKDCC_HandleDataAbort.s
Purpose : Data abort handler for ARM J-Link type communication via DCC
Notes   : (1) How to use
              In order to use the DCC abort handler, 3 things need to be done:
              * Place a branch to DCC_Abort at address 0x10 ("vector" used for data aborts)
              * Initialize the Abort-mode stack pointer to an area of at least 8 bytes of stack
                memory required by the handler
              * Add the DCC abort handler assembly file to the application
          (2) Compatibility
              The J-Link ARM DCC handler is compatible to the DCC communication
              protocol used by IAR in the embedded workbench for ARM and allows
              using the live data window in C-Spy
              
*/


/*********************************************************************
*
*       GNU ARM
*
*/
        .text
        .extern  JLINKDCC_IsInHandler
        .extern  JLINKDCC_AbortOccurred
        .global  JLINKDCC_HandleDataAbort
        .arm
        .section .text, "ax" 

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/

/*********************************************************************
*
*       JLINKDCC_HandleDataAbort
*
*  Function description
*    Data abort handler for J-Link ARM type DCC communication.
*
*  Notes
*    (1) Abort mode
*        This handler is supposed to be executed in abort mode by simply placing a
*        jump to the handler at address 0x10.
*        Just like an interrupt service routine, it needs to restore all registers
*        for other modes and may only permanently modify registers of abort mode (SPSR_ABT, R14_ABT)
*/
JLINKDCC_HandleDataAbort:
@
@ Check if exception stemmed from JLINKARM DCC communication. Otherwise, user abort handler is called
@
        STMDB     SP!, {R0,R1}            @ Save registers on stack
        LDR       R1, =JLINKDCC_IsInHandler
        LDRB      R0, [R1, #0]
        TST       R0, R0                  @ Check if abort occurred during execution of DCC handler
        LDMEQIA   SP!, {R0,R1}            @ Restore registers
        BEQ       UserAbortHandler
@
@ Set JLINKDCC_AbortOccurred so PC (via J-Link) has a chance to find out that memory operation was aborted
@
        LDR       R1, =JLINKDCC_AbortOccurred
        STRB      R0, [R1, #0]            @ Set abort occurred state
@
@ Continue code execution right after the instruction which caused the abort.
@
        MRS       R0, SPSR
        TST       R0, #0x20               @ Check thumb bit
        LDMIA     SP!, {R0,R1}            @ Restore registers
        SUBEQS    PC, R14, #4             @ Return after the aborted instruction (ARM mode)
        SUBS      PC, R14, #6             @ Return after the aborted instruction (Thumb mode)

/*********************************************************************
*
*       UserAbortHandler
*
*  Function description
*    User defined abort handler.
*    This function is called if a data abort occurs for other reasons than JLINK DCC comunication.
*    Feel free to modify and jump to your own handler as required.
*/
UserAbortHandler:
        B         UserAbortHandler        @ Modify this line to jump to your own handler if desired

        .end

/*************************** end of file ****************************/
