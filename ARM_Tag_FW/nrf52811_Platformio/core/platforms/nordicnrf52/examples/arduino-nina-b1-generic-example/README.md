# Arduino-u-blox-NINA-B1

u-blox NINA-B1 Arduino example + SHT31 temperature and humidity + Bluetooth BLE

[![GitHub version](https://img.shields.io/github/release/ldab/Arduino-u-blox-NINA-B1.svg)](https://github.com/ldab/Arduino-u-blox-NINA-B1/releases/latest)
[![Build Status](https://travis-ci.org/ldab/Arduino-u-blox-NINA-B1.svg?branch=master)](https://travis-ci.org/ldab/Arduino-u-blox-NINA-B1)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](https://github.com/ldab/Arduino-u-blox-NINA-B1/blob/master/LICENSE)

[![GitHub last commit](https://img.shields.io/github/last-commit/ldab/Arduino-u-blox-NINA-B1.svg?style=social)](https://github.com/ldab/Arduino-u-blox-NINA-B1)

[![EVK-NINA-B1](https://www.u-blox.com/sites/default/files/styles/product_full/public/products/EVK-NINA-B1_2D-medium_RGB-CI.png)](https://www.u-blox.com/en/product/nina-b1-series)

## How to build PlatformIO based project

1. [Install PlatformIO Core](https://docs.platformio.org/page/core.html)
2. Download [development platform with examples](https://github.com/platformio/platform-nordicnrf52/archive/develop.zip)
3. Extract ZIP archive
4. Run these commands:

```
# Change directory to example
> cd platform-nordicnrf52/examples/Arduino-u-blox-NINA-B1

# Build project
> platformio run

# Upload firmware
> platformio run --target upload

# Build specific environment
> platformio run -e nina_b1

# Upload firmware for the specific environment
> platformio run -e nina_b1 --target upload

# Clean build files
> platformio run --target clean
```

## Why?

This example tries to implement some key functions and key PIN atributes in order to get you started with using Arduino and the NRF52832 BLE board u-blox NINA-B1.

Timer functionas are implemented intead of `delay()` and the PINs have been re-mapped on the `#define` section

## Bluetooth iOS and Android app

You can download the sample Bluetooth low energy app - BLE App straight from u-blox wesite: [https://www.u-blox.com/en/product/bluetooth-ios-and-android-app](https://www.u-blox.com/en/product/bluetooth-ios-and-android-app)

![App example](https://raw.githubusercontent.com/ldab/Arduino-u-blox-NINA-B1/master/extras/Screenshot_20190328-130832_u-blox%20BLE.jpg)

## fatal error: ble_gatts.h: No such file or directory

The arduino-nRF5x core **REQUIRES** a SoftDevice in order to successfully use this library. Please see [Flashing a SoftDevice](https://github.com/sandeepmistry/arduino-nRF5#selecting-a-softdevice).

On PlatformIO and when using this example you don't need to do anything special as the `build_flags = -DNRF52_S132` has already been included on `platform.ini` file.

SoftDevices contain the BLE stack and housekeeping, and must be downloaded once before a sketch using BLE can be loaded. The SD consumes ~5k of Ram + some extra based on actual BLE configuration.

* SoftDevice S132 v2.0.1 supports nRF52 in peripheral and central role. It is 112k in size.

## Credits

Github Shields and Badges created with [Shields.io](https://github.com/badges/shields/)

Sandeep Mistry's [Arduino BLE library](https://github.com/sandeepmistry/arduino-BLEPeripheral)

Adafruit [SHT31 Library](https://www.adafruit.com/product/2857)

u-blox NINA-B1 blueprint example [GitHub](https://github.com/u-blox/blueprint-B200-NINA-B1)

u-blox official mBed [library](https://os.mbed.com/platforms/u-blox-EVK-NINA-B1/)
