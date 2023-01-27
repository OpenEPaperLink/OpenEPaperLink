@echo off
makeit clean
makeit
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

