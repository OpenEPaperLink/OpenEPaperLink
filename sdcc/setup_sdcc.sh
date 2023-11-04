#/bin/sh
# set environment variables to use locally built sdcc 

#set -x
SDCC_VER=${1:-4.2.0}

if [ "${BASH_SOURCE[0]}" -ef "$0" ]; then
   echo "This script must be SOURCED, not run directly (source $0)."
   exit 1
fi

SETUP_PATH=`readlink -f ${BASH_SOURCE[0]}`
SETUP_DIR=`dirname ${SETUP_PATH}`
SDCC_VER_DIR=${SETUP_DIR}/sdcc-${SDCC_VER}
source ${SETUP_DIR}/unset_sdcc.sh
if [ ! -e  ${SDCC_VER_DIR} ]; then
    (cd ${SETUP_DIR};./build_sdcc.sh ${SDCC_VER})
fi

SDCC_PATH=${SDCC_VER_DIR}/bin
echo $PATH | grep ${SDCC_PATH} > /dev/null
if [ $? -ne 0 ]; then
   export CC=sdcc
   export SDCC_PATH_SAVE=$PATH
   export PATH=${SDCC_PATH}:$PATH
   echo "Added ${SDCC_PATH} to PATH"
fi

