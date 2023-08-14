# -----------------------------------------------------------------------------
# This file is part of the xPack distribution.
#   (https://xpack.github.io)
# Copyright (c) 2019 Liviu Ionescu.
#
# Permission to use, copy, modify, and/or distribute this software 
# for any purpose is hereby granted, under the terms of the MIT license.
# -----------------------------------------------------------------------------

# Helper script used in the second edition of the xPack build 
# scripts. As the name implies, it should contain only functions and 
# should be included with 'source' by the container build scripts.

# -----------------------------------------------------------------------------

function download_openocd() 
{
  if [ ! -d "${WORK_FOLDER_PATH}/${OPENOCD_SRC_FOLDER_NAME}" ]
  then
    (
      cd "${WORK_FOLDER_PATH}"
      git_clone "${OPENOCD_GIT_URL}" "${OPENOCD_GIT_BRANCH}" \
          "${OPENOCD_GIT_COMMIT}" "${OPENOCD_SRC_FOLDER_NAME}"
      cd "${WORK_FOLDER_PATH}/${OPENOCD_SRC_FOLDER_NAME}"
      git submodule update --init --recursive --remote
    )
  fi
}

# -----------------------------------------------------------------------------

function build_openocd()
{
  download_openocd

  local openocd_folder_name="openocd"

  mkdir -pv "${LOGS_FOLDER_PATH}/${openocd_folder_name}/"

  local openocd_stamp_file_path="${INSTALL_FOLDER_PATH}/stamp-openocd-installed"
  if [ ! -f "${openocd_stamp_file_path}" ]
  then
    (
      xbb_activate_installed_dev


      cd "${WORK_FOLDER_PATH}/${OPENOCD_SRC_FOLDER_NAME}"
      (
        if [ ! -d "autom4te.cache" ]
        then
          ./bootstrap
        fi
      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${openocd_folder_name}/configure-output.txt"

      mkdir -pv "${APP_BUILD_FOLDER_PATH}"
      cd "${APP_BUILD_FOLDER_PATH}"

      CPPFLAGS="${XBB_CPPFLAGS}"
      CFLAGS="${XBB_CFLAGS_NO_W}" 
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}" 
      
      # It makes little sense to use -static-libgcc here, since
      # several shared libraries will refer to it anyway.
      LDFLAGS="${XBB_LDFLAGS_APP}"
      LIBS=""

      export CPPFLAGS 
      export CFLAGS 
      export CXXFLAGS 

      export LDFLAGS
      export LIBS

      export JAYLINK_CFLAGS='${XBB_CFLAGS} -fvisibility=hidden'

      if [ ! -f "config.status" ]
      then

        # May be required for repetitive builds, because this is an executable built 
        # in place and using one for a different architecture may not be a good idea.
        rm -rfv "${WORK_FOLDER_PATH}/${OPENOCD_SRC_FOLDER_NAME}/jimtcl/autosetup/jimsh0"

        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi
          
          echo
          echo "Running openocd configure..."
      
          if [ "${IS_DEVELOP}" == "y" ]
          then
            bash "${WORK_FOLDER_PATH}/${OPENOCD_SRC_FOLDER_NAME}/configure" --help
          fi

          config_options=()

          config_options+=("--prefix=${APP_PREFIX}")
            
          config_options+=("--build=${BUILD}")
          config_options+=("--host=${HOST}")
          config_options+=("--target=${TARGET}")

          config_options+=("--datarootdir=${INSTALL_FOLDER_PATH}")
          config_options+=("--localedir=${APP_PREFIX}/share/locale")
          config_options+=("--mandir=${APP_PREFIX_DOC}/man")
          config_options+=("--pdfdir=${APP_PREFIX_DOC}/pdf")
          config_options+=("--infodir=${APP_PREFIX_DOC}/info")
          config_options+=("--docdir=${APP_PREFIX_DOC}")
                
          config_options+=("--disable-wextra")
          config_options+=("--disable-werror")
          config_options+=("--enable-dependency-tracking")
                
          config_options+=("--enable-branding=${BRANDING}")
        
          # Add explicit functionality.
          config_options+=("--enable-aice")
          config_options+=("--enable-armjtagew")
          config_options+=("--enable-at91rm9200")
          config_options+=("--enable-bcm2835gpio")
          config_options+=("--enable-cmsis-dap")
          config_options+=("--enable-dummy")
          config_options+=("--enable-ep93xx")
          config_options+=("--enable-ft232r")
          config_options+=("--enable-ftdi")
          config_options+=("--enable-imx_gpio")
          config_options+=("--enable-jlink")
          config_options+=("--enable-jtag_vpi")
          config_options+=("--enable-kitprog")
          # Deprecated
          # config_options+=("--enable-oocd_trace")
          config_options+=("--enable-opendous")
          config_options+=("--enable-openjtag")
          config_options+=("--enable-osbdm")
          config_options+=("--enable-presto")
          config_options+=("--enable-remote-bitbang")
          config_options+=("--enable-rlink")
          config_options+=("--enable-stlink")
          config_options+=("--enable-ti-icdi")
          config_options+=("--enable-ulink")
          config_options+=("--enable-usb-blaster")
          config_options+=("--enable-usb_blaster_2")
          config_options+=("--enable-usbprog")
          config_options+=("--enable-vsllink")
          config_options+=("--enable-xds110")

          # Disable drivers that apparently failed to build on all platforms.
          config_options+=("--disable-zy1000-master")
          config_options+=("--disable-zy1000")
          config_options+=("--disable-ioutil")
          config_options+=("--disable-minidriver-dummy")
          config_options+=("--disable-parport-ppdev")

          if [ "${TARGET_PLATFORM}" == "win32" ]
          then

            export OUTPUT_DIR="${BUILD_FOLDER_PATH}"
            
            # Without it, mingw redefines it as 0.
            CPPFLAGS+=" -D__USE_MINGW_ANSI_STDIO=1"

            # --enable-minidriver-dummy -> configure error
            # --enable-zy1000 -> netinet/tcp.h: No such file or directory

            # --enable-openjtag_ftdi -> --enable-openjtag
            # --enable-presto_libftdi -> --enable-presto
            # --enable-usb_blaster_libftdi -> --enable-usb_blaster

            config_options+=("--enable-amtjtagaccel")
            config_options+=("--enable-gw16012")
            config_options+=("--enable-parport")
            config_options+=("--enable-parport-giveio")

            # --enable-sysfsgpio -> available only on Linux
            config_options+=("--disable-sysfsgpio")
            # --enable-buspirate -> not supported on mingw
            config_options+=("--disable-buspirate")

            # oocd_trace.h:22:10: fatal error: termios.h: No such file or directory
            config_options+=("--disable-oocd_trace")

          elif [ "${TARGET_PLATFORM}" == "linux" ]
          then

            LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
            LIBS+=" -lpthread -lrt -ludev"

            # --enable-minidriver-dummy -> configure error

            # --enable-openjtag_ftdi -> --enable-openjtag
            # --enable-presto_libftdi -> --enable-presto
            # --enable-usb_blaster_libftdi -> --enable-usb_blaster

            config_options+=("--enable-amtjtagaccel")
            config_options+=("--enable-buspirate")
            config_options+=("--enable-gw16012")
            config_options+=("--enable-parport")
            config_options+=("--enable-parport-giveio")
            config_options+=("--enable-sysfsgpio")

            # Deprecated
            # config_options+=("--enable-oocd_trace")

          elif [ "${TARGET_PLATFORM}" == "darwin" ]
          then

            # --enable-minidriver-dummy -> configure error

            # --enable-openjtag_ftdi -> --enable-openjtag
            # --enable-presto_libftdi -> --enable-presto
            # --enable-usb_blaster_libftdi -> --enable-usb_blaster

            config_options+=("--enable-buspirate")

            # --enable-amtjtagaccel -> 'sys/io.h' file not found
            config_options+=("--disable-amtjtagaccel")
            # --enable-gw16012 -> 'sys/io.h' file not found
            config_options+=("--disable-gw16012")
            config_options+=("--disable-parport")
            config_options+=("--disable-parport-giveio")
            # --enable-sysfsgpio -> available only on Linux
            config_options+=("--disable-sysfsgpio")

            # /Users/ilg/Work/openocd-0.10.0-14/openocd.git/src/target/oocd_trace.c: In function ‘oocd_trace_init’:
            # /Users/ilg/Work/openocd-0.10.0-14/openocd.git/src/target/oocd_trace.c:121:54: error: ‘B2500000’ undeclared (first use in this function)
            config_options+=("--disable-oocd_trace")

          else

            echo "Unsupported target platorm ${TARGET_PLATFORM}."
            exit 1

          fi

          run_verbose bash ${DEBUG} "${WORK_FOLDER_PATH}/${OPENOCD_SRC_FOLDER_NAME}/configure" \
            "${config_options[@]}"

          cp "config.log" "${LOGS_FOLDER_PATH}/${openocd_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${openocd_folder_name}/configure-output.txt"

      fi

      (
        echo
        echo "Running openocd make..."
      
        # Build.
        run_verbose make -j ${JOBS} bindir="bin" pkgdatadir=""

        if [ "${WITH_STRIP}" == "y" ]
        then
          run_verbose make install-strip
        else
          run_verbose make install  
        fi

        if [ "${TARGET_PLATFORM}" == "win32" ]
        then
          rm -f "${APP_PREFIX}/bin/openocdw.exe"
        fi

        (
          xbb_activate_tex

          if [ "${WITH_PDF}" == "y" ]
          then
            run_verbose make bindir="bin" pkgdatadir="" pdf 
            run_verbose make install-pdf
          fi

          if [ "${WITH_HTML}" == "y" ]
          then
            run_verbose make bindir="bin" pkgdatadir="" html
            run_verbose make install-html
          fi
        )

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${openocd_folder_name}/make-output.txt"

      copy_license \
        "${WORK_FOLDER_PATH}/${OPENOCD_SRC_FOLDER_NAME}" \
        "${OPENOCD_FOLDER_NAME}"
    )

    touch "${openocd_stamp_file_path}"
  else
    echo "Component openocd already installed."
  fi

  tests_add "test_openocd"
}

function test_openocd()
{
  if [ -d "xpacks/.bin" ]
  then
    OPENOCD="xpacks/.bin/openocd"
  elif [ -d "${APP_PREFIX}/bin" ]
  then
    OPENOCD="${APP_PREFIX}/bin/openocd"
  else
    echo "Wrong folder."
    exit 1
  fi

  echo
  echo "Checking the openocd shared libraries..."
  show_libs "${OPENOCD}"

  echo
  echo "Checking if openocd starts..."

  run_app "${OPENOCD}" --version

  run_app "${OPENOCD}" \
    -c "adapter driver dummy" \
    -c "adapter speed 1000" \
    -c "adapter list" \
    -c "transport list" \
    -c "target types" \
    -c "echo baburiba" \
    -c "shutdown"

}

# -----------------------------------------------------------------------------
