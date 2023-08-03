#ifndef __R2K_H__
#define __R2K_H__ 1

#ifndef _RABREG
#define _RABREG(addr, reg) __sfr __at(addr) reg
#endif

_RABREG(0x00, GCSR);  // global control / status register

_RABREG(0x01, RTCCR); // Real Time Clock Control Register
_RABREG(0x02, RTC0R); // Real Time Clock Data Register 0
_RABREG(0x03, RTC1R); // Real Time Clock Data Register 1
_RABREG(0x04, RTC2R); // Real Time Clock Data Register 2
_RABREG(0x05, RTC3R); // Real Time Clock Data Register 3
_RABREG(0x06, RTC4R); // Real Time Clock Data Register 4
_RABREG(0x07, RTC5R); // Real Time Clock Data Register 5

_RABREG(0x08, WDTCR); // watch-dog timer control register
_RABREG(0x09, WDTTR); // watch-dog timer test register

_RABREG(0x0D, GPSCR); // global power save control register
_RABREG(0x0E, GOCR);  // global output control register
_RABREG(0x0F, GCDR);  // global clock double register

_RABREG(0x10, MMIDR); // MMI Instruction/Data Register
_RABREG(0x11, STACKSEG);
_RABREG(0x12, DATASEG);
_RABREG(0x13, SEGSIZE);

_RABREG(0x14, MB0CR); // Memory Bank 0 Control Register
_RABREG(0x15, MB1CR); // Memory Bank 1 Control Register
_RABREG(0x16, MB2CR); // Memory Bank 2 Control Register
_RABREG(0x17, MB3CR); // Memory Bank 3 Control Register

_RABREG(0x20, SPD0R); // Slave Port Data 0 Register
_RABREG(0x21, SPD1R); // Slave Port Data 1 Register
_RABREG(0x22, SPD2R); // Slave Port Data 2 Register
_RABREG(0x23, SPSR);  // Slave Port Status Register
_RABREG(0x24, SPCR);  // Slave Port Control Register

_RABREG(0x30, PADR);  // Port A Data Register
_RABREG(0x40, PBDR);  // Port B Data Register

_RABREG(0x50, PCDR);  // Port C Data Register
_RABREG(0x55, PCFR);  // Port C Function Register

_RABREG(0x60, PDDR);  // Port D Data Register
_RABREG(0x64, PDCR);  // Port D Control Register
_RABREG(0x64, PDDCR); // Port D Drive Control Register
_RABREG(0x67, PDDDR); // Port D Data Direction Register
_RABREG(0x68, PDB0R); // Port D Bit 0 Register
_RABREG(0x69, PDB1R); // Port D Bit 1 Register
_RABREG(0x6A, PDB2R); // Port D Bit 2 Register
_RABREG(0x6B, PDB3R); // Port D Bit 3 Register
_RABREG(0x6C, PDB4R); // Port D Bit 4 Register
_RABREG(0x6D, PDB5R); // Port D Bit 5 Register
_RABREG(0x6E, PDB6R); // Port D Bit 6 Register
_RABREG(0x6F, PDB7R); // Port D Bit 7 Register

_RABREG(0x70, PEDR);  // Port E data register
_RABREG(0x74, PECR);  // Port E control register
_RABREG(0x77, PEDDR); // Port E data direction register
_RABREG(0x78, PEB0R); // Port E Bit 0 Register
_RABREG(0x79, PEB1R); // Port E Bit 1 Register
_RABREG(0x7A, PEB2R); // Port E Bit 2 Register
_RABREG(0x7B, PEB3R); // Port E Bit 3 Register
_RABREG(0x7C, PEB4R); // Port E Bit 4 Register
_RABREG(0x7D, PEB5R); // Port E Bit 5 Register
_RABREG(0x7E, PEB6R); // Port E Bit 6 Register
_RABREG(0x7F, PEB7R); // Port E Bit 7 Register

_RABREG(0x80, IB0CR); // I/O Bank 0 Control Register
_RABREG(0x81, IB1CR); // I/O Bank 1 Control Register
_RABREG(0x82, IB2CR); // I/O Bank 2 Control Register
_RABREG(0x83, IB3CR); // I/O Bank 3 Control Register
_RABREG(0x84, IB4CR); // I/O Bank 4 Control Register
_RABREG(0x85, IB5CR); // I/O Bank 5 Control Register
_RABREG(0x86, IB6CR); // I/O Bank 6 Control Register
_RABREG(0x87, IB7CR); // I/O Bank 7 Control Register

_RABREG(0x98, I0CR);  // Interrupt 0 Control Register
_RABREG(0x99, I1CR);  // Interrupt 1 Control Register

_RABREG(0xA0, TACSR); // Timer A Control/Status Register
_RABREG(0xA3, TAT1R); // Timer A Time Constant 1 Register
_RABREG(0xA4, TACR);  // Timer A Control Register
_RABREG(0xA9, TAT4R); // Timer A Time Constant 4 Register
_RABREG(0xAB, TAT5R); // Timer A Time Constant 5 Register
_RABREG(0xAD, TAT6R); // Timer A Time Constant 6 Register
_RABREG(0xAF, TAT7R); // Timer A Time Constant 7 Register

_RABREG(0xB0, TBCSR); // Timer B Control/Status Register
_RABREG(0xB1, TBCR);  // Timer B Control Register
_RABREG(0xB2, TBM1R); // Timer B MSB 1 Register
_RABREG(0xB3, TBL1R); // Timer B LSB 1 Register
_RABREG(0xB4, TBM2R); // Timer B MSB 2 Register
_RABREG(0xB5, TBL2R); // Timer B LSB 2 Register
_RABREG(0xBE, TBCMR);  // Timer B Count MSB Register
_RABREG(0xBF, TBCLR);  // Timer B Count LSB Register

_RABREG(0xC0, SADR);  // Serial Port A Data Register
_RABREG(0xC1, SAAR);  // Serial Port A Adress Register
_RABREG(0xC3, SASR);  // Serial Port A Status Register
_RABREG(0xC4, SACR);  // Serial Port A Control Register

_RABREG(0xD0, SBDR);  // Serial Port B Data Register
_RABREG(0xD1, SBAR);  // Serial Port B Adress Register
_RABREG(0xD3, SBSR);  // Serial Port B Status Register
_RABREG(0xD4, SBCR);  // Serial Port B Control Register

_RABREG(0xE0, SCDR);  // Serial Port C Data Register
_RABREG(0xE1, SCAR);  // Serial Port C Adress Register
_RABREG(0xE3, SCSR);  // Serial Port C Status Register
_RABREG(0xE4, SCCR);  // Serial Port C Control Register

_RABREG(0xF0, SDDR);  // Serial Port D Data Register
_RABREG(0xF1, SDAR);  // Serial Port D Adress Register
_RABREG(0xF3, SDSR);  // Serial Port D Status Register
_RABREG(0xF4, SDCR);  // Serial Port D Control Register

#endif

