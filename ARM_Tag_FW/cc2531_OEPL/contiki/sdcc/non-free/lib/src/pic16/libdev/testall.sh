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
#include <pic18fregs.h>
#include <stdio.h>
#pragma stack 0x00c0 0x40

void foo(char *c)
{
  if (*c)
    *c = 1;
}

void main()
{
  PORTA = INTCON;
  LATB = TRISB;
  printf_tiny("foo"); /* printf() is too large for the smallest devices ... */
}
EOT

for f in ./pic1*.c; do
    p="${f##*/pic}";
    p="${p%.c}";
    
    printf "### Compiling for $p: ";
    OK="FAILED";
    echo "$CC" -V $CPPFLAGS $CFLAGS $LDFLAGS -mpic16 -p$p -o test-$p sample.c $LIBS > "$BUILD_LOG2";
    "$CC" -V $CPPFLAGS $CFLAGS $LDFLAGS -mpic16 -p$p -o test-$p sample.c $LIBS >> "$BUILD_LOG2" 2>&1 && OK="ok";
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
