
#| Function                           | ESP32H2 Pin           | ESP32S3 Pin   
#| :--------------------------------: | :-------------------: |  :------------------
#| Activate The BOOT Mode Of ESP32H2  | ESP32H2_IO9           | ESP32S3_IO33 
#| Reset ESP32H2                      | ESP32H2_Pin Number 8  | ESP32S3_IO34 
#| Uart                               | ESP32H2_TX_IO24       | ESP32S3_RX_IO48
#| Uart                               | ESP32H2_RX_IO23       | ESP32S3_TX_IO47

# cd ESP32_AP-Flasher

# export PLATFORMIO_BUILD_FLAGS="-D BUILD_VERSION=${{ github.ref_name }} -D SHA=$GITHUB_SHA"

python gzip_wwwfiles.py

&(Join-Path $env:USERPROFILE '\.platformio\penv\Scripts\pio') run --environment ESP32_S3_16_8_LILYGO_AP

&(Join-Path $env:USERPROFILE '\.platformio\penv\Scripts\pio') run --target buildfs --environment ESP32_S3_16_8_LILYGO_AP

# mkdir ESP32_S3_16_8_LILYGO_AP

copy "$env:USERPROFILE\.platformio\packages\framework-arduinoespressif32\tools\partitions\boot_app0.bin" ESP32_S3_16_8_LILYGO_AP\boot_app0.bin

copy .pio\build\ESP32_S3_16_8_LILYGO_AP\firmware.bin ESP32_S3_16_8_LILYGO_AP\firmware.bin

copy .pio\build\ESP32_S3_16_8_LILYGO_AP\bootloader.bin ESP32_S3_16_8_LILYGO_AP\bootloader.bin

copy .pio\build\ESP32_S3_16_8_LILYGO_AP\partitions.bin ESP32_S3_16_8_LILYGO_AP\partitions.bin

copy .pio\build\ESP32_S3_16_8_LILYGO_AP\littlefs.bin ESP32_S3_16_8_LILYGO_AP\littlefs.bin

copy ESP32_S3_16_8_LILYGO_AP\firmware.bin espbinaries\ESP32_S3_16_8_LILYGO_AP.bin

copy ESP32_S3_16_8_LILYGO_AP\merged-firmware.bin espbinaries\ESP32_S3_16_8_LILYGO_AP_full.bin

cd ESP32_S3_16_8_LILYGO_AP

#python -m esptool                          --chip esp32-s3 merge_bin -o merged-firmware.bin --flash_mode dio --flash_freq 80m --flash_size 16MB 0x0000 bootloader.bin 0x8000 partitions.bin 0xe000 boot_app0.bin 0x10000 firmware.bin 0x00910000 littlefs.bin

#python -m esptool -p COM12 -b 460800 --before default_reset --after hard_reset --chip esp32s3  write_flash --flash_mode dio --flash_size detect 0x0000 bootloader.bin 0x8000 partitions.bin 0xe000 boot_app0.bin 0x10000 firmware.bin 0x00910000 littlefs.bin

cd ..

#esptool.py v4.7.0
#Serial port COM12
#Connecting...
#Chip is ESP32-S3 (QFN56) (revision v0.2)
#Features: WiFi, BLE
#Crystal is 40MHz
#MAC: f4:12:fa:af:5b:9c
#Uploading stub...
#Running stub...
#Stub running...
#Changing baud rate to 460800
#Changed.
#Configuring flash size...
#Auto-detected Flash size: 16MB
#Flash will be erased from 0x00000000 to 0x00003fff...
#Flash will be erased from 0x00008000 to 0x00008fff...
#Flash will be erased from 0x0000e000 to 0x0000ffff...
#Flash will be erased from 0x00010000 to 0x00203fff...
#Flash will be erased from 0x00910000 to 0x00feffff...
#Compressed 15104 bytes to 10401...
#Wrote 15104 bytes (10401 compressed) at 0x00000000 in 0.3 seconds (effective 375.1 kbit/s)...
#Hash of data verified.
#Compressed 3072 bytes to 146...
#Wrote 3072 bytes (146 compressed) at 0x00008000 in 0.1 seconds (effective 272.1 kbit/s)...
#Hash of data verified.
#Compressed 8192 bytes to 47...
#Wrote 8192 bytes (47 compressed) at 0x0000e000 in 0.1 seconds (effective 447.3 kbit/s)...
#Hash of data verified.
#Compressed 2047040 bytes to 1259376...
#Wrote 2047040 bytes (1259376 compressed) at 0x00010000 in 18.7 seconds (effective 876.9 kbit/s)...
#Hash of data verified.
#Compressed 7208960 bytes to 302805...
#Wrote 7208960 bytes (302805 compressed) at 0x00910000 in 47.0 seconds (effective 1227.2 kbit/s)...
#Hash of data verified.
