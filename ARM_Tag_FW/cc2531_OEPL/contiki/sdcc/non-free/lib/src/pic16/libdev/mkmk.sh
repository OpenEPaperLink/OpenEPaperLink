#!/bin/sh

#
# Consider using running 'sh update.sh' in '..' instead to
# also update the automake conditionals ENABLE_xxx!
#
# Script to generate Makefile.am,
# execute in device/lib/pic16/libdev/ with no arguments.
#
# For this to work, sdcc from PATH must already use the latest
# pic16fdevices.txt including to-be-added devices!
#
# Written by Raphael Neider <tecodev AT users sourceforge net>
#
# Released under the terms of the GPL v2.
#

cat <<HERE
## Makefile.am -- Process this file with automake to produce Makefile.in
## This file has been automatically generated using $0.

lib_LIBRARIES =

HERE

for f in ./pic1*.c; do
    p="${f##*/pic}";
    p="${p%.c}";
    arch="$p";
    ARCH=$(echo "$arch" | tr abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ);
    cat <<HERE
if ENABLE_${ARCH}
lib_LIBRARIES += libdev${arch}.a
endif ENABLE_${ARCH}
libdev${arch}_a_SOURCES = pic${arch}.c
HERE


    sdcc -mpic16 -p${arch} --nostdinc -I../../../../include/pic16 -I../../../include/pic16 -o pic${arch}.inc -E pic${arch}.c;
    if grep '\bEEADRH\b' pic${arch}.inc >/dev/null 2>&1; then
        # 16 bit EEPROM address space
        for i in get put; do
            for j in 1 2 3 4; do
                echo "libdev${arch}_a_SOURCES += gptr/eeprom16_gptr${i}${j}_dispatch.S";
            done;
        done;
    elif grep '\bEEADR\b' pic${arch}.inc >/dev/null 2>&1; then
        # 8 bit EEPROM address space
        for i in get put; do
            for j in 1 2 3 4; do
                echo "libdev${arch}_a_SOURCES += gptr/eeprom8_gptr${i}${j}_dispatch.S";
            done;
        done;
    else
        # No EEPROM
        echo "libdev${arch}_a_SOURCES += gptr/dispatch.S";
    fi;

    if ! grep '\bSSPBUF\b' pic${arch}.inc >/dev/null 2>&1; then
        echo "libdev${arch}_a_SOURCES += fake_sspbuf.c";
    fi;
    rm pic${arch}.inc;

    echo "libdev${arch}_a_CFLAGS = -p${arch} \$(AM_CFLAGS)";
    echo "";
done;

cat <<HERE
include \$(top_srcdir)/Makefile.common

HERE

