# Getting started with the OpenEPaperLink and the Mini-AP #

So you've made or bought yourself an AP and a few tags! Cool! Here's how to get started with OpenEPaperLink. We'll explain the hardware features of the Mini-AP, if you built one yourself, you'll probably be familiar enough with that hardware. This guide expects a fully flashed AP, both the ESP32 and the AP-tag inside. Also, this only works with tags running OpenEPaperLink firmware. If this is not the case, this guide will be a less-than-awesome place to start setting up OpenEPaperLink

## Hardware ##
<img width="500" src="https://github.com/jjwbruijn/OpenEPaperLink/assets/2544995/be3c25dd-6232-43d4-acce-bc87dbdadd5c">

That is all!

Now I can hear you think: 'Geewhizz mister, you're lazy!', and you're not wrong... Okay fine I'll explain a little further.

This thing:
- is USB-C powered (5V, about 120mA or 0.6 watt)
- Powered by an ESP32-S2 (with PSRAM)
- Uses an upcycled Solum pricetag as display and radio
- Is actually multi-CPU! Neato!
- Has a WS2812B RGB Led (for the haters: don't worry, you can disable it easily)
- Has two hardware switches; one to enter usb-download mode, (GPIO 0) and one to reset the ESP32. You can reach them with a paperclip
- Uses an E-Paper display for telling you some Accesspoint status information. Who doesn't like E-Paper!

The MiniAP uses an ESP32 microcontroller and pricetag to work as an accesspoint for OpenEPaperLink displays that support 802.15.4 (Zigbee-esque) packets.

## Software ##

This thing runs software from this repo:
- ESP32_AP-Flasher firmware on the ESP32
- zbs243_AP_FW on the segmented tag-AP

The AP can be accessed with a nice web-interface that lets you select data for the tags to display. The ESP32 connects to your network through WiFi.

## AP Setup ##
Alright, let's get this show on the road. We'll start with the AP first, and then connect some tags to it in the next part.
* Connect your AP to a power source. This can be a computer, of course, but you can also use any 5V wall-wart you have laying around. The power consumption isn't incredibly high.
* The tag will start, and the RGB led should blink blue-green-blue. This indicates it is waiting for a wifi connection
* You'll need to set up the WiFi connection for the AP. The ESP32 inside uses the awesome WiFiManager project to set up an wifi-accesspoint that you can use to easily configure wifi
* Use your computer, phone, tablet, whatever, and connect to the 'OpenEPaperLink Setup' WiFi Network.

<img width="200" src="https://github.com/jjwbruijn/OpenEPaperLink/assets/2544995/01c2a4df-fbd8-45f2-8f56-ded9c83fed11">

* Usually, a browser will pop-up, and presents you a 'captive portal'. If it doesn't, simpy remain connected to the OpenEPaperLink wifi network and point your browser to http://192.168.4.1

<img width="200" src="https://github.com/jjwbruijn/OpenEPaperLink/assets/2544995/46f2ce2a-797e-4b6a-ad66-f76ef64d66f7">

* Click or tap on 'Configure Wifi' - this will start a scan for WiFi networks
* You should get a list of WiFi networks the AP can receive. Click on your WiFi network, enter your password, and hit 'save'.

<img width="200" src="https://github.com/jjwbruijn/OpenEPaperLink/assets/2544995/b32ad10c-ccb7-4319-8559-30cacaea4564">

* After the AP has been successfully connected to the network, the LED on the back should be green, and will fade in and out.
* The 802.15.4/ZigBee-ish radio will now start. You can see some stuff appear on the ePaper screen
* After the 'Boot'-message, the screen will show you the AP's IP address

<img width="200" src="https://github.com/jjwbruijn/OpenEPaperLink/assets/2544995/5039bc77-0d74-435e-b7a5-2f7d9634b652">

* Due to the limited characters, the screen will show the IP address in two parts. Remember this IP address, you're going to need it...
* ... here. Connect your device back to the same network as you've just connected the AP to, and point your browser to the IP address. Forgot it already? It happens. Simply restart your AP, and it'll show the IP address again.
* You should see an awesome web interface! If you want to kill the RGB Led, now is the first convenient moment to do it. Hit up 'AP config' and check out the config there. All kinds of cool features!
* If you're aware of a zigbee network at home, you can select a different channel in the AP config. As most channels have some kind of overlap with WiFi, we've only made some channels available with little or no overlap. Now would also be a good time to select a channel if you change it, because if you change it with tags already associated, it may take them up to an hour-ish to reconnect to the new channel.

## Adding Tags ##
* Take a (programmed) tag, and insert the batteries. This may sound easier than it is, in reality. See, these tags are cheap. They don't reset automatically when the voltage drops; if you replace batteries, you'll sometimes need to reset them (by shortening out the contacts), and you want to insert the batteries in a swift motion, so that they make contact and stay connected. You can use the battery cover to pop the batteries in, as shown in the video below.

https://github.com/jjwbruijn/OpenEPaperLink/assets/2544995/e4c693f8-b018-4b83-94df-399dc285618a


* If you want to make sure you'll reset the tag properly, the easiest way to drain the internal capacitors is to shorten the them using a battery. A battery inserted in reverse will shorten the contacts. On tags with multiple batteries: don't keep batteries in the bay with one battery shorting out the contacts; they're wired in parallel.
* Now that you've successfully powered on your tag, it's time to see if it's showing up on the AP-webinterface. A few seconds after the 'Waiting for data...' screen is shown on a tag, it should show up on the accesspoint.
* Select the tag in the webinterface, choose some content, and the tag should update the next time it checks in!

## Signal strength ##
The AP has a range of up to 25 meters line of sight, but since there is some overlap with WiFi channels, the coexistence of WiFi can reduce the system's range. Also, don't expect these signals to penetrate concrete (rebar) floors or walls very well.
  
## Further reading ##
* [Troubleshooting tags](https://github.com/jjwbruijn/OpenEPaperLink/blob/master/Tags-specs/troubleshooting.md)
* [How to build a Mini AP](https://github.com/jjwbruijn/OpenEPaperLink/tree/master/Hardware/OpenEPaperLink%20Mini%20AP/README.md)
