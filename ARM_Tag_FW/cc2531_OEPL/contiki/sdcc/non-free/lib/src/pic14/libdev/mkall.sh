#!/bin/sh

# This script is supposed to recreate all device libraries and their
# accompanying header files from the gputils' header/*.inc files
# using sdcc/support/scripts/inc2h.pl.
#
# The devices to be created are listed in ./devices.txt, the output
# files will be located in ./build/, which is created before use.
#
# (c) 2007 by Raphael Neider <rneider @ web.de>
# This file is published under the terms of the GPLv2.

GPUTILS=$HOME/gputils.svn
SDCC=$HOME/sdcc/sdcc.svn

HEADERS=$SDCC/device/non-free/include/pic14

if [ ! -d "$GPUTILS" ]; then
    echo "gputils not found in '$GPUTILS' -- exiting.";
    exit 1;
fi;

if [ ! -d "$SDCC" ]; then
    echo "sdcc not found in '$SDCC' -- exiting.";
    exit 1;
fi;

mkdir build;
cd build;
cp ../pic14ports.txt .

if true; then
    sed -e 's/\s*#.*$//' ../devices.txt | grep -v '^\s*$' | while read PROC; do
        echo "### Generating files for $PROC ...";
        EMIT_LEGACY_NAMES=0;
        if grep -q 'NO_LEGACY_NAMES' "$HEADERS/pic${PROC}.h" >/dev/null 2>&1; then
            EMIT_LEGACY_NAMES=1;
        fi;
        $SDCC/support/scripts/inc2h.pl $PROC $GPUTILS $EMIT_LEGACY_NAMES;
    done;
fi;

for i in *.c; do
    newc=1;
    newh=1;
    differs=0;
    h="${i%.c}.h";
    echo "";
    echo "### Checking $i and $h ...";
    if [ -f "../$i" ]; then
        newc=0;
        if diff -wup "../$i" "$i"; then
            # identical
            :
        else
            differs=1;
        fi;
    fi;
    if [ -f "$HEADERS/$h" ]; then
        newh=0;
        if diff -wup "$HEADERS/$h" "$h"; then
            # identical
            :
        else
            differs=1;
        fi;
    fi;
    if [ "x$newc$newh" = "x11" ]; then
        # new device -- copy into place
        echo "Installing new files ...";
        mv -f "$i" ..;
        mv -f "$h" "$HEADERS/$h";
    elif [ "x$differs" = "x0" ]; then
        # identical files -- ignore
        echo "No change -- keeping files ...";
        rm -f "$i" "$h";
    else
        ok=no;
        while [ ! xyes = "x$ok" ]; do
            echo "Replace? [y/n]";
            read -n1 ans;
            case "$ans" in
                y|Y)
                    echo "Replacing ...";
                    mv -f "$i" ..;
                    mv -f "$h" "$HEADERS/$h";
                    ok=yes;
                    ;;
                n|N)
                    echo "Keeping ...";
                    rm -f "$i" "$h";
                    ok=yes;
                    ;;
                *)
                    ok=no;
                    ;;
            esac;
        done;
    fi;
done;

cd ..;
