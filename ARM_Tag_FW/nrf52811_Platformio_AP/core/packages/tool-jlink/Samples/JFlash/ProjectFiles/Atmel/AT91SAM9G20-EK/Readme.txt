This custom RAMCode package includes the following files:

  - AT91SAM9G20-EK_DataFlash.jflash (J-Flash project for DataFlash)
  - RAMCodeV2_AT91SAM9G20-EK_DataFlashAT45DBxxxD_LE.mot (Custom RAMCode, compiled for little endian (LE))
  - Readme.txt (This readme)
  - Test_LEDBlinky.bin (Sample application which boots from data flash and toggles two LEDs)

*************************************************************************************************************

This RAMCode is designed to be used and has been tested on an Atmel AT91SAM9G20-EK Rev. B,
using a AT45DB642D, but it should be compatible to all AT45DBxxxD devices

*************************************************************************************************************

Requirements to use this RAMCode:
  - AT91SAM9G20-EK Rev B. compatible hardware
  - DataFlash has to be connected to SPI0
  - nCS of DataFlash has to be connected to SPI0_nCS1 (PC11)
  - The J-Flash project assumes that an 18.43 MHz main oscillator is used.
    Otherwise you have to change the CPU init sequence