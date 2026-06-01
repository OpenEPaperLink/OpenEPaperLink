#!/usr/bin/env bash
#
# Build the ESP32-C6 AP firmware. Shell port of compile.ps1.
#
# Sources the ESP-IDF environment and runs a clean build. Expects ESP-IDF in
# the default ~/esp/esp-idf location (as compile.ps1 does); set IDF_EXPORT to
# point at a different export.sh.

IDF_EXPORT="${IDF_EXPORT:-$HOME/esp/esp-idf/export.sh}"

cd "$(dirname "$0")" || exit 1

# shellcheck source=/dev/null
. "$IDF_EXPORT"

idf.py fullclean
idf.py build
