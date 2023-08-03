#!/bin/sh

#
# Script to generate Makefile.am,
# execute in device/lib/pic16/libio/ with no arguments.
#
# Written by Raphael Neider <tecodev AT users sourceforge net>
#
# Released under the terms of the GPL v2.
#

cat <<HEREDOC
## Makefile.am -- Process this file with automake to produce Makefile.in
## This file was automatically generated using $0.

lib_LIBRARIES = 

HEREDOC

for f in "../../../non-free/lib/pic16/libdev/pic1"*.c; do
    p="${f##*/pic}";
    p="${p%.c}";
    P=$(echo "$p" | tr abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ);
    echo "Generating for device >>${p}<<" >&2;
    cat <<HERE
if ENABLE_$P
lib_LIBRARIES += libio${p}.a
endif ENABLE_$P
libio${p}_a_SOURCES = dummy.c
HERE
    sed -e 's/\s*\(#.*\)\{0,1\}$//' adc.ignore | grep -x "$p" > /dev/null 2>&1 && { echo "No adc." >&2 ; } || cat <<HERE
libio${p}_a_SOURCES += adc/adcbusy.c adc/adcclose.c adc/adcconv.c
libio${p}_a_SOURCES += adc/adcopen.c adc/adcread.c adc/adcsetch.c
HERE
    sed -e 's/\s*\(#.*\)\{0,1\}$//' i2c.ignore | grep -x "$p" > /dev/null 2>&1 && { echo "No i2c." >&2 ; } || cat <<HERE
libio${p}_a_SOURCES += i2c/i2cack.c i2c/i2cclose.c i2c/i2cdrdy.c
libio${p}_a_SOURCES += i2c/i2cidle.c i2c/i2cnack.c i2c/i2copen.c
libio${p}_a_SOURCES += i2c/i2creadc.c i2c/i2creads.c i2c/i2crestart.c
libio${p}_a_SOURCES += i2c/i2cstart.c i2c/i2cstop.c i2c/i2cwritec.c
libio${p}_a_SOURCES += i2c/i2cwrites.c
HERE
    sed -e 's/\s*\(#.*\)\{0,1\}$//' usart.ignore | grep -x "$p" > /dev/null 2>&1 && { echo "No usart." >&2 ; } || cat <<HERE
libio${p}_a_SOURCES += usart/ubaud.c usart/ubusy.c usart/uclose.c
libio${p}_a_SOURCES += usart/udrdy.c usart/ugetc.c usart/ugets.c
libio${p}_a_SOURCES += usart/uopen.c usart/uputc.c usart/uputs.c
libio${p}_a_SOURCES += usart/usartd.c
HERE
    cat <<HERE
libio${p}_a_CFLAGS = -p${p} \$(AM_CFLAGS)

HERE
done

cat <<HEREDOC
include \$(top_srcdir)/Makefile.common

HEREDOC

