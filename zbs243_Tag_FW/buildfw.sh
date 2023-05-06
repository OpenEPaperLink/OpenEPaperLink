#!/bin/bash
make clean > /dev/null

echo -e "\nBuilding 1.54 version..."
make BUILD=zbs154v033 CPU=8051 SOC=zbs243 > /dev/null
mv main.bin ../binaries/Tag_FW_1.54.bin -v
make clean > /dev/null

echo -e "\n\nBuilding 2.9 (SSD1619) version..."
make BUILD=zbs29v033 CPU=8051 SOC=zbs243 > /dev/null
mv main.bin ../binaries/Tag_FW_2.9.bin -v
make clean > /dev/null

echo -e "\n\nBuilding 4.2 version..."
make BUILD=zbs42v033 CPU=8051 SOC=zbs243 > /dev/null
mv main.bin ../binaries/Tag_FW_4.2.bin -v
make clean > /dev/null

echo -e "\n\nBuilding 2.9 (UC8151) version..."
make BUILD=zbs29_uc8151 CPU=8051 SOC=zbs243 > /dev/null
mv main.bin ../binaries/Tag_FW_2.9-uc8151.bin -v
make clean > /dev/null

php packagebinaries.php
