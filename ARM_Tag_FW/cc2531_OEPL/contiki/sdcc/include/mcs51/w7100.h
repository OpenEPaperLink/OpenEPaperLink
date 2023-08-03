/*-------------------------------------------------------------------------
   w7100.h - Register Declarations for WIZnet's W7100
   (Based on Internet Embedded MCU W7100 Datasheet Version 0.9 Beta)

   Copyright (C) 2009, Jesus Calvino-Fraga / jesusc at ece.ubc.ca

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

#ifndef REG_W7100_H
#define REG_W7100_H

#include <compiler.h>

SFR(ACC, 0xE0);   // Accumulator
    SBIT(ACC_0, 0xE0, 0);
    SBIT(ACC_1, 0xE0, 1);
    SBIT(ACC_2, 0xE0, 2);
    SBIT(ACC_3, 0xE0, 3);
    SBIT(ACC_4, 0xE0, 4);
    SBIT(ACC_5, 0xE0, 5);
    SBIT(ACC_6, 0xE0, 6);
    SBIT(ACC_7, 0xE0, 7);

SFR(B, 0xF0);   // Register B
    SBIT(B_0, 0xF0, 0);
    SBIT(B_1, 0xF0, 1);
    SBIT(B_2, 0xF0, 2);
    SBIT(B_3, 0xF0, 3);
    SBIT(B_4, 0xF0, 4);
    SBIT(B_5, 0xF0, 5);
    SBIT(B_6, 0xF0, 6);
    SBIT(B_7, 0xF0, 7);

SFR(PCON, 0x87); // Power Configuration Register
    #define SMOD0 0x80 // Baud rate select bit
    #define SMOD1 0x40 // Not documented
    #define PWE   0x10 // Program Code Memory Write Enable Bit

SFR(WTST, 0x92); // Program Code Memory Wait States Register
    #define WTST_2 0x04 // Wait States Register bit 2
    #define WTST_1 0x02 // Wait States Register bit 1
    #define WTST_0 0x01 // Wait States Register bit 0

SFR(DPX0, 0x93); // Data Pointer Extended Register low
SFR(DPX1, 0x95); // Data Pointer Extended Register high
SFR(MXAX, 0xEA); // Data Pointer Extended Register

SFR16E(DPTR0, 0x8382); // Data Pointer Register 0
SFR(DPH,  0x83); // Data Pointer Register 0 high
SFR(DPL,  0x82); // Data Pointer Register 0 low
SFR(DPH0, 0x83); // Data Pointer Register 0 high
SFR(DPL0, 0x82); // Data Pointer Register 0 low
SFR16E(DPTR1, 0x8584); // Data Pointer Register 1
SFR(DPH1, 0x85); // Data Pointer Register 1 high
SFR(DPL1, 0x84); // Data Pointer Register 1 low
SFR(DPS, 0x86);  //Data Pointer Select Register
    #define ID1 0x80 // DPTR0, DPTR1 Operations bit 1
    #define ID0 0x40 // DPTR0, DPTR1 Operations bit 0
    #define TSL 0x20 // Toggle select enable
    #define SEL 0x01 // DPTR register select

SFR(CKCON, 0x8E); // Clock Control Register
    #define WD1 0x80 // Watchdog Interval bit 1
    #define WD0 0x40 // Watchdog Interval bit 0
    #define MD2 0x04 // Stretch bit 2
    #define MD1 0x02 // Stretch bit 1
    #define MD0 0x01 // Stretch bit 0

SFR(SP, 0x81);        // Stack Pointer
SFR(ISPID, 0xF1);     // ID Register for ISP.
SFR(ISPADDR16, 0xF2); // 16bit Address Register for ISP
SFR(ISPDATA, 0xF4);   // Data Register for ISP.
SFR(CKCBK, 0xF5);     // CKCON Backup Register.
SFR(DPX0BK, 0xF6);    // DPX0 Backup Register.
SFR(DPX1BK, 0xF7);    // DPX1 Backup Register.
SFR(DPSBK, 0xF9);     // DPX Backup Register.

// WARNING: the following six definitions are not well documented
SFR16(RAMBA16, 0xFAFB);  // RAM Base Address Register.
SFR(RAMBA16H, 0xFA);
SFR(RAMBA16L, 0xFB);
SFR16(RAMEA16, 0xFCFD);  // RAM End Address Register.
SFR(RAMEA16H, 0xFC);
SFR(RAMEA16L, 0xFD);

SFR(WCONF, 0xFF); // W7100 Configuration Register.
    #define RB    0x80 // 0 : No Reboot / 1 : Reboot after the ISP done.
    #define ISPEN 0x40 // 0 : Enable ISP in Boot built in W7100 / 1 : Disable.
    #define F64EN 0x04 // Always '0'. Read only.
    #define FB    0x02 // FLASH Busy Flag for ISP. Read only.
    #define BE    0x01 // Boot Enable (1 : Boot Running / 0 : Apps Running). Read only.

SFR(TCON, 0x88); // Timer0, 1 configuration register.
    SBIT(IT0, 0x88, 0); // INT0 level (at 0)/edge (at 1) sensitivity.
    SBIT(IE0, 0x88, 1); // INT0 interrupt flag. 
    SBIT(IT1, 0x88, 2); // INT1 level (at 0)/edge (at 1) sensitivity.
    SBIT(IE1, 0x88, 3); // INT1 interrupt flag.
    SBIT(TR0, 0x88, 4); // Timer 0 run control bit 1: Enabled, 0: Disabled.
    SBIT(TF0, 0x88, 5); // Timer 0 interrupt (overflow) flag.
    SBIT(TR1, 0x88, 6); // Timer 1 run control bit 1: Enabled, 0: Disabled.
    SBIT(TF1, 0x88, 7); // Timer 1 interrupt (overflow) flag.

SFR(TMOD, 0x89); // Timer 0 and timer 1 control mode register.
    #define T0_M0    0x01
    #define T0_M1    0x02
    #define T0_CT    0x04
    #define T0_GATE  0x08
    #define T1_M0    0x10
    #define T1_M1    0x20
    #define T1_CT    0x40
    #define T1_GATE  0x80
SFR(TH0,  0x8C); // Counter register of timer 0 high.
SFR(TL0,  0x8A); // Counter register of timer 0 low.
SFR(TH1,  0x8D); // Counter register of timer 1 high. 
SFR(TL1,  0x8B); // Counter register of timer 1 low. 

SFR(SCON, 0x98); // UART Configuration Register.
    SBIT(SM0, 0x98, 7); // UART mode bit 0.
    SBIT(SM1, 0x98, 6); // UART mode bit 1.
    SBIT(SM2, 0x98, 5); // Enable a multiprocessor communication feature.
    SBIT(REN, 0x98, 4); // Enable/Disable serial receive.
    SBIT(TB8, 0x98, 3); // The 9th transmitted data bit in Modes 2 and 3.
    SBIT(RB8, 0x98, 2); // The 9th received data bit in Modes 2 and 3.
    SBIT(TI,  0x98, 1); // UART transmitter interrupt flag.
    SBIT(RI,  0x98, 0); // UART receiver interrupt flag.

SFR(SBUF, 0x99); // UART Buffer Register. 

SFR(IE, 0xA8); // Interrupt Enable Register.
    SBIT(EX0, 0xA8, 0); // Enable INT0 interrupt.
    SBIT(ET0, 0xA8, 1); // Enable Timer0 interrupt.
    SBIT(EX1, 0xA8, 2); // Enable INT1 interrupt.
    SBIT(ET1, 0xA8, 3); // Enable Timer1 interrupt.
    SBIT(ES,  0xA8, 4); // Enable UART interrupt.
    SBIT(ET2, 0xA8, 5); // Enable Timer2 interrupt.
    SBIT(EA,  0xA8, 7); // Enable global interrupt.

SFR(IP, 0xB8); // Interrupt Priority Register.
    SBIT(PX0, 0xA8, 0); // INT0 priority level control.
    SBIT(PT0, 0xA8, 1); // Timer0 priority level control.
    SBIT(PX1, 0xA8, 2); // INT1 priority level control.
    SBIT(PT1, 0xA8, 3); // Timer1 priority level control.
    SBIT(PS,  0xA8, 4); // UART priority level control.
    SBIT(PT2, 0xA8, 5); // Timer2 priority level control.

SFR(TA, 0xC7); // Timed Access Register. 

SFR(T2CON, 0xC8); // Timer 2 Configuration Register.
    SBIT(CPRL2, 0xC8, 0); // Capture/Reload select.
    SBIT(CT2,   0xC8, 1); // Timer/Counter select.
    SBIT(TR2,   0xC8, 2); // Start/Stop Timer2.
    SBIT(EXEN2, 0xC8, 3); // Enable T2EX pin functionality.
    SBIT(TCLK,  0xC8, 4); // Transmit clock enable.
    SBIT(RCLK,  0xC8, 5); // Receive clock enable.
    SBIT(EXF2,  0xC8, 6); // Indicates a Falling edge in the T2EX pin when EXEN2=1.
    SBIT(TF2,   0xC8, 7); // Timer2 overflow flag.

SFR(RLDH, 0xCB); // Capture Registers of Timer 2 high.
SFR(RLDL, 0xCA); // Capture Registers of Timer 2 low. 

SFR(TH2, 0xCD); // Counter Register of Timer 2 high.
SFR(TL2, 0xCC); // Counter Register of Timer 2 low.

SFR(PSW, 0xD0); // Program Status Word Register.
    SBIT(P,   0xD0, 0); // Parity flag.
    SBIT(F1,  0xD0, 1); // General purpose flag 1.
    SBIT(OV,  0xD0, 2); // Overflow flag.
    SBIT(RS0, 0xD0, 3); // Register bank select bit 0.
    SBIT(RS1, 0xD0, 4); // Register bank select bit 1.
    SBIT(F0,  0xD0, 5); // General purpose flag 0.
    SBIT(AC,  0xD0, 6); // Auxiliary carry.
    SBIT(CY,  0xD0, 7); // Carry flag.

SFR(EIE, 0xE8); //Extended Interrupt Enable Register.
    SBIT(EINT2, 0xE8, 0); // Enable INT2 Interrupt.
    SBIT(EINT3, 0xE8, 1); // Enable INT3 Interrupt.
    SBIT(EINT4, 0xE8, 2); // Must be set to '0'.
    SBIT(EINT5, 0xE8, 3); // Enable TCPIPCore Interrupt.
    SBIT(EWDI,  0xE8, 4); // Enable WATCHDOG Interrupt.

SFR(EIP, 0xF8); // Extended Interrupt Priority Register.
    SBIT(PINT2, 0xF8, 0); // INT2 priority level control.
    SBIT(PINT3, 0xF8, 1); // INT3 priority level control.
    SBIT(PINT4, 0xF8, 2); // Must be set to '0'.
    SBIT(PINT5, 0xF8, 3); // TCPIPCore Interrupt priority level control.
    SBIT(PWDI,  0xF8, 4); // WATCHDOG priority level control.

SFR(EIF, 0x91); // Extended Interrupt Flag Register.
    #define INT2F 0x01 // INT2 interrupt flag. Must be cleared by software.
    #define INT3F 0x02 // INT3 interrupt flag. Must be cleared by software.
    #define INT4F 0x04 // Must be set to '0'.
    #define INT5F 0x08 // TCPIPCore Interrupt flag. Must be cleared by software.

SFR(WDCON, 0xD8); // Watchdog Control Register
    SBIT(RWT,  0xD8, 0); // Reset the Watchdog Timer.
    SBIT(EWT,  0xD8, 1); // Enable the Watchdog Timer reset.
    SBIT(WTRF, 0xD8, 2); // Watchdog Timer reset Flag.
    SBIT(WDIF, 0xD8, 3); // Watchdog Interrupt Flag.

SFR(P0, 0x80); // Port 0 Register.
    SBIT(P0_0, 0x80, 0);
    SBIT(P0_1, 0x80, 1);
    SBIT(P0_2, 0x80, 2);
    SBIT(P0_3, 0x80, 3);
    SBIT(P0_4, 0x80, 4);
    SBIT(P0_5, 0x80, 5);
    SBIT(P0_6, 0x80, 6);
    SBIT(P0_7, 0x80, 7);

SFR(P1, 0x90); // Port 1 Register.
    SBIT(P1_0, 0x90, 0);
    SBIT(P1_1, 0x90, 1);
    SBIT(P1_2, 0x90, 2);
    SBIT(P1_3, 0x90, 3);
    SBIT(P1_4, 0x90, 4);
    SBIT(P1_5, 0x90, 5);
    SBIT(P1_6, 0x90, 6);
    SBIT(P1_7, 0x90, 7);

SFR(P2, 0xA0); // Port 2 Register.
    SBIT(P2_0, 0xA0, 0);
    SBIT(P2_1, 0xA0, 1);
    SBIT(P2_2, 0xA0, 2);
    SBIT(P2_3, 0xA0, 3);
    SBIT(P2_4, 0xA0, 4);
    SBIT(P2_5, 0xA0, 5);
    SBIT(P2_6, 0xA0, 6);
    SBIT(P2_7, 0xA0, 7);

SFR(P3, 0xB0); // Port 3 Register.
    SBIT(P3_0, 0xB0, 0);
    SBIT(P3_1, 0xB0, 1);
    SBIT(P3_2, 0xB0, 2);
    SBIT(P3_3, 0xB0, 3);
    SBIT(P3_4, 0xB0, 4);
    SBIT(P3_5, 0xB0, 5);
    SBIT(P3_6, 0xB0, 6);
    SBIT(P3_7, 0xB0, 7);

#endif /*REG_W7100_H*/
