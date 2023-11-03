#/bin/sh
# set environment variables to use locally built sdcc version 4.2.0

#set -x
SDCC_VER=4.2.0
if [ "${BASH_SOURCE[0]}" -ef "$0" ]; then
   echo "This script must be SOURCED, not run directly (source $0)."
   exit 1
fi

SDCC_DIR=`dirname ${BASH_SOURCE[0]}`
SDCC_PATH=${SDCC_DIR}/sdcc/${SDCC_VER}/bin
echo $PATH | grep ${SDCC_PATH} > /dev/null
if [ ! "${SDCC_PATH_SAVE}x" = "x" ]; then
   export PATH=${SDCC_PATH_SAVE}
   export SDCC_PATH_SAVE=
   export CC=
fi

