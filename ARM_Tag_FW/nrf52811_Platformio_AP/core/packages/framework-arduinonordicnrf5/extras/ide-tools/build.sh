#!/bin/sh -x

IDE_LIB_PATH=/Applications/Arduino.app/Contents/Java

rm -rf bin
mkdir -p bin
javac -target 1.8 -cp "$IDE_LIB_PATH/pde.jar:$IDE_LIB_PATH/arduino-core.jar" -d bin *.java && jar cvf nRF5FlashSoftDevice.jar -C bin .
