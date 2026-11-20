# Tag Flasher

You can use the following flasher-scripts to program various types of tags with the Tag-Flasher or AP-Flasher. The Tag-Flasher is the easiest to build; it uses only a Wemos S2-Mini or clone to program quite a few different tags. The tags below are the tags that can be programmed with this flasher.

* ZBS243-based tags
* nRF52-based tags
* 88MZ100-based tags
* cc1110-based tags

The 88MZ100 and CC2630 scripts further down also work with a plain 3.3 V USB-TTL adapter.

This is the schematic for the flasher, including a pinout for the debug-header that is used by quite a few Solum tags. This flasher is also compatible with the OEPL-AP-Flasher jig/programming boards.
![image](https://github.com/jjwbruijn/OpenEPaperLink/assets/2544995/1fa5bef8-6624-4e53-9caa-aeaaf4dbdc55)

This is what it looks like with the wires connected. I've chosen to use 0.635mm pitch wires, to be able to use the jigs with the 1.27mm FDC connectors. After soldering, you can add a little bit of hot-glue to make sure they stay attached.

![image](https://github.com/jjwbruijn/OpenEPaperLink/assets/2544995/c89d5de3-be3e-49a3-aa7e-7aaa0b8dcacb)

## Tag - Flasher connections

* R - required
* O - optional

| Flasher Pin | Cable Pin |  ESP32-S2 Pin  |  ZBS  |  nRF  | 88mz100 | CC1110 |
| :---------: | :-------: | :------------: | :---: | :---: | :-----: | :----: |
|     VCC     |     1     | 16, 17, 18, 21 |   R   |   R   |    R    |    R   |
|     CS      |     2     |       34       |   R   |       |         |        |
|     GND     |     3     |      GND       |   R   |   R   |    R    |    R   |
|     CLK     |     4     |       33       |   R   |   R   |         | (DC) R |
|     TXD     |     5     |       36       |   O   |   O   |    R    |        |
|    MISO     |     6     |       35       |   R   |   R   |         | (DD) R |
|    TEST     |     7     |       38       |       |       |         |        |
|    MOSI     |     8     |       37       |   R   |       |         |        |
|     RXD     |     9     |       40       |   O   |   O   |    R    |        |
|    RESET    |    10     |       39       |   R   |       |    O    |    R   |

Not all connections are required by all tags! If you want to solder fewer wires, skip the optional and unused ones.

## Flashing the flasher

Clone the [Tag_Flasher repo](https://github.com/jjwbruijn/OpenEPaperLink/tree/master/Tag_Flasher/ESP32_Flasher) and open into PlatformIO. Choose the correct COM-port and hit 'Upload'.
Also, the precompiled binaries are part of any [release](https://github.com/jjwbruijn/OpenEPaperLink/releases), and to make it even easier, you can use the web flasher on [https://install.openepaperlink.de](https://install.openepaperlink.de) to even flash it without installing any extra software.

## OEPL-Flasher.py

This script connects to the S2-mini's serial port and enables flashing to ZBS243 and nRF52811-based tags.

```shell
usage: OEPL-Flasher.py [-h] [-p PORT] [-f] [-i] [-n] [-z] [-c] [--internalap] [-e] [--altradio] [--pt] {read,write,autoflash,debug} [filename]

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
  -c, --ccxxxx          CCxxxx programming
  --internalap          Selects the internal accesspoint port
  -e, --external        Selects the external(side) port
  --altradio            Selects the alternate radio port
  --pt, --passthrough   Enters serial passthrough for debug output after flashing
```

## 88MZ100-OEPL/Serial-Flasher.py

This script connects through a serial-TTL interface or OEPL - Tag_Flasher or AP-Flasher.

```shell
Example: COM1 read file.bin, or COM1 write file.bin, or COM1 write_flash file.bin
To create an OTA file use: img file.bin
```

## Solum ZBS243-based

Use with the -z option for ZBS243. Autoflash is currently not implemented on the Tag_Flasher/S2 version.

```shell
python3 OEPL-Flasher.py -e -z -p COM31 read blaat.bin --flash --pt
```

## Nordic nRF-based

Use with the -n option for nRF52811

```shell
python3 OEPL-Flasher.py -e -n -p COM31 read blaat.bin --flash --pt
```

**Note:** If something's gone wrong with the UICR partition on your nRF tag, you can use the [`nrf_uicr_mac_patcher.py`](nrf_uicr_mac_patcher.py) tool to patch a known good UICR partition dump with the MAC address of your broken tag.

## Marvell 88MZ100-based

Main article [here](https://github.com/jjwbruijn/OpenEPaperLink/wiki/88MZ100-Programming-and-interfacing).

The Tag-Flasher is used in serial passthrough-mode in order to flash the 88MZ100 with a modified flasher-script. A serial DTR/Reset line is used to reset the tag during the connection stage; this is however not strictly necessary! If you connect or reset the tag when prompted by the script, the flasher will work fine.

```shell
python3 .\88MZ100-OEPL-Flasher.py COM31 write_flash '0130c8144117.bin'
```

### 88MZ100-Flasher-robust.py (plain USB-TTL adapter, hand-held jig)

A variant of `88MZ100-Serial-Flasher.py` for a plain 3.3 V USB-TTL adapter and a press-on/Dupont jig whose contacts tend to bounce. It uses the same protocol and the same `88mz100_second_stage_bootloader.bin`, but:

* answers every boot prompt and only starts once the line has stayed quiet for 1.5 s (debounce),
* restarts automatically after a communication error: just reconnect 3V3, no need to restart the script,
* `read` writes to exactly the given file and refuses to start if it already exists (it never appends).

Connect TXD, RXD and GND first and plug in the 3V3 wire last, when the script asks for it.

```shell
python3 88MZ100-Flasher-robust.py COM4 read backup.bin
python3 88MZ100-Flasher-robust.py COM4 write_flash SOL_M2_75_full_0028.bin
```

## TI CC2630-based (cc2630-flash-robust.py / cc2630-read-robust.py)

For tags like the Solum TG-GR7500C. These scripts talk to the TI ROM serial bootloader through a plain 3.3 V USB-TTL adapter and use the command layer of [cc2538-bsl](https://github.com/JelmerT/cc2538-bsl) by Jelmer Tiete. Install it with

```shell
pip install git+https://github.com/JelmerT/cc2538-bsl
```

or set `CC2538_BSL_PATH` to a checkout of it (a `cc2538-bsl` checkout next to the scripts is found as well).

Wiring: adapter TXD → tag RXD, adapter RXD → tag TXD, GND → GND, 3V3 → BAT (connect last). Hold `TI_DN` on GND while power comes on to enter the bootloader (backdoor on DIO11 on the TG-GR7500C).

* Both scripts keep sending the synch sequence while you connect power, so bouncing contacts don't matter.
* `cc2630-read-robust.py` resumes at the same address after an error and never overwrites an existing file. It prints the CCFG `BL_CONFIG` at the end.
* `cc2630-flash-robust.py` expects a full 128 KB image including CCFG. It refuses images whose CCFG disables the serial bootloader (you could only recover such a tag with JTAG). It bank-erases once, then writes sector by sector with a CRC check per sector and writes the CCFG sector last, and verifies a full-flash CRC at the end. After an error it waits for the bootloader again and redoes only the failed sector. An erased CC2630 always starts the ROM bootloader, so `TI_DN` is only needed for the first connection.

```shell
python3 cc2630-read-robust.py COM4 tg-gr7500c-original.bin
python3 cc2630-flash-robust.py COM4 Tag_FW_CC2630_TG-GR7500C.bin
```

## TI CC1110-based

> [!IMPORTANT]
> Tag flasher version 50 or later is required for CC1110 support.
> 
Use with the -c option for CC1110. 

The CC1110 does not have an infopage nor is the Tag's EEPROM accessable. Autoflash is currently not implemented for the CC1110.

```shell
python3 OEPL-Flasher.py -e -c -p COM31 read blaat.bin --flash
```

See this [page](https://github.com/OpenEPaperLink/OpenEPaperLink/wiki/Chroma-Series-SubGhz-Tags#flashing-cc1110-based-chroma-tags) 
on the Wiki for additional information.

## EFR32-based

This flasher does **NOT** support tags based on the EFR32.  See the [wiki](https://github.com/OpenEPaperLink/OpenEPaperLink/wiki/Flashing-SiLabs-based-M3-Newton-Displays) for information on how to flash those.
## Credits

Much code was reused from ATC1441's various flashers
* [ATC1441's ESP32-NRF52-SWD](https://github.com/atc1441/ESP32_nRF52_SWD)
* [ATC1441's ZBS-Flasher](https://github.com/atc1441/ZBS_Flasher)
* [ATC1441's 88MZ100 Flasher](https://github.com/atc1441/88MZ100/tree/master/88MZ100_Flasher)
* [ATC1441's ESP_CC_Flasher](https://github.com/atc1441/ESP_CC_Flasher)
* [JelmerT's cc2538-bsl](https://github.com/JelmerT/cc2538-bsl) (used by the CC2630 scripts)

