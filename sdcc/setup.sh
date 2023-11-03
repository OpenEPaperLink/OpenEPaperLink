#/bin/sh
# set environment variables to use locally built sdcc version 4.2.0

#set -x
SDCC_VER=4.2.0

if [ "${BASH_SOURCE[0]}" -ef "$0" ]; then
   echo "This script must be SOURCED, not run directly (source $0)."
   exit 1
fi

SDCC_DIR=`dirname ${BASH_SOURCE[0]}`

if [ ! -e ${SDCC_DIR}/sdcc/${SDCC_VER} ]; then
    (cd ${SDCC_DIR};./build_sdcc.sh)
fi

export CC=sdcc
SDCC_PATH=${SDCC_DIR}/sdcc/${SDCC_VER}/bin
echo $PATH | grep ${SDCC_PATH} > /dev/null
if [ $? -ne 0 ]; then
   export SDCC_PATH_SAVE=$PATH
   export PATH=${SDCC_PATH}:$PATH
   echo "Added ${SDCC_PATH} to PATH"
fi

