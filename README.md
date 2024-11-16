# OpenEPaperLink

This is an alternative firmware and protocol for the multiple Electronic Shelf Labels. It can be used to set up E-Paper tags and supply them with content.

The software in this project consists of two parts: Accesspoint-firmware and Tag firmware.
Additionally, there are various hardware designs for accesspoints and flasher-interfaces to program the tags, preferably using programming jigs
 
## Aims
- Low power (currently around 9µA with a minimum of 40 second latency)
- Even lower power when there's no AP around
- Low latency (tags can check for new data every 40 seconds)
- High transfer speeds - It can do about 5kbyte/s in favorable RF conditions. This allows for lower power
- RF-friendly - We don't need to acknowledge EVERY packet, and we don't need to transfer data we already have

## What is required to set up OpenEPaperLink

On the [Wiki](https://github.com/jjwbruijn/OpenEPaperLink/wiki) there is a ton of information regarding all kinds of tags and building the access points.

The entire setup requires a few tags and an ESP32. A tag is used as an 802.15.4 radio for the ESP32. You'll need a ZBS_Flasher in order to flash the tags. Using the 'mac' option on ZBS_Flasher makes sure a tag flashed with a custom firmware has a valid mac address; it uses the stock mac address assigned to the tag if it hasn't been flashed before. If you want to set it yourself, you can edit the mac address in the infopage.

You can hook the AP tag up to the ESP32 with mod wires or a flex PCB. The esp will flash the AP firmware to the Tag automatically. In some cases, a power off/on cycle is required. Please check the serial console output for status information.

After programming the ESP32, make sure to also program the filesystem. This will upload the 'data' folder to the ESP32, with the webinterface.

## OpenEPaperLink Parts and links
* [Mini AP](https://github.com/OpenEPaperLink/Hardware/tree/main/OpenEPaperLink%20Mini%20AP)
* [Combined Flasher and AP](https://github.com/OpenEPaperLink/Hardware/tree/main/OpenEPaperLink%20AP%20and%20Flasher)
* [ATC1441's ZBS flasher page](https://github.com/atc1441/ZBS_Flasher)
* [Tags specs](https://github.com/jjwbruijn/OpenEPaperLink/wiki#tags)
* [Tags troubleshooting](https://github.com/jjwbruijn/OpenEPaperLink/wiki/Troubleshooting-Tags)

## The protocol explained
- The tag checks in with the AP every 40+ seconds. Actual check-in interval is highly dependent on RF conditions, or if the AP tells the tag to delay the next check-in
- The AP holds a list with tag MACs that have pending transfers.
- If a tag checks in, the AP replies with either no data, or information about a pending transfer
- The tag checks if this information is already downloaded to EEPROM, or is already displayed. If this is the case, the transfer is immediately cancelled by issuing a 'transfer complete' packet to the AP.
- The tag then proceeds to request data in 'blocks' of 4096 bytes. The AP responds with an ACK on the request, and specifies how long it will take to gather the data. The tag sleeps until the AP will send the data
- The AP requests its block-buffer to be filled by the ESP32, specifying MD5 and blockID
- Datablock is sent by the AP, which will take about 42 packets for a full block. The tag will keep track of which blocks it has seen
- After a block has been received with missing parts, the tag will request the same block, with a bitmask of blocks that are missing
- The AP responds with the parts as requested by the tag. If there aren't too many blocks requested, the AP will fill the block with duplicate parts, to increase the chance of a successful transmission
- After a full block has been received, the tag will do a simple checksum over all received data. If the checksum matches, the 4K of data is stored in EEPROM storage for later use
- The tag will now either request the next block, or do a full re-request if the checksum failed
- If all blocks are received, the tag will send a 'transfer complete'. 
- The AP will report the completed transfer to the ESP32, and removes the pendingData for this transfer from the queue

## Known issues:
- Some tags work better as APs than others. Your range may suck. The boards on these tags are tiny and fragile. For instance, a dab of hot-glue on a board is enough to warp it pretty severely, and will damage the components that are soldered on there. Reportedly, segmented-display Solum tags work well. 

## Hints and excuses:
- There is no warranty whatsoever. Nothing. Not implied or otherwise suggested. This code isn't fit for anything. Please don't use this code to do nasty things.
- Do a ```make clean``` between buildings for different boards. It really helps!
- This repo builds on SDCC 4.2.0 for Linux. Different SDCC versions can behave VERY differently.  Source sdcc/setup.sh to setup build a local copy and use it for compiling.
- We are happy and honored to see your pull requests! But please, no code/indent style changes :)

## Credits
### Large parts of this repo are based on code written by, and wouldn't be possible without the hard work of:
- dmitry.gr
- atc1441

Hats off to these legends!
 
## Automated Builds
- After a PR gets merged to the main branch, the changed code will be compiled.
- On release, files are added to enable OTA

![Test](https://github.com/jjwbruijn/OpenEPaperLink/actions/workflows/conditional-build-test.yml/badge.svg)

![Release](https://github.com/jjwbruijn/OpenEPaperLink/actions/workflows/release.yml/badge.svg)

## License
[Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)](https://creativecommons.org/licenses/by-nc-sa/4.0/)

<img width="150" src="https://github.com/jjwbruijn/OpenEPaperLink/assets/2544995/0f3c945f-377e-49a4-a431-cd9e111f997f">
