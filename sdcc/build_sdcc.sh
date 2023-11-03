#/bin/sh
# build local version of sdcc 4.2.0 from source

SDCC_VER=4.2.0

if [ ! -e sdcc ]; then
   git clone https://github.com/swegener/sdcc.git
   if [ $? -ne 0 ]; then
      echo "Couldn't clone https://github.com/swegener/sdcc.git"
      exit 1
   fi
fi
cd sdcc
git tag 4.2.0 56dc646f8ba6d37c039c5e0c3e60b1340da65b9f
git checkout ${SDCC_VER}
if [ $? -ne 0 ]; then
  echo "Couldn't checkout ${SDCC_VER} source from git"
  exit 1
fi

./configure --prefix=`pwd`/${SDCC_VER} --disable-z80-port --disable-z180-port --disable-r2k-port --disable-r2ka-port --disable-r3ka-port --disable-sm83-port --disable-tlcs90-port --disable-ez80_z80-port --disable-z80n-port --disable-ds390-port --disable-ds400-port --disable-pic14-port --disable-pic16-port --disable-hc08-port --disable-s08-port --disable-stm8-port --disable-pdk13-port --disable-pdk14-port --disable-pdk15-port --disable-pdk16-port --disable-mos6502-port --disable-mos65c02-port 2>&1 | tee build.log
make -j8  2>&1 | tee -a build.log
if [ $? -ne 0 ]; then
  echo "sdcc make failed see ./sdcc/build.log for details"
  exit 1
fi

make -j8  install 2>&1 | tee -a build.log
if [ $? -ne 0 ]; then
  echo "sdcc make install failed see ./sdcc/build.log for details"
  exit 1
fi


