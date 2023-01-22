#!/bin/bash
make clean
make BUILD=zbs154v033 CPU=8051 SOC=zbs243
mv main.bin fw154.bin
make clean
make BUILD=zbs29v033 CPU=8051 SOC=zbs243
mv main.bin fw29.bin
make clean
make BUILD=zbs42v033 CPU=8051 SOC=zbs243
mv main.bin fw42.bin
make clean
