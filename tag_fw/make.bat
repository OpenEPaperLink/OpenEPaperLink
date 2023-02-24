@echo off
del fw154.bin
del fw29.bin
del fw42.bin
makeit clean
makeit BUILD=zbs154v033 CPU=8051 SOC=zbs243
pause
ren main.bin fw154_%date:~10,4%%date:~4,2%%date:~7,2%.bin
makeit clean
makeit BUILD=zbs29v033 CPU=8051 SOC=zbs243
pause
ren main.bin fw29_%date:~10,4%%date:~4,2%%date:~7,2%.bin
makeit clean
makeit BUILD=zbs42v033 CPU=8051 SOC=zbs243
pause
ren main.bin fw42_%date:~10,4%%date:~4,2%%date:~7,2%.bin

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

