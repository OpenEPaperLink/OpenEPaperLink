# OpenEPaperLink

## ⚠️⚠️⚠️THIS IS NOT PRODUCTION READY!⚠️⚠️⚠️
This is not a final, polished codebase. Not by a long shot. You'll need some knowledge on the use of these tags. A very good place to start is here: https://github.com/atc1441/ZBS_Flasher. You'll need to fix issues yourself, troubleshoot stuff. Once again: this is not for everyone.

This is an alternative firmware and protocol for the ZBS243-based Electronic Shelf Labels - ESL / price tags by Solum / Samsung.
 
<img width="600" alt="board" src="https://user-images.githubusercontent.com/2544995/227208651-993d11a2-380f-44ae-847c-f8d90296c8cf.png">

### Compatibility
It is currently compatible with the following tags:
* 4.2"
* 2.9"
* 1.54"
* Segmented (work in progress)

On the 2.9" tags, both the UC8151 and SSD1619 display variants are supported

### Aims
- Low power (currently around 9µA with a minimum of 40 second latency)
- Even lower power when there's no AP around
- Low latency (tags can check for new data every 40 seconds)
- High transfer speeds - It can do about 5kbyte/s in favorable RF conditions. This allows for lower power
- RF-friendly - We don't need to acknowledge EVERY packet, and we don't need to transfer data we already have

The entire setup requires a few tags, and an ESP32. A tag is used as an 802.15.4 radio for the ESP32. You'll need a ZBS_Flasher in order to flash the tags. Using the 'mac' option on ZBS_Flasher makes sure a tag flashed with a custom firmware has a valid mac address; it used the stock mac address assigned to the tag if it hasn't been flashed before. If you want to set it yourself, you can edit the mac address in the infopage. The AP expects a tag with a mac that starts with 00:00, followed by 6 bytes.

You can hook the AP tag up to the ESP32 with mod wires or a flex pcb. The esp will flash the AP fitmware to the Tag automatically. In some case, a power off on cicle is reqired. Please check the serial console output for status information.

You can access the ESP32 with any web browser after connecting it to your WiFi Network. The file browser is located at <ip>/edit. For sending data to tags, you'll need to upload the information in 'data' to the ESP32's filesystem or over HTTP. After uploading, you can access the status screen at <ip>/index.html. If everything is working, you should be able to see tags synchronising to the network. After uploading a suitable .bmp file to the filesystem, this file can be sent to the tag by entering it's 6-byte mac address and filename.

## The protocol explained
- The tag checks in with the AP every 40+ seconds. Actual check-in interval is highly dependent on RF conditions
- The AP holds a list with tag MAC's that have pending transfers.
- If a tag checks in, the AP replies with either no data, or information about a pending transfer
- The tag checks if this information is already downloaded to EEPROM, or is already displayed. If this is the case, the transfer is immediately cancelled by issuing a 'transfer complete' packet to the AP.
- The tag then proceeds to request data in 'blocks' of 4096 bytes. The AP responds with an ACK on the request, and specifies how long it will spend to gather the data. The tag sleeps until the AP will send the data
- The AP requests its block-buffer to be filled by the ESP32, specifying MD5 and blockID
- Datablock is sent by the AP, which will take about 42 packets for a full block. The tag will keep track of which blocks it has seen
- After a block has been received with missing parts, the tag will request the same block, with a bitmask of blocks that are missing
- The AP responds with the parts as requested by the tag. If there aren't too many blocks requested, the AP will fill the block with duplicate parts, to increase the chance of a successful transmission
- After a full block has been received, the tag will do a simple checksum over all received data. If the checksum matches, the 4K of data is stored in EEPROM storage for later use
- The tag will now either request the next block, or do a full re-request if the checksum failed
- If all blocks are received, the tag will send a 'transfer complete'. 
- The AP will report the completed transfer to the ESP32, and removes the pendingData for this transfer from the queue

## Todo:
- Code cleanup... Splitting into different files, for instance. It's a mess.
### Tags:
- Implement NFC for URL's
### AP:
- Important! The AP needs to be able to tell a tag to try again later if it's already doing comms with another tag. The AP can't handle concurrent checkins/download due to memory constraints!
- More reliable serial comms (sometimes bytes are dropped)
- Include source mac with blockrequest struct 
### ESP32:
- Do more with status info as sent by the tags

## Known issues:
- Some tags work better as AP's than others. Your range may suck. The boards on these tags are tiny and fragile. For instance, a dab of hot-glue on a board is enough to warp it pretty severely, and will damage the components that are soldered on there. Reportedly, segmented-display solum tags work well. 

## Hints and excuses:
- I'm sorry if reading this spaghetti code makes you lose your mind. <sub>'Of all the things I've lost, I miss my mind the most'</sub> I know it is pretty unreadable. I could blame SDCC for a lot of things, but it's mostly me.
- There is no warranty whatsoever. Nothing. Not implied or otherwise suggested. This code isn't fit for anything. Please don't use this code to do nasty things.
- Do a ```make clean``` between building for different boards. It really helps!
- This repo builds on SDCC 4.2.0 for Linux. Different SDCC versions can behave VERY differently.
- I am happy and honored to accept pull requests! But please, no code/indent style changes :)

## Credits
### Large parts of this repo are based on code written by, and wouldn't be possible without the hard work of:
- dmitry.gr
- atc1441

Hats off to these legends!
 
## Automated Builds
- After a PR gets merged to the main branch, the esp 32 code will automatically be compiled. This can take up to 20 minutes.
- Information about the latest builds can be found below
<img alt="builds" src="https://openepaperlink.de/build/img.php">

