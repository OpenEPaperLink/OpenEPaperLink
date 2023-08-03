/*-------------------------------------------------------------------------
   XC866.h - register Declarations for the Infineon XC866

   Copyright (C) 2005, Llewellyn van Zyl <eduprep AT myconnection.co.za>

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

#ifndef XC866_H
#define XC866_H

//   SFR byte definitions
__sfr __at (0xE0) A;
__sfr __at (0xCA) ADC_CHCTR0;
__sfr __at (0xCB) ADC_CHCTR1;
__sfr __at (0xCC) ADC_CHCTR2;
__sfr __at (0xCD) ADC_CHCTR3;
__sfr __at (0xCE) ADC_CHCTR4;
__sfr __at (0xCF) ADC_CHCTR5;
__sfr __at (0xD2) ADC_CHCTR6;
__sfr __at (0xD3) ADC_CHCTR7;
__sfr __at (0xCB) ADC_CHINCR;
__sfr __at (0xCA) ADC_CHINFR;
__sfr __at (0xCD) ADC_CHINPR;
__sfr __at (0xCC) ADC_CHINSR;
__sfr __at (0xCA) ADC_CRCR1;
__sfr __at (0xCC) ADC_CRMR1;
__sfr __at (0xCB) ADC_CRPR1;
__sfr __at (0xCF) ADC_ETRCR;
__sfr __at (0xCF) ADC_EVINCR;
__sfr __at (0xCE) ADC_EVINFR;
__sfr __at (0xD3) ADC_EVINPR;
__sfr __at (0xD2) ADC_EVINSR;
__sfr __at (0xCA) ADC_GLOBCTR;
__sfr __at (0xCB) ADC_GLOBSTR;
__sfr __at (0xCE) ADC_INPCR0;
__sfr __at (0xCD) ADC_LCBR;
__sfr __at (0xD1) ADC_PAGE;
__sfr __at (0xCC) ADC_PRAR;
__sfr __at (0xCF) ADC_Q0R0;
__sfr __at (0xD2) ADC_QBUR0;
__sfr __at (0xD2) ADC_QINR0;
__sfr __at (0xCD) ADC_QMR0;
__sfr __at (0xCE) ADC_QSR0;
__sfr __at (0xCA) ADC_RCR0;
__sfr __at (0xCB) ADC_RCR1;
__sfr __at (0xCC) ADC_RCR2;
__sfr __at (0xCD) ADC_RCR3;
__sfr __at (0xCB) ADC_RESR0H;
__sfr __at (0xCA) ADC_RESR0L;
__sfr __at (0xCD) ADC_RESR1H;
__sfr __at (0xCC) ADC_RESR1L;
__sfr __at (0xCF) ADC_RESR2H;
__sfr __at (0xCE) ADC_RESR2L;
__sfr __at (0xD3) ADC_RESR3H;
__sfr __at (0xD2) ADC_RESR3L;
__sfr __at (0xCB) ADC_RESRA0H;
__sfr __at (0xCA) ADC_RESRA0L;
__sfr __at (0xCD) ADC_RESRA1H;
__sfr __at (0xCC) ADC_RESRA1L;
__sfr __at (0xCF) ADC_RESRA2H;
__sfr __at (0xCE) ADC_RESRA2L;
__sfr __at (0xD3) ADC_RESRA3H;
__sfr __at (0xD2) ADC_RESRA3L;
__sfr __at (0xCE) ADC_VFCR;
__sfr __at (0xF0) B;
__sfr __at (0xBD) BCON;
__sfr __at (0xBE) BG;
__sfr __at (0xFB) CCU6_CC60RH;
__sfr __at (0xFA) CCU6_CC60RL;
__sfr __at (0xFB) CCU6_CC60SRH;
__sfr __at (0xFA) CCU6_CC60SRL;
__sfr __at (0xFD) CCU6_CC61RH;
__sfr __at (0xFC) CCU6_CC61RL;
__sfr __at (0xFD) CCU6_CC61SRH;
__sfr __at (0xFC) CCU6_CC61SRL;
__sfr __at (0xFF) CCU6_CC62RH;
__sfr __at (0xFE) CCU6_CC62RL;
__sfr __at (0xFF) CCU6_CC62SRH;
__sfr __at (0xFE) CCU6_CC62SRL;
__sfr __at (0x9B) CCU6_CC63RH;
__sfr __at (0x9A) CCU6_CC63RL;
__sfr __at (0x9B) CCU6_CC63SRH;
__sfr __at (0x9A) CCU6_CC63SRL;
__sfr __at (0xA7) CCU6_CMPMODIFH;
__sfr __at (0xA6) CCU6_CMPMODIFL;
__sfr __at (0xFF) CCU6_CMPSTATH;
__sfr __at (0xFE) CCU6_CMPSTATL;
__sfr __at (0x9D) CCU6_IENH;
__sfr __at (0x9C) CCU6_IENL;
__sfr __at (0x9F) CCU6_INPH;
__sfr __at (0x9E) CCU6_INPL;
__sfr __at (0x9D) CCU6_ISH;
__sfr __at (0x9C) CCU6_ISL;
__sfr __at (0xA5) CCU6_ISRH;
__sfr __at (0xA4) CCU6_ISRL;
__sfr __at (0xA5) CCU6_ISSH;
__sfr __at (0xA4) CCU6_ISSL;
__sfr __at (0xA7) CCU6_MCMCTR;
__sfr __at (0x9B) CCU6_MCMOUTH;
__sfr __at (0x9A) CCU6_MCMOUTL;
__sfr __at (0x9F) CCU6_MCMOUTSH;
__sfr __at (0x9E) CCU6_MCMOUTSL;
__sfr __at (0xFD) CCU6_MODCTRH;
__sfr __at (0xFC) CCU6_MODCTRL;
__sfr __at (0xA3) CCU6_PAGE;
__sfr __at (0x9F) CCU6_PISEL0H;
__sfr __at (0x9E) CCU6_PISEL0L;
__sfr __at (0xA4) CCU6_PISEL2;
__sfr __at (0xA6) CCU6_PSLR;
__sfr __at (0xA5) CCU6_T12DTCH;
__sfr __at (0xA4) CCU6_T12DTCL;
__sfr __at (0xFB) CCU6_T12H;
__sfr __at (0xFA) CCU6_T12L;
__sfr __at (0x9B) CCU6_T12MSELH;
__sfr __at (0x9A) CCU6_T12MSELL;
__sfr __at (0x9D) CCU6_T12PRH;
__sfr __at (0x9C) CCU6_T12PRL;
__sfr __at (0xFD) CCU6_T13H;
__sfr __at (0xFC) CCU6_T13L;
__sfr __at (0x9F) CCU6_T13PRH;
__sfr __at (0x9E) CCU6_T13PRL;
__sfr __at (0xA7) CCU6_TCTR0H;
__sfr __at (0xA6) CCU6_TCTR0L;
__sfr __at (0xFB) CCU6_TCTR2H;
__sfr __at (0xFA) CCU6_TCTR2L;
__sfr __at (0x9D) CCU6_TCTR4H;
__sfr __at (0x9C) CCU6_TCTR4L;
__sfr __at (0xFF) CCU6_TRPCTRH;
__sfr __at (0xFE) CCU6_TRPCTRL;
__sfr __at (0xBA) CMCON;
__sfr __at (0x83) DPH;
__sfr __at (0x82) DPL;
__sfr __at (0xA2) EO;
__sfr __at (0xB7) EXICON0;
__sfr __at (0xBA) EXICON1;
__sfr __at (0xBD) FEAH;
__sfr __at (0xBC) FEAL;
__sfr __at (0xF7) HWBPDR;
__sfr __at (0xF6) HWBPSR;
__sfr __at (0xB3) ID;
__sfr __at (0xA8) IEN0;
__sfr __at (0xE8) IEN1;
__sfr __at (0xB8) IP;
__sfr __at (0xF8) IP1;
__sfr __at (0xB9) IPH;
__sfr __at (0xF9) IPH1;
__sfr __at (0xB4) IRCON0;
__sfr __at (0xB5) IRCON1;
__sfr __at (0xF3) MMBPCR;
__sfr __at (0xF1) MMCR;
__sfr __at (0xE9) MMCR2;
__sfr __at (0xF5) MMDR;
__sfr __at (0xF4) MMICR;
__sfr __at (0xF2) MMSR;
__sfr __at (0xB3) MODPISEL;
__sfr __at (0xBB) NMICON;
__sfr __at (0xBC) NMISR;
__sfr __at (0xB6) OSC_CON;
__sfr __at (0x80) P0_ALTSEL0;
__sfr __at (0x86) P0_ALTSEL1;
__sfr __at (0x80) P0_DATA;
__sfr __at (0x86) P0_DIR;
__sfr __at (0x80) P0_OD;
__sfr __at (0x86) P0_PUDEN;
__sfr __at (0x80) P0_PUDSEL;
__sfr __at (0x90) P1_ALTSEL0;
__sfr __at (0x91) P1_ALTSEL1;
__sfr __at (0x90) P1_DATA;
__sfr __at (0x91) P1_DIR;
__sfr __at (0x90) P1_OD;
__sfr __at (0x91) P1_PUDEN;
__sfr __at (0x90) P1_PUDSEL;
__sfr __at (0xA0) P2_DATA;
__sfr __at (0xA1) P2_PUDEN;
__sfr __at (0xA0) P2_PUDSEL;
__sfr __at (0xB0) P3_ALTSEL0;
__sfr __at (0xB1) P3_ALTSEL1;
__sfr __at (0xB0) P3_DATA;
__sfr __at (0xB1) P3_DIR;
__sfr __at (0xB0) P3_OD;
__sfr __at (0xB1) P3_PUDEN;
__sfr __at (0xB0) P3_PUDSEL;
__sfr __at (0xBB) PASSWD;
__sfr __at (0x87) PCON;
__sfr __at (0xB7) PLL_CON;
__sfr __at (0xB4) PMCON0;    
__sfr __at (0xB5) PMCON1;    
__sfr __at (0xB2) PORT_PAGE;    
__sfr __at (0xD0) PSW;    
__sfr __at (0x99) SBUF;    
__sfr __at (0x98) SCON;    
__sfr __at (0xBF) SCU_PAGE;    
__sfr __at (0x81) SP;    
__sfr __at (0xAF) SSC_BRH;    
__sfr __at (0xAE) SSC_BRL;    
__sfr __at (0xAB) SSC_CONH_O;    
__sfr __at (0xAB) SSC_CONH_P;    
__sfr __at (0xAA) SSC_CONL_O;    
__sfr __at (0xAA) SSC_CONL_P;    
__sfr __at (0xA9) SSC_PISEL;    
__sfr __at (0xAD) SSC_RBL;    
__sfr __at (0xAC) SSC_TBL;    
__sfr __at (0x8F) SYSCON0;    
__sfr __at (0xC3) T2_RC2H;    
__sfr __at (0xC2) T2_RC2L;    
__sfr __at (0xC0) T2_T2CON;    
__sfr __at (0xC5) T2_T2H;
__sfr __at (0xC4) T2_T2L;
__sfr __at (0xC1) T2_T2MOD;
__sfr __at (0x88) TCON;
__sfr __at (0x8C) TH0;
__sfr __at (0x8D) TH1;
__sfr __at (0x8A) TL0;
__sfr __at (0x8B) TL1;
__sfr __at (0x89) TMOD;
__sfr __at (0xBB) WDTCON;    // located in the mapped SFR area
__sfr __at (0xBF) WDTH;      // located in the mapped SFR area
__sfr __at (0xBE) WDTL;      // located in the mapped SFR area
__sfr __at (0xBC) WDTREL;    // located in the mapped SFR area
__sfr __at (0xBD) WDTWINB;   // located in the mapped SFR area

__sfr __at (0xB3) XADDRH;    // beware this is in an sfr page!
__sfr __at (0xB3) _XPAGE;    // this is the name SDCC expects for this sfr

//   SFR bit definitions

/*  P0  */
__sbit __at (0x80) P0_0 ;
__sbit __at (0x81) P0_1 ;
__sbit __at (0x82) P0_2 ;
__sbit __at (0x83) P0_3 ;
__sbit __at (0x84) P0_4 ;
__sbit __at (0x85) P0_5 ;

/*  P1  */
__sbit __at (0x90) P1_0 ;
__sbit __at (0x91) P1_1 ;
__sbit __at (0x92) P1_5 ;
__sbit __at (0x93) P1_6 ;
__sbit __at (0x94) P1_7 ;

/*  P2  */
__sbit __at (0xA0) P2_0 ;
__sbit __at (0xA1) P2_1 ;
__sbit __at (0xA2) P2_2 ;
__sbit __at (0xA3) P2_3 ;
__sbit __at (0xA4) P2_4 ;
__sbit __at (0xA5) P2_5 ;
__sbit __at (0xA6) P2_6 ;
__sbit __at (0xA7) P2_7 ;

/*  P3  */
__sbit __at (0xB0) P3_0 ;
__sbit __at (0xB1) P3_1 ;
__sbit __at (0xB2) P3_2 ;
__sbit __at (0xB3) P3_3 ;
__sbit __at (0xB4) P3_4 ;
__sbit __at (0xB5) P3_5 ;
__sbit __at (0xB6) P3_6 ;
__sbit __at (0xB7) P3_7 ;


//   IEN0
__sbit __at (0xAF) EA;
__sbit __at (0xAC) ES;
__sbit __at (0xA9) ET0;
__sbit __at (0xAB) ET1;
__sbit __at (0xAD) ET2;
__sbit __at (0xA8) EX0;
__sbit __at (0xAA) EX1;

//   IEN1
__sbit __at (0xE8) EADC;
__sbit __at (0xEC) ECCIP0;
__sbit __at (0xED) ECCIP1;
__sbit __at (0xEE) ECCIP2;
__sbit __at (0xEF) ECCIP3;
__sbit __at (0xE9) ESSC;
__sbit __at (0xEA) EX2;
__sbit __at (0xEB) EXM;

//   IP1
__sbit __at (0xF8) PADC;
__sbit __at (0xFC) PCCIP0;
__sbit __at (0xFD) PCCIP1;
__sbit __at (0xFE) PCCIP2;
__sbit __at (0xFF) PCCIP3;
__sbit __at (0xF9) PSSC;
__sbit __at (0xFA) PX2;
__sbit __at (0xFB) PXM;

//   IP
__sbit __at (0xBC) PS;
__sbit __at (0xB9) PT0;
__sbit __at (0xBB) PT1;
__sbit __at (0xBD) PT2;
__sbit __at (0xB8) PX0;
__sbit __at (0xBA) PX1;

//   PSW
__sbit __at (0xD6) AC;
__sbit __at (0xD7) CY;
__sbit __at (0xD5) F0;
__sbit __at (0xD1) F1;
__sbit __at (0xD2) OV;
__sbit __at (0xD0) P;
__sbit __at (0xD3) RS0;
__sbit __at (0xD4) RS1;

//   SCON
__sbit __at (0x9A) RB8;
__sbit __at (0x9C) REN;
__sbit __at (0x98) RI;
__sbit __at (0x9F) SM0;
__sbit __at (0x9E) SM1;
__sbit __at (0x9D) SM2;
__sbit __at (0x9B) TB8;
__sbit __at (0x99) TI;

//   T2_T2CON
__sbit __at (0xC0) CP_RL2;
__sbit __at (0xC3) EXEN2;
__sbit __at (0xC6) EXF2;
__sbit __at (0xC7) TF2;
__sbit __at (0xC2) TR2;

//   TCON
__sbit __at (0x89) IE0;
__sbit __at (0x8B) IE1;
__sbit __at (0x88) IT0;
__sbit __at (0x8A) IT1;
__sbit __at (0x8D) TF0;
__sbit __at (0x8F) TF1;
__sbit __at (0x8C) TR0;
__sbit __at (0x8E) TR1;

//   Definition of the PAGE SFR

//   PORT_PAGE
#define _pp0 PORT_PAGE=0 // PORT_PAGE postfix
#define _pp1 PORT_PAGE=1 // PORT_PAGE postfix
#define _pp2 PORT_PAGE=2 // PORT_PAGE postfix
#define _pp3 PORT_PAGE=3 // PORT_PAGE postfix

//   ADC_PAGE
#define _ad0 ADC_PAGE=0 // ADC_PAGE postfix
#define _ad1 ADC_PAGE=1 // ADC_PAGE postfix
#define _ad2 ADC_PAGE=2 // ADC_PAGE postfix
#define _ad3 ADC_PAGE=3 // ADC_PAGE postfix
#define _ad4 ADC_PAGE=4 // ADC_PAGE postfix
#define _ad5 ADC_PAGE=5 // ADC_PAGE postfix
#define _ad6 ADC_PAGE=6 // ADC_PAGE postfix

//   SCU_PAGE
#define _su0 SCU_PAGE=0 // SCU_PAGE postfix
#define _su1 SCU_PAGE=1 // SCU_PAGE postfix
#define _su2 SCU_PAGE=2 // SCU_PAGE postfix

//   CCU_PAGE
#define _cc0 CCU_PAGE=0 // CCU_PAGE postfix
#define _cc1 CCU_PAGE=1 // CCU_PAGE postfix
#define _cc2 CCU_PAGE=2 // CCU_PAGE postfix
#define _cc3 CCU_PAGE=3 // CCU_PAGE postfix

//   FLASH_PAGE
#define _fl0 FLASH_PAGE=0 // FLASH_PAGE postfix
#define _fl1 FLASH_PAGE=1 // FLASH_PAGE postfix
#define _fl2 FLASH_PAGE=2 // FLASH_PAGE postfix

#define SST0 0x80         // Save SFR page to ST0
#define RST0 0xC0         // Restore SFR page from ST0
#define SST1 0x90         // Save SFR page to ST1
#define RST1 0xD0         // Restore SFR page from ST1
#define SST2 0xA0         // Save SFR page to ST2
#define RST2 0xE0         // Restore SFR page from ST2
#define SST3 0xB0         // Save SFR page to ST3
#define RST3 0xF0         // Restore SFR page from ST3
#define noSST 0x00	  // Switch page without saving

#define SFR_PAGE(pg,op) pg+op

#endif
