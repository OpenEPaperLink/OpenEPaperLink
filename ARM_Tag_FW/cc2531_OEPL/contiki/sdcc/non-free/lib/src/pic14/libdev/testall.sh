#!/bin/sh

BASE="$(pwd)";
BUILD_LOG="$BASE/build.log";
BUILD_LOG2="$BASE/build.log2";
BUILD_DIR="$BASE/__tests";

CC="sdcc";
CPPFLAGS="";
CFLAGS="--use-non-free";
LDFLAGS="";
LIBS="";

cleanup()
{
  rm -f "$BUILD_LOG2";
  rm -rf "$BUILD_DIR";
}

if [ -e "$BUILD_DIR" ]; then
    echo "$BUILD_DIR already there -- exiting.";
    exit 1;
fi;
{ mkdir -p "$BUILD_DIR" && cd "$BUILD_DIR"; } || exit 1;
trap "cleanup" INT;

cat > sample.c <<EOT
#include <pic14regs.h>

void foo(char *c)
{
  if (*c)
    *c = 1;
}

void main()
{
  OPTION_REG = PCLATH;
  INTCON = 0;
}
EOT

sed -e 's/\s*#.*$//' ../devices.txt | grep -v '^\s*$' | while read PROC; do
    printf "### Compiling for $PROC: ";
    OK="FAILED";
    echo "$CC" -V $CPPFLAGS $CFLAGS $LDFLAGS -mpic14 -p$PROC -o test-$PROC sample.c $LIBS > "$BUILD_LOG2";
    "$CC" -V $CPPFLAGS $CFLAGS $LDFLAGS -mpic14 -p$PROC -o test-$PROC sample.c $LIBS >> "$BUILD_LOG2" 2>&1 && OK="ok";
    cat "$BUILD_LOG2" >> "$BUILD_LOG";
    printf "$OK\n";
    case "$OK" in
        ok)
            ;;
        *)
            cat "$BUILD_LOG2";
            printf "\n";
            ;;
    esac;
done;

cd "$BASE";
cleanup;
