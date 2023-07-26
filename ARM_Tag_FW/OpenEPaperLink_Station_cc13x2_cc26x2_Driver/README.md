# CC1352 / CC2652 802.15.4 Firmware for E-Paper Station

<a href="CC2652P_USB_Dongle.jpg"><img width="600" alt="Example CC2652P_USB_Dongle" src="CC2652P_USB_Dongle.jpg"></a>

This firmware is meant to be compiled with the [Ti CCS IDE](https://www.ti.com/tool/CCSTUDIO)

The Precompiled .out or .hex files can be flashed via UART [FLASH-PROGRAMMER-2](https://www.ti.com/tool/download/FLASH-PROGRAMMER-2/1.8.2) or JTAG/cJTAG (With a compatible flasher like Segger)

This firmware will emulate the TiMAC UART Protocol and allows to send and receive RAW 802.15.4(ZigBee) Packets
the UART protocol is just as much implementet so that the E-Paper Station will work without problems.

See a demo of it running on an Sonoff CC2652P USB Stick:

https://www.youtube.com/watch?v=FDRU17OAT4w

[![YoutubeVideo](https://img.youtube.com/vi/FDRU17OAT4w/0.jpg)](https://www.youtube.com/watch?v=FDRU17OAT4w)
