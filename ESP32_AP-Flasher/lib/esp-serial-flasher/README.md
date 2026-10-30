# esp-serial-flasher

Espressif's [esp-serial-flasher](https://github.com/espressif/esp-serial-flasher), version 1.11.0. The AP uses it to flash its ESP32-C6/H2 radio over UART (`src/espflasher.cpp`).

The source files are unchanged copies. Only what the AP needs is here: `include/`, `private_include/`, the UART sources in `src/` and the ESP32 port in `port/`. `library.json` replaces the CMake build: it selects those files and sets the defaults that CMake would set (`SERIAL_FLASHER_WRITE_BLOCK_RETRIES`, `SERIAL_FLASHER_RESET_INVERT`, `SERIAL_FLASHER_BOOT_INVERT`). The other options come from the build flags in `platformio.ini` (`SERIAL_FLASHER_INTERFACE_UART`, `MD5_ENABLED`, the hold times).

To update: copy the same files from the new release and check `CMakeLists.txt` there for new source files or options.

Version 2.x changed the API (a context argument on every call) and needs changes in `espflasher.cpp`.
