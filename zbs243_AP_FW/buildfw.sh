#!/bin/bash
make clean > /dev/null


echo -e "\nBuilding Segmented version..."
make BUILD=zbs_segmented_uk CPU=8051 SOC=zbs243 > /dev/null
mv main.bin ../binaries/AP_FW_Segmented_UK.bin -v
make clean > /dev/null

echo -e "\n\nBuilding No-display version..."
make BUILD=zbs_nodisplay CPU=8051 SOC=zbs243 > /dev/null
mv main.bin ../binaries/AP_FW_Nodisplay.bin -v
make clean > /dev/null

echo -e "\n\nBuilding 1.54 version..."
make BUILD=zbs154v033 CPU=8051 SOC=zbs243 > /dev/null
mv main.bin ../binaries/AP_FW_1.54.bin -v
make clean > /dev/null
