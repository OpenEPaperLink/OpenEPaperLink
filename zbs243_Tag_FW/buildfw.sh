#!/bin/bash
make clean > /dev/null

VER=$(grep "#define FW_VERSION" settings.h | grep -v "suffix" |  awk '{print $3}' | cut -c 3-)

echo "Building ZBS version ${VER}"

BINDIR="../binaries/Tag"

echo -e "\nBuilding 1.54 version..."
make BUILD=zbs154_ssd1619 CPU=8051 SOC=zbs243 > /dev/null
mv main.bin ${BINDIR}/SOL_M2_154_SSD_${VER}.bin -v
make clean > /dev/null

echo -e "\n\nBuilding 2.9 (SSD1619) version..."
make BUILD=zbs29_ssd1619 CPU=8051 SOC=zbs243 > /dev/null
mv main.bin ${BINDIR}/SOL_M2_29_SSD_${VER}.bin -v
make clean > /dev/null

echo -e "\n\nBuilding 4.2 version..."
make BUILD=zbs42_ssd1619 CPU=8051 SOC=zbs243 > /dev/null
mv main.bin ${BINDIR}/SOL_M2_42_SSD_${VER}.bin -v
make clean > /dev/null

echo -e "\n\nBuilding 2.9 (UC8151) version..."
make BUILD=zbs29_uc8151 CPU=8051 SOC=zbs243 > /dev/null
mv main.bin ${BINDIR}/SOL_M2_29_UC_${VER}.bin -v
make clean > /dev/null

echo -e "\n\nBuilding 2.9 BW (SSD1619 - freezer) version..."
make BUILD=zbs29_BW_ssd1619 CPU=8051 SOC=zbs243 > /dev/null
mv main.bin ${BINDIR}/SOL_M2_29_LT_${VER}.bin -v
make clean > /dev/null

