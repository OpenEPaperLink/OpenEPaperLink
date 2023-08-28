This custom RAMCode package includes the following files:

  - GlynGraphicBaseBoard_TMPA900_LP_NAND_LE.jflash (J-Flash project for NAND flash)
  - RAMCodeV2_GlynGraphicBaseBoard_LP_NAND_LE.mot (Custom RAMCode, compiled for little endian (LE))
  - Readme.txt (This readme)
  - SEGGERDemo.bin (SEGGER demo which boots from NAND flash)

*************************************************************************************************************

This RAMCode is designed to be used and has been tested on an Glyn Graphic Base Board (Rev 0A0) with
the Toshiba TMPA900 CPU module using a Samsung K9F2G08U0A NAND flash

*************************************************************************************************************

Requirements to use this RAMCode:
  - Glyn Graphic Base Board (Rev 0A0) with Toshiba TMPA900 CPU module compatible hardware
  - The J-Flash project assumes that an 25 MHz oscillator is used on X1.
    Otherwise CPU init sequence in J-Flash needs to be changed