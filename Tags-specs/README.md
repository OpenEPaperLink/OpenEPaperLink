# Pinouts

## Segmented display (UK version, at least)
<img width="600" alt="board" src="https://user-images.githubusercontent.com/2544995/227795256-bb5df6d6-abe5-4b94-9b4d-4931053de64a.png">

## 2.9" / 1.54" Displays
<img width="600" alt="board" src="https://user-images.githubusercontent.com/2544995/227795403-c36d61d7-41db-4799-b647-cb9081543b61.png">

## 4.2" Displays (ZBS version)
<img width="600" alt="board" src="https://user-images.githubusercontent.com/2544995/227795476-cc0529b2-9f2a-4cd9-b3c5-3fef7683719c.png">

# Tech specs
## 4.2”
* ZBS 243 / SEM9110 8051 based core, 64kbyte flash, 8kbyte XRAM, 256 bytes iRAM 16 MHz, 802.15.4 2.4Ghz radio
* 4.2” e-paper display (BWR) 400x300 SSD1619
* 1Mbyte SPI flash
* 2x CR2540 button cell
* Option: NXP NFC controller (I2C)

## 2.9”

* ZBS 243 / SEM9110 8051 based core, 64kbyte flash, 8kbyte XRAM, 256 bytes iRAM 16 MHz, 802.15.4 2.4Ghz radio
* 2.9” e-paper display (BWR) 296x128 SSD1619 or UC8151
* 1Mbyte SPI flash
* 2x CR2540 button cell
* Option: NXP NFC controller (I2C)

## 1.54”
* ZBS 243 / SEM9110 8051 based core, 64kbyte flash, 8kbyte XRAM, 256 bytes iRAM 16 MHz, 802.15.4 2.4Ghz radio
* 1.54” e-paper display (BWR) 152x152 SSD1619
* 1Mbyte SPI flash
* 1x CR2540 button cell
* Option: NXP NFC controller (I2C)


# ZBS 243 SoC (SEM9110)
<img width="400" alt="board" src="https://user-images.githubusercontent.com/2544995/227795788-249fe764-816e-48da-9932-0b70dad87f47.png">
* 8051 based core
* 64kbyte flash
* 8kbyte XRAM
* 256 bytes IRAM
* 16Mhz clock
* +- 1µA Sleep
* SPI / UART / I²C / RF Wake (undocumented) / 802.15.4 Radio / 
Dmitry Grinberg [reverse engineered](https://dmitry.gr/?r=05.Projects&proj=30.%20Reverse%20Engineering%20an%20Unknown%20Microcontroller) almost all parts of this SoC! This project uses his SDK and some of his, and [ATC1441's](https://github.com/atc1441/ZBS_Flasher/)  Some features, such as RF wake and the battery voltage reading are as of now, undocumented

