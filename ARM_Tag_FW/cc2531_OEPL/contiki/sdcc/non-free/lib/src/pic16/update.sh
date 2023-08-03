#!/bin/sh

#
# Run this script every time the set of supported devices changes.
#

LIB=".";
OUTPUT="$LIB/processors.ac";

# Update configure script.
echo "### Generating configure script for AM_CONDITIONALs ...";
rm -f "$OUTPUT";
for i in "$LIB/libdev/pic1"*.c; do
    p="${i##*/pic}";
    p="${p%.c}";
    P=$(echo "$p" | tr abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ);
    cat >> "$OUTPUT" <<EOT
AM_CONDITIONAL([ENABLE_${P}], [ grep -i "^${P}\$" pics.supported >/dev/null 2>/dev/null ])
EOT
done;

# Update libdev/Makefile.am.
echo "### Updating libdev/Makefile.am ...";
( cd "$LIB/libdev" && sh ./mkmk.sh > Makefile.am )

# Recreate configure script.
echo "### Updating build system ..."
( cd "$LIB" && autoreconf )

# Also update the free library part.
echo "### Updating dependent build system in ../../../lib/pic16 ..."
( cd "$LIB/../../../lib/pic16/libio" && sh ./mkmk.sh > Makefile.am )
( cd "$LIB/../../../lib/pic16" && autoreconf )
