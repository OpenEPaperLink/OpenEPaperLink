#!/bin/bash
make clean

make BUILD=zbs154v033 CPU=8051 SOC=zbs243
mv main.bin ../binaries/Tag_FW_1.54.bin
make clean

make BUILD=zbs29v033 CPU=8051 SOC=zbs243
mv main.bin ../binaries/Tag_FW_2.9.bin
make clean

make BUILD=zbs42v033 CPU=8051 SOC=zbs243
mv main.bin ../binaries/Tag_FW_4.2.bin
make clean

make BUILD=zbs29_uc8151 CPU=8051 SOC=zbs243
mv main.bin ../binaries/Tag_FW_2.9-uc8151.bin
make clean
