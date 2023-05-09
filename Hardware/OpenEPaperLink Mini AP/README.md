# OpenEPaperLink Mini AP #

<img width="500" src="https://github.com/jjwbruijn/OpenEPaperLink/assets/2544995/708e9800-9e6f-472c-b933-7818d78b0cfc">

This is a very minimalistic accesspoint; in the most basic version it is exactly a tiny ESP32, a 'segmented-display' tag, and a little flex PCB. It can be used as an accesspoint for OpenEPaperLink. The small size makes it also very useful as an extra accesspoint, using OpenEPaperLink's Multi-AP feature

## Parts ##
<img width="300" src="https://github.com/jjwbruijn/OpenEPaperLink/assets/2544995/2b775298-45d8-40f1-80ae-20100630d8dd">

* Wemos S2 Mini (or clone)
* Solum Segmented pricetag
* OpenEPaperLink Mini AP Flex PCB

While it can't hurt to add the following optional parts, they're *not* needed
* 3D-printed case
* Optional: 1206-sized SMD capacitor
* Optional: WS2812B LED

While the Wemos S3 mini *should* be compatible with an updated pindefinitions, this has not been tested (yet)

## Assembly ##

<img width="300" src="https://github.com/jjwbruijn/OpenEPaperLink/assets/2544995/abb287bf-c5ab-493a-8a0b-be9cd0f94c2b">

1. A good place to start is to add the optional parts first; the optional extra capacitor and WS2812B LED
The flex PCB (depending on the board house) can be very thin indeed, don't let your soldering iron dwell on the pads any longer than necessary.
 
 

<img width="300" src="https://github.com/jjwbruijn/OpenEPaperLink/assets/2544995/62579c1e-b782-4cb5-831a-97799af83e0a">

2. Add a little bit of solder on each of the debug pads on the tag. This makes it a little easier to solder the flex PCB later on



<img width="300" src="https://github.com/jjwbruijn/OpenEPaperLink/assets/2544995/f32f1c96-07e0-416e-8599-9d3f0ea6e998">

3. Place the Wemos S2 mini on the flex PCB, on a somewhat heat-resistant surface. Make sure to align the hole properly, and apply some pressure on the S2 module to make sure there is minimal distance between the flex PCB and the module. Fill a few holes with solder, taking care to not heat it any longer than required for the solder to properly flow. Not all pads need to be soldered; the top 3 rows of pins can be left unsoldered; this makes it easier to put the module into the case later on. 



<img width="300" src="https://github.com/jjwbruijn/OpenEPaperLink/assets/2544995/0a7fa1f1-89ba-45b4-ba78-bbbd48984eef">

4. Turn the PCB/Module around, and make sure all the pads are soldered on the other side. Again, as can be seen in the picture, the top 3 rows can be left unsoldered


<img width="300" src="https://github.com/jjwbruijn/OpenEPaperLink/assets/2544995/66c60566-85a2-4ab2-8723-4ee1b26128c0">

5. Align the tag-pads with the debug header on the Segmented tag, and solder it on.

6. You're basically done! This combination isn't really sturdy though, its best mounted in a small 3D-printed enclosure. 

<img width="300" src="https://github.com/jjwbruijn/OpenEPaperLink/assets/2544995/03fc95c5-d8e8-4f10-9488-5d7d9faf78b7">
<img width="300" src="https://github.com/jjwbruijn/OpenEPaperLink/assets/2544995/26c9f6a2-8858-4fda-ba1b-28a3bdf4e17c">


7. Start by inserting the Wemos module into the lower part of the case. It should snap in place; depending on the tolerances of your printer, you might need to make small adjustments with a sharp knife.

<img width="300" src="https://github.com/jjwbruijn/OpenEPaperLink/assets/2544995/489fc951-1875-41c8-93bc-54981777b184">

8. Optionally, you can add some small screws, but the fit should be tight enough for the Wemos module not to move around (too) much.

<img width="300" src="https://github.com/jjwbruijn/OpenEPaperLink/assets/2544995/5f54a0b0-6262-46f0-8e26-b615df0e53f0">

9. Flip the tag over, and insert it into the top half of the case. Again, the fit should be pretty tight. After inserting the tag, add some cyanoacrylate glue to the edges to fix it in place.

10. After you've given the glue enough time to cure, glue the two halves of the case together. Use some clamps to make sure all the edges are nice and clean

## Programming ##

* Use PlatformIO to program the ESP32, using the 'Mini AP' build option.
* Use the 'Upload filesystem image' option to prepare the ESP32's filesystem. Crucially, the tag_db_md5.json and AP_FW_Pack.bin need to be on the ESP32 to allow it to update the tag to a new version
