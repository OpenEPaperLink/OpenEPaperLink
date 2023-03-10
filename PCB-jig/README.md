# The OpenEpaperLink PCB #
The OpenEpaperLink PCB is a set of PCBs that can help you flash the custom OpenEPaperLink firmware to some of the Solum ESL tags.

<img width="600" alt="board" src="gerbv-board.png">

<img width="600" alt="board" src="pcb-photo.jpg">

## Pogo / Interface boards ##
There are pogo interface boards for the following display types:
* 1.54" displays
* 2.9" displays
* 4.2" displays
* Segmented displays

In addition, there are a few loose debug-header footprints that can help to secure pogo pins in 3d-printed programming jigs.

### Parts (per board) ###
* 10x 1mm pogo pin (I'd take the rounded ones, but the pointy ones are fine too)
* 1x 90 degree angle 1.27mm 2x5 male IDC header (search for JTAG connector/header)
* 1x 6-pin 2.54mm angled pinheader
* 1x 1206-sized SMD LED
* 1x 1206 SMD resistor (47 ohm or so, depending on LED color, personal taste, whatever)

### Segmented, soldered board ###
As this board sits flush on the ESL board, it can't use through-hole components. It only uses:
* 1x SMD 2x5 male IDC header

For power consumption / debugging purposes, an optional 2.54mm 90-degree-angled pinheader can be added to connect a power profiler, such as the PPK2. A jumper or solder bridge needs to be added when not used or populated. 

## Flasher/ESP32 Board ##
This board expects an ESP32-S3 DevkitC1 board. If you want to use it as an AP and generate pictures on the ESP32, it is probably advisable to get an ESP32 with extra RAM, it'll probably scale better in the future. However, current codebase does not require extra RAM.

### Parts ###
* 1x ESP32-S3 DevkitC1
* 2x SMD 2x5 male IDC header
* 1x 90 degree angle 1.27mm 2x5 male IDC header
* 2x AO3401 P-channel MOSFET
* 2x AO3400 N-channel MOSFET
* 1x 1206-sized SMD LED
* 1x 1206 SMD resistor (47 ohm or so, depending on LED color, personal taste, whatever)
Some 1206 SMD resistors, not critical:
* 4x 100 (suggested) R4, R5, R8, R9
* 2x 4k7 (pull up) R6, R10
* 2x 47 (discharge shunt) R7, R11

### Errata ###:
* The tags have quite a lot of capacitance with a LOT of condensors over the power rail, these can cause the 3v3 rail to sag enough to cause the ESP32 to reset if power to the tag is switched on directly. Use a PWM to switch the tag on, and you're fine. It's also possible to add a small resistor in series with the tag, or add a bunch of extra capacitance to the ESP32.

## Use as Access Point ##
In addition, there is a sub-board that can be used to connect a segmented EPD-ESL to the flasher via a soldered connection. For this to work, you'll need to open the tag up and solder the interface board directly to the PCB. A fine pitched soldering iron tip and thin solder wire make this job easier. If you cut out a small slot in the back of the case, you can close the case up again.

Any tag can be used as an AP, when connected to the AP. The 'internal AP' header is meant for that purpose. This allows you to use the ESL as a 802.15.4 radio, using the ZBS243/SEM9110 over a serial link to the ESP32. The segmented ESL seems to be best suited for that, anecdotal evidence suggests the best range with this type of tag.

Please note that while this works mostly 'fine', these tags aren't built to be used as AP's. Tags are really built down to a price, and it shows. Some tags have a LOT better range than others. The PCB is pretty fragile and bends really easily; this can cause cracks in components and connections reducing range or killing the device altogether. Your milage may vary.

## Other radios ## 
While there is no current application or code within the project, it's possible to connect a A7105 or CC1101 to the board in order to connect to other wireless experiments. A small subboard is included for this specific purpose.

## Getting PCB's ##
You can order the boards from your favorite boardhouse, using the zip file in this repository. Some boardhouses don't like the amount of routing/milling this board requires, or add hefty fees depending on how many 'designs' are included in your board. As-is, according to some boardhouses, the PCB contains 9 different designs.

Also, this design uses castellated edges by milling through a debug-header footprint. Some houses don't like that either. I've had success ordering this PCB from DirtyPCBs, but this should not be seen as an endorsement; there's no guarantee they will continue to manufacture this board.

## Disclaimer ##
There is no guarantee or warranty whatsoever, nor is there any promise or insinuation that this board fill fullfill any particular purpose. This board may very well not work for you, set your hair and/or, but not limited to, pants on fire, incite violance or persuade other countries to invade your country. You're on your own, chief!