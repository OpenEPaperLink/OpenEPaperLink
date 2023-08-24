# Tag Flasher

You can use the following flasher-scripts to program various types of tags with the Tag-Flasher or AP-Flasher. The Tag-Flasher is the easiest to build; it uses only a Wemos S2-Mini or clone to program quite a few different tags. The tags below are the tags that can be programmed with this flasher.

This is the schematic for the flasher, including a pinout for the debug-header that is used by quite a few Solum tags. This flasher is also compatible with the OEPL-AP-Flasher jig/programming boards. 
![image](https://github.com/jjwbruijn/OpenEPaperLink/assets/2544995/25227120-d3cb-4c2d-b19d-99fdc795bda1)

This is what it looks like with the wires connected. I've chosen to use 0.635mm pitch wires, to be able to use the jigs with the 1.27mm FDC connectors. After soldering, you can add a little bit of hot-glue to make sure they stay attached.

![image](https://github.com/jjwbruijn/OpenEPaperLink/assets/2544995/c89d5de3-be3e-49a3-aa7e-7aaa0b8dcacb)

## Tag - Flasher connections
Flasher Pin | Cable Pin | ESP32-S2 Pin 
:-------------------------:|:------:|:-----------------------:
VCC | 1 | 16, 17, 18, 21
CS | 2 | 34
GND | 3 | GND
CLK | 4 | 33
TXD | 5 | 36
MISO | 6 | 35
TEST | 7 | 38
MOSI | 8 | 37
RXD | 9 | 40
RSET | 10 | 39

## Solum ZBS243-based

## Nordic nRF-based

## Marvell 88MZ100-based
