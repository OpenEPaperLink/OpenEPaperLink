# Troubleshooting Solum ZBS243-based pricetags

While these tags seem to be pretty dependable, there are a few failure modes
* Failure to (re)boot)
* Battery
* EPD Failure
* EPD voltage driver failure
* SPI EEPROM failure
* SOC/Flash failure

## Failure to (re)boot ##
The SOC/board doesn't have a reliable POR/BOD circuit that we know of. This means that the SOC needs a pretty clean/sharp application of power, otherwise it will enter an undefined state where it will consume power, but not do anything useful. This problems is exacerbated by the presence of some pretty large-ish bypass caps in on the tag-boards, and its low sleep current.

To put it simply: just removing the batteries and inserting them again doesn't reboot the tag, as the capacitors will remain charged. To cleanly 'boot' a tag, you'll need to shorten the battery pins to drain the capacitors, and then insert the battery(ies) in a smooth motion. This is a good example right[here.](https://youtu.be/98fOzZs__fc?t=2002)

Removing the batteries and inserting a battery backwards will shorten the contacts just fine, and is a perfectly acceptable way to do it.

## Battery failure ##
Dying batteries can be hard to diagnose sometimes. The CR2540 has limited pulse/high current-handling capabilities, and when a battery starts to become drained, its internal resistance will start to increase. This is impossible to see with only a high-impedance meter, as the float voltage will still read about 3v. As soon as a tag boots up, it will try to determine battery voltage, and the only way to reliably do this, is by loading the battery down a bit.

OpenEPaperLink will do this by enabling the radio during the voltage reading. However, if a battery is very bad, the voltage will sag down enough to crash the tag. As a tag doesn't have a brown-out detection circuit that we know of, and doesn't do a power on reset, it's not going to reset/cleanly. The SOC will stay in a partially crashed state, and will consume a few 10's of milliamps. This will drain whatever remains in the battery in a matter of hours. This issue will come up earlier for 1.54" tags, as they contain only one battery. That poor battery has to cope with the same current spikes all on its own...

If a tag boots up but hangs during the boot/AP searching process, it's most likely a bad battery. If the battery reads >=2.6v during boot, the battery is probably good enough to continue the boot process. 

Please note that the battery will never read more than 2.6v. This is due to the fact that battery monitoring on the SOC itself is currently undocumented; OpenEPaperLink uses the low-voltage-detect function on the EPD to determine the battery voltage. In normal use, the battery is checked once during boot, and after that approximately every 24 hours. This should be more than enough to show if a battery is becoming low. The tag will work properly down to about 2.15v, under load.

A dead battery can be more than a problem in itself, it can be a symptom of a problematic tag. Some tags have bad EPD driver chips (CoG), or bad a bad EEPROM that can cause a high quiescent current. This can sometimes be spotted by powering a tag with an external power supply, or using a power profiler such as a PPK2. This will drain the battery far faster than usual. If a tag keeps eating batteries, measure the sleep current. It should be around 2µA, or about 7µA on a long term average including check-ins. Any higher may be indicative of another issue.

## EPD Failure ##
* Simple failure

Simply put, you'll know if this is the case when an EPD refuses to update. It can sometimes be more subtle, like increased quiescent current, but the far most common failure mode is an entirely dead EPD, combined with a wildly higher current draw (about 100mA)

* Blurry screen

A problem seen mostly on the 1.54" displays; sometimes a screen becomes blurry, and will lack the sharpness to display any thin lines.
<img width="300" alt="blurry" src="https://user-images.githubusercontent.com/2544995/232600436-cdf12ca8-2d73-46b1-90be-24ae670d1bf9.png">

* Burn-in

When exposed to a lot of light, sometimes the the pigments will 'burn' in place, leaving a 'ghost'-image in place that will not easily go away. 

* Shifted or distorted images

<img width="300" alt="distorted" src="https://user-images.githubusercontent.com/2544995/232601619-af98ff12-a784-4cc5-a962-65533282a1c6.png"><img width="300" alt="shifted" src="https://user-images.githubusercontent.com/2544995/232601832-432ccfeb-39c7-4f70-aaf7-8beb156f03cb.png">
Sometimes the EPD has problems keeping track of incoming bits, or has a problem with its X/Y memory address pointer. Most of the times, this is transient, but some panels are more susceptible than others. Most of the time it's just a small shift; usually limited to 8 pixels. In that case, the EPD lost track of an entire byte

* Other failures

In some cases, the EPD will load wrong LUT's, have a grey or reddish color cast over it, have missing lines. There are a lot of ways in which a bad EPD will manifest itself.

## EPD voltage pump failure ##
The EPD uses an external inductor and mosfet to generate the voltages needed for the screen updates. If this fails, expect one or more of the following:
* A warm inductor / mosfet
* Dead EPD
* Entirely black EPD
* High current draw

<img width="300" alt="blackepd" src="https://user-images.githubusercontent.com/2544995/232597068-07a993e4-69a4-4f68-b59c-b99527995b9d.png">

These EPD's can be used on another board, and will generally work perfect again! It's a failure of an external component located on the PCB

## SPI EEPROM failure ##
All but the segmented tags have on-board SPI EEPROM's that can fail. Generally, if the EEPROM fails in a way that can be detected by OpenEPaperLink, the firmware will let you know with a warning on-screen. If the EEPROM fails in a less dramatic way, it can sometimes read back data from wrong sectors. This will show as shifted or corrupted images sent to the screen (the firmware splash-screen should show correctly).

Additionally, it can sometimes add a higher-than-normal drain on the battery, depleting it rapidly.

## SOC/Flash failure ##
In rare cases, the SOC can reject all flasher-commands, preventing the tag from getting flashed externally. OTA updates sometimes still work. A problem with the EPD driver can also present itself as a failure to update/flash the SOC; as the SPI lines are shared, a malfunctioning EPD can sometimes cause problems with external flashers.
