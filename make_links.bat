@echo off

REM TAG_FW
rmdir zbs243_TAG_FW\board
rmdir zbs243_TAG_FW\cpu
rmdir zbs243_TAG_FW\soc

rm zbs243_TAG_FW\asmUtil.h
rm zbs243_TAG_FW\barcode.c
rm zbs243_TAG_FW\barcode.h
rm zbs243_TAG_FW\bitmaps.h
rm zbs243_TAG_FW\eeprom.c
rm zbs243_TAG_FW\eeprom.h
rm zbs243_TAG_FW\font.h
rm zbs243_TAG_FW\lut.h
rm zbs243_TAG_FW\printf.h
rm zbs243_TAG_FW\proto.h
rm zbs243_TAG_FW\sleep.h
rm zbs243_TAG_FW\wdt.h

mklink /J zbs243_TAG_FW\board zbs243_shared\board
mklink /J zbs243_TAG_FW\cpu zbs243_shared\cpu
mklink /J zbs243_TAG_FW\soc zbs243_shared\soc

mklink /H zbs243_TAG_FW\asmUtil.h zbs243_shared\asmUtil.h
mklink /H zbs243_TAG_FW\barcode.c zbs243_shared\barcode.c
mklink /H zbs243_TAG_FW\barcode.h zbs243_shared\barcode.h
mklink /H zbs243_TAG_FW\bitmaps.h zbs243_shared\bitmaps.h
mklink /H zbs243_TAG_FW\eeprom.c zbs243_shared\eeprom.c
mklink /H zbs243_TAG_FW\eeprom.h zbs243_shared\eeprom.h
mklink /H zbs243_TAG_FW\font.h zbs243_shared\font.h
mklink /H zbs243_TAG_FW\lut.h zbs243_shared\lut.h
mklink /H zbs243_TAG_FW\printf.h zbs243_shared\printf.h
mklink /H zbs243_TAG_FW\proto.h zbs243_shared\proto.h
mklink /H zbs243_TAG_FW\sleep.h zbs243_shared\sleep.h
mklink /H zbs243_TAG_FW\wdt.h zbs243_shared\wdt.h

REM AP_FW
rmdir zbs243_AP_FW\board
rmdir zbs243_AP_FW\cpu
rmdir zbs243_AP_FW\soc

rm zbs243_AP_FW\adc.h
rm zbs243_AP_FW\asmUtil.h
rm zbs243_AP_FW\barcode.c
rm zbs243_AP_FW\barcode.h
rm zbs243_AP_FW\font.h
rm zbs243_AP_FW\lut.h
rm zbs243_AP_FW\printf.h
rm zbs243_AP_FW\proto.h
rm zbs243_AP_FW\sleep.h
rm zbs243_AP_FW\wdt.h

mklink /J zbs243_AP_FW\board zbs243_shared\board
mklink /J zbs243_AP_FW\cpu zbs243_shared\cpu
mklink /J zbs243_AP_FW\soc zbs243_shared\soc

mklink /H zbs243_AP_FW\adc.h zbs243_shared\adc.h
mklink /H zbs243_AP_FW\asmUtil.h zbs243_shared\asmUtil.h
mklink /H zbs243_AP_FW\barcode.c zbs243_shared\barcode.c
mklink /H zbs243_AP_FW\barcode.h zbs243_shared\barcode.h
mklink /H zbs243_AP_FW\font.h zbs243_shared\font.h
mklink /H zbs243_AP_FW\lut.h zbs243_shared\lut.h
mklink /H zbs243_AP_FW\printf.h zbs243_shared\printf.h
mklink /H zbs243_AP_FW\proto.h zbs243_shared\proto.h
mklink /H zbs243_AP_FW\sleep.h zbs243_shared\sleep.h
mklink /H zbs243_AP_FW\wdt.h zbs243_shared\wdt.h