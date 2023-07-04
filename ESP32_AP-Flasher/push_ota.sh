#!/bin/bash

source updateRemote.sh > /dev/null

if [ -z "$IP" ]
  then
    echo "ERROR: Empty IP variable"
    exit 1
fi

if [ -z "$PIOENV" ]
  then
    echo "ERROR: Empty PIOENV variable"
    exit 1
fi

md5sum .pio/build/$PIOENV/firmware.bin | tee .pio/build/$PIOENV/firmware.md5
upload_file .pio/build/$PIOENV/firmware.md5:ota_md5.txt
upload_file .pio/build/$PIOENV/firmware.bin:ota.bin
