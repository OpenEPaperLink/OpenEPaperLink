@echo off
makeit clean
makeit
ren main.bin apfw_%date:~10,4%%date:~4,2%%date:~7,2%.bin
del /s *.asm
del /s *.lst
del /s *.rst
del /s *.sym
del /s *.map
del /s *.mem
del /s *.ihx
del /s *.adb
del /s *.rel
del /s *.omf

