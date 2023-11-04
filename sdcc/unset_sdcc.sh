#/bin/sh
# set environment variables to use locally built sdcc version 4.2.0

#set -x
if [ "${BASH_SOURCE[0]}" -ef "$0" ]; then
   echo "This script must be SOURCED, not run directly (source $0)."
   exit 1
fi

if [ ! "${SDCC_PATH_SAVE}x" = "x" ]; then
   export PATH=${SDCC_PATH_SAVE}
   export SDCC_PATH_SAVE=
   export CC=
fi

