# Tag Flasher

You can use the following flasher-scripts to program various types of tags with the Tag-Flasher or AP-Flasher. The Tag-Flasher is the easiest to build; it uses only a Wemos S2-Mini or clone to program quite a few different tags. The tags below are the tags that can be programmed with this flasher.
* ZBS243-based tags
* nRF52-based tags
* 88MZ100-based tags

This is the schematic for the flasher, including a pinout for the debug-header that is used by quite a few Solum tags. This flasher is also compatible with the OEPL-AP-Flasher jig/programming boards. 
![image](https://github.com/jjwbruijn/OpenEPaperLink/assets/2544995/1fa5bef8-6624-4e53-9caa-aeaaf4dbdc55)

This is what it looks like with the wires connected. I've chosen to use 0.635mm pitch wires, to be able to use the jigs with the 1.27mm FDC connectors. After soldering, you can add a little bit of hot-glue to make sure they stay attached.

![image](https://github.com/jjwbruijn/OpenEPaperLink/assets/2544995/c89d5de3-be3e-49a3-aa7e-7aaa0b8dcacb)

## Tag - Flasher connections

* R - required
* O - optional

Flasher Pin | Cable Pin | ESP32-S2 Pin | ZBS | nRF | 88mz100
:-------------------------:|:------:|:-----------------------:|:----:|:------:|:---:
VCC | 1 | 16, 17, 18, 21 | R | R | R
CS | 2 | 34 | R |  |
GND | 3 | GND | R | R | R
CLK | 4 | 33 | R | R | 
TXD | 5 | 36 | O | O | R
MISO | 6 | 35 | R | R |
TEST | 7 | 38 | 
MOSI | 8 | 37 | R |
RXD | 9 | 40 | O | O | R
RSET | 10 | 39 | R |  | O

Not all connections are required by all tags! If you want to solder fewer wires, skip the optional and unused ones.

## Flashing the flasher
Clone the [Tag_Flasher repo](https://github.com/jjwbruijn/OpenEPaperLink/tree/master/Tag_Flasher/ESP32_Flasher) and open into PlatformIO. Choose the correct COM-port and hit 'Upload'.
Also, the precompiled binaries are part of any [release](https://github.com/jjwbruijn/OpenEPaperLink/releases), and to make it even easier, you can use the web flasher on https://install.openepaperlink.de to even flash it without installing any extra software.

## OEPL-Flasher.py

This script connects to the S2-mini's serial port and enables flashing to ZBS243 and nRF52811-based tags.
```
usage: OEPL-Flasher.py [-h] [-p PORT] [-f] [-i] [-n] [-z] [--internalap] [-e] [--altradio] [--pt] {read,write,autoflash,debug} [filename]

OpenEPaperLink Flasher for AP/Flasher board

positional arguments:
  {read,write,autoflash,debug}
                        Command to execute
  filename              Filename for read/write commands

options:
  -h, --help            show this help message and exit
  -p PORT, --port PORT  COM port to use
  -f, --flash           Write to the flash
  -i, --infopage        Write to the infopage/UICR
  -n, --nrf82511        nRF82511 programming
  -z, --zbs243          ZBS243 programming
  --internalap          Selects the internal accesspoint port
  -e, --external        Selects the external(side) port
  --altradio            Selects the alternate radio port
  --pt, --passthrough   Enters serial passthrough for debug output after flashing
```

## 88MZ100-OEPL/Serial-Flasher.py
This script connects through a serial-TTL interface or OEPL - Tag_Flasher or AP-Flasher.

```
Example: COM1 read file.bin, or COM1 write file.bin, or COM1 write_flash file.bin
To create an OTA file use: img file.bin
```

## Solum ZBS243-based
Use with the -z option for ZBS243. Autoflash is currently not implemented on the Tag_Flasher/S2 version.
```
python3 OEPL-Flasher.py -e -z -p COM31 read blaat.bin --flash --pt
```

## Nordic nRF-based
Use with the -n option for nRF52811
```
python3 OEPL-Flasher.py -e -n -p COM31 read blaat.bin --flash --pt
```

## Marvell 88MZ100-based
Main article [here](https://github.com/jjwbruijn/OpenEPaperLink/wiki/88MZ100-Programming-and-interfacing).

The Tag-Flasher is used in serial passthrough-mode in order to flash the 88MZ100 with a modified flasher-script. A serial DTR/Reset line is used to reset the tag during the connection stage; this is however not strictly necessary! If you connect or reset the tag when prompted by the script, the flasher will work fine.
```
python3 .\88MZ100-OEPL-Flasher.py COM31 write_flash '0130c8144117.bin'
```

## Credits
Much code was reused from ATC1441's various flashers
* [ATC1441's ESP32-NRF52-SWD](https://github.com/atc1441/ESP32_nRF52_SWD)
* [ATC1441's ZBS-Flasher](https://github.com/atc1441/ZBS_Flasher)
* [ATC1441's 88MZ100 Flasher](https://github.com/atc1441/88MZ100/tree/master/88MZ100_Flasher)
