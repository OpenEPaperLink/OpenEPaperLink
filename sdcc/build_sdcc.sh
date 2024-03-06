#/bin/sh
# build local version of sdcc from source

#set -x
SDCC_VER=${1:-4.2.0}
SDCC_DIR=sdcc-${SDCC_VER}

echo "Building ${SDCC_DIR}, please be patient this is going to take a while!"

if [ ! -e build ]; then
   mkdir build
fi
cd build
if [ ${SDCC_VER} = "trunk" ]; then
   SDCC_DIR="trunk"
   SVN_URL="svn://svn.code.sf.net/p/sdcc/code/trunk"
else
   SVN_URL="svn://svn.code.sf.net/p/sdcc/code/tags/${SDCC_DIR}"
fi

echo -n "Checking out ${SDCC_DIR} source ..."
svn checkout ${SVN_URL} > /dev/null
echo
if [ $? -ne 0 ]; then
  echo "Couldn't checkout ${SVN_URL}"
  exit 1
fi

cd ${SDCC_DIR}/sdcc
CWD=`pwd`
echo -n "Configuring ..."
./configure --prefix=${CWD}/../../../sdcc-${SDCC_VER} --disable-z80-port --disable-z180-port --disable-r2k-port --disable-r2ka-port --disable-r3ka-port --disable-sm83-port --disable-tlcs90-port --disable-ez80_z80-port --disable-z80n-port --disable-ds390-port --disable-ds400-port --disable-pic14-port --disable-pic16-port --disable-hc08-port --disable-s08-port --disable-stm8-port --disable-pdk13-port --disable-pdk14-port --disable-pdk15-port --disable-pdk16-port --disable-mos6502-port --disable-mos65c02-port > build.log 2>&1 
echo
if [ $? -ne 0 ]; then
  echo "configure failed see ${CWD}/build.log for details"
  exit 1
fi
echo -n "Compiling ..."
make -j8 >> build.log 2>&1
status=$?
if [ $status  -ne 0 ]; then
  echo "sdcc make failed (${status}) see ${CWD}/build.log for details"
  exit 1
fi
echo
echo -n "Installing ..."
make -j8  install >> build.log 2>&1 
if [ $? -ne 0 ]; then
  echo "sdcc make install failed see ${CWD}/build.log for details"
  exit 1
fi
#echo "Cleaning up ..."
#cd ../../..
#rm -rf build

