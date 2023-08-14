# -----------------------------------------------------------------------------
# This file is part of the xPack distribution.
#   (https://xpack.github.io)
# Copyright (c) 2020 Liviu Ionescu.
#
# Permission to use, copy, modify, and/or distribute this software 
# for any purpose is hereby granted, under the terms of the MIT license.
# -----------------------------------------------------------------------------

# Helper script used in the second edition of the xPack build 
# scripts. As the name implies, it should contain only functions and 
# should be included with 'source' by the build scripts (both native
# and container).

# -----------------------------------------------------------------------------

function build_zlib() 
{
  # http://zlib.net
  # http://zlib.net/fossils/

  # https://archlinuxarm.org/packages/aarch64/zlib/files/PKGBUILD
  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=zlib-static
  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=zlib-git
  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=mingw-w64-zlib

  # 2013-04-28 "1.2.8"
  # 2017-01-15 "1.2.11"

  local zlib_version="$1"

  # The folder name as resulted after being extracted from the archive.
  local zlib_src_folder_name="zlib-${zlib_version}"

  local zlib_archive="${zlib_src_folder_name}.tar.gz"
  local zlib_url="http://zlib.net/fossils/${zlib_archive}"

  # The folder name for build, licenses, etc.
  local zlib_folder_name="${zlib_src_folder_name}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${zlib_folder_name}"

  local zlib_stamp_file_path="${STAMPS_FOLDER_PATH}/stamp-${zlib_folder_name}-installed"
  if [ ! -f "${zlib_stamp_file_path}" ]
  then

    echo
    echo "zlib in-source building"

    if [ ! -d "${LIBS_BUILD_FOLDER_PATH}/${zlib_folder_name}" ]
    then
      cd "${LIBS_BUILD_FOLDER_PATH}"

      download_and_extract "${zlib_url}" "${zlib_archive}" \
        "${zlib_src_folder_name}"

      if [ "${zlib_src_folder_name}" != "${zlib_folder_name}" ]
      then
        mv -v "${zlib_src_folder_name}" "${zlib_folder_name}"
      fi
    fi

    (
      cd "${LIBS_BUILD_FOLDER_PATH}/${zlib_folder_name}"

      xbb_activate_installed_dev

      if [ "${TARGET_PLATFORM}" == "win32" ]
      then
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running zlib make..."

          # Build.
          run_verbose make -f win32/Makefile.gcc \
            PREFIX=${CROSS_COMPILE_PREFIX}- \
            prefix="${LIBS_INSTALL_FOLDER_PATH}" \
            CFLAGS="${XBB_CFLAGS_NO_W} -Wp,-D_FORTIFY_SOURCE=2 -fexceptions --param=ssp-buffer-size=4"
          
          run_verbose make -f win32/Makefile.gcc install \
            DESTDIR="${LIBS_INSTALL_FOLDER_PATH}/" \
            INCLUDE_PATH="include" \
            LIBRARY_PATH="lib" \
            BINARY_PATH="bin"

        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${zlib_folder_name}/make-output.txt"
      else

        CPPFLAGS="${XBB_CPPFLAGS}"
        CFLAGS="${XBB_CFLAGS_NO_W}"
        CXXFLAGS="${XBB_CXXFLAGS_NO_W}"

        LDFLAGS="${XBB_LDFLAGS_LIB}"
        if [ "${TARGET_PLATFORM}" == "linux" ]
        then
          LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
        fi      

        export CPPFLAGS
        export CFLAGS
        export CXXFLAGS
        export LDFLAGS

        # No config.status left, use the library.
        if [ ! -f "libz.a" ]
        then
          (
            if [ "${IS_DEVELOP}" == "y" ]
            then
              env | sort
            fi

            echo
            echo "Running zlib configure..."

            if [ "${IS_DEVELOP}" == "y" ]
            then
              run_verbose bash "configure" --help
            fi

            run_verbose bash ${DEBUG} "configure" \
              --prefix="${LIBS_INSTALL_FOLDER_PATH}" 
            
            cp "configure.log" "${LOGS_FOLDER_PATH}/${zlib_folder_name}/configure-log.txt"
          ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${zlib_folder_name}/configure-output.txt"
        fi

        (
          echo
          echo "Running zlib make..."

          # Build.
          run_verbose make -j ${JOBS}

          if [ "${WITH_TESTS}" == "y" ]
          then
            run_verbose make -j1 test
          fi

          run_verbose make install

        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${zlib_folder_name}/make-output.txt"
      fi

      copy_license \
        "${LIBS_BUILD_FOLDER_PATH}/${zlib_folder_name}" \
        "${zlib_folder_name}"

    )

    (
      test_zlib
    ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${zlib_folder_name}/test-output.txt"

    touch "${zlib_stamp_file_path}"

  else
    echo "Library zlib already installed."
  fi
}

function test_zlib()
{
  (
    xbb_activate

    echo
    echo "Checking the zlib shared libraries..."

    show_libs "${LIBS_INSTALL_FOLDER_PATH}/lib/libz.${SHLIB_EXT}"
  )
}

# -----------------------------------------------------------------------------

function build_gmp() 
{
  # https://gmplib.org
  # https://gmplib.org/download/gmp/

  # https://archlinuxarm.org/packages/aarch64/gmp/files/PKGBUILD
  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=gmp-hg

  # 01-Nov-2015 "6.1.0"
  # 16-Dec-2016 "6.1.2"
  # 17-Jan-2020 "6.2.0"
  # 14-Nov-2020, "6.2.1"

  local gmp_version="$1"

  # The folder name as resulted after being extracted from the archive.
  local gmp_src_folder_name="gmp-${gmp_version}"

  local gmp_archive="${gmp_src_folder_name}.tar.xz"
  local gmp_url="https://gmplib.org/download/gmp/${gmp_archive}"

  # The folder name for build, licenses, etc.
  local gmp_folder_name="${gmp_src_folder_name}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${gmp_folder_name}"

  local gmp_stamp_file_path="${STAMPS_FOLDER_PATH}/stamp-${gmp_folder_name}-installed"
  if [ ! -f "${gmp_stamp_file_path}" ]
  then

    cd "${SOURCES_FOLDER_PATH}"

    download_and_extract "${gmp_url}" "${gmp_archive}" \
      "${gmp_src_folder_name}"

    (
      mkdir -pv "${LIBS_BUILD_FOLDER_PATH}/${gmp_folder_name}"
      cd "${LIBS_BUILD_FOLDER_PATH}/${gmp_folder_name}"

      xbb_activate_installed_dev

      # Exceptions used by Arm GCC script and by mingw-w64.
      CPPFLAGS="${XBB_CPPFLAGS} -fexceptions"
      # Test fail with -Ofast, revert to -O2
      CFLAGS="${XBB_CFLAGS_NO_W}" 
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"

      LDFLAGS="${XBB_LDFLAGS_LIB}"
      if [ "${TARGET_PLATFORM}" == "linux" ]
      then
        LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
      fi      

      if [ "${TARGET_PLATFORM}" == "win32" ]
      then
        export CC_FOR_BUILD="${NATIVE_CC}"
      fi

      export CPPFLAGS
      export CFLAGS
      export CXXFLAGS
      export LDFLAGS

      # ABI is mandatory, otherwise configure fails on 32-bit.
      # (see https://gmplib.org/manual/ABI-and-ISA.html)
      if [ "${TARGET_ARCH}" == "x64" -o "${TARGET_ARCH}" == "x32" -o "${TARGET_ARCH}" == "ia32" ]
      then
        export ABI="${TARGET_BITS}"
      fi

      if [ ! -f "config.status" ]
      then 
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running gmp configure..."

          # ABI is mandatory, otherwise configure fails on 32-bit.
          # (see https://gmplib.org/manual/ABI-and-ISA.html)

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${gmp_src_folder_name}/configure" --help
          fi

          config_options=()

          config_options+=("--prefix=${LIBS_INSTALL_FOLDER_PATH}")
            
          config_options+=("--build=${BUILD}")
          config_options+=("--host=${HOST}")
          config_options+=("--target=${TARGET}")
            
          config_options+=("--enable-cxx")

          if [ "${TARGET_PLATFORM}" == "win32" ]
          then
            # mpfr asks for this explicitly during configure.
            # (although the message is confusing)
            config_options+=("--enable-shared")
            config_options+=("--disable-static")
          fi

          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${gmp_src_folder_name}/configure" \
            "${config_options[@]}"

          if [ "${TARGET_PLATFORM}" == "darwin" ] # and clang
          then
            # Disable failing `t-sqrlo` test.
            run_verbose sed -i.bak \
              -e 's| t-sqrlo$(EXEEXT) | |' \
              "tests/mpn/Makefile"
          fi

          cp "config.log" "${LOGS_FOLDER_PATH}/${gmp_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${gmp_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running gmp make..."

        # Build.
        run_verbose make -j ${JOBS}

        if [ "${WITH_TESTS}" == "y" ]
        then
          run_verbose make -j1 check
        fi

        if [ "${WITH_STRIP}" == "y" ]
        then
          run_verbose make install-strip
        else
          run_verbose make install
        fi

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${gmp_folder_name}/make-output.txt"

      copy_license \
        "${SOURCES_FOLDER_PATH}/${gmp_src_folder_name}" \
        "${gmp_folder_name}"

    )

    touch "${gmp_stamp_file_path}"

  else
    echo "Library gmp already installed."
  fi
}

# Depends on gmp.
function build_mpfr()
{
  # http://www.mpfr.org
  # http://www.mpfr.org/history.html

  # https://archlinuxarm.org/packages/aarch64/mpfr/files/PKGBUILD
  # https://git.archlinux.org/svntogit/packages.git/tree/trunk/PKGBUILD?h=packages/mpfr

  # 6 March 2016 "3.1.4"
  # 7 September 2017 "3.1.6"
  # 31 January 2019 "4.0.2"
  # 10 July 2020 "4.1.0"

  local mpfr_version="$1"

  # The folder name as resulted after being extracted from the archive.
  local mpfr_src_folder_name="mpfr-${mpfr_version}"

  local mpfr_archive="${mpfr_src_folder_name}.tar.xz"
  local mpfr_url="http://www.mpfr.org/${mpfr_src_folder_name}/${mpfr_archive}"

  # The folder name for build, licenses, etc.
  local mpfr_folder_name="${mpfr_src_folder_name}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${mpfr_folder_name}"

  local mpfr_stamp_file_path="${STAMPS_FOLDER_PATH}/stamp-${mpfr_folder_name}-installed"
  if [ ! -f "${mpfr_stamp_file_path}" ]
  then

    cd "${SOURCES_FOLDER_PATH}"

    download_and_extract "${mpfr_url}" "${mpfr_archive}" \
      "${mpfr_src_folder_name}"

    (
      mkdir -pv "${LIBS_BUILD_FOLDER_PATH}/${mpfr_folder_name}"
      cd "${LIBS_BUILD_FOLDER_PATH}/${mpfr_folder_name}"

      xbb_activate_installed_dev

      CPPFLAGS="${XBB_CPPFLAGS}"
      CFLAGS="${XBB_CFLAGS_NO_W}"
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"

      LDFLAGS="${XBB_LDFLAGS_LIB}"
      if [ "${TARGET_PLATFORM}" == "linux" ]
      then
        LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
      fi      

      export CPPFLAGS
      export CFLAGS
      export CXXFLAGS
      export LDFLAGS

      if [ ! -f "config.status" ]
      then 
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running mpfr configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${mpfr_src_folder_name}/configure" --help
          fi

          config_options=()

          config_options+=("--prefix=${LIBS_INSTALL_FOLDER_PATH}")
            
          config_options+=("--build=${BUILD}")
          config_options+=("--host=${HOST}")
          config_options+=("--target=${TARGET}")

          config_options+=("--with-gmp=${LIBS_INSTALL_FOLDER_PATH}")

          config_options+=("--disable-warnings")
          
          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${mpfr_src_folder_name}/configure" \
            "${config_options[@]}"
             
          cp "config.log" "${LOGS_FOLDER_PATH}/${mpfr_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${mpfr_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running mpfr make..."

        # Build.
        run_verbose make -j ${JOBS}

        if [ "${WITH_TESTS}" == "y" ]
        then
          run_verbose make -j1 check
        fi

        if [ "${WITH_STRIP}" == "y" ]
        then
          run_verbose make install-strip
        else
          run_verbose make install
        fi

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${mpfr_folder_name}/make-output.txt"

      copy_license \
        "${SOURCES_FOLDER_PATH}/${mpfr_src_folder_name}" \
        "${mpfr_folder_name}"

    )
    touch "${mpfr_stamp_file_path}"

  else
    echo "Library mpfr already installed."
  fi
}

# Depends on gmp, mpfr.
function build_mpc()
{
  # http://www.multiprecision.org/
  # ftp://ftp.gnu.org/gnu/mpc

  # https://archlinuxarm.org/packages/aarch64/mpc/files/PKGBUILD
  # https://git.archlinux.org/svntogit/packages.git/tree/trunk/PKGBUILD?h=packages/libmpc

  # 20 Feb 2015 "1.0.3"
  # 2018-01-11 "1.1.0"
  # 2020-08 "1.2.0"
  # 2020-10 "1.2.1"

  local mpc_version="$1"

  # The folder name as resulted after being extracted from the archive.
  local mpc_src_folder_name="mpc-${mpc_version}"

  local mpc_archive="${mpc_src_folder_name}.tar.gz"
  local mpc_url="ftp://ftp.gnu.org/gnu/mpc/${mpc_archive}"
  if [[ "${mpc_version}" =~ 0\.* ]]
  then
    mpc_url="http://www.multiprecision.org/downloads/${mpc_archive}"
  fi

  # The folder name for build, licenses, etc.
  local mpc_folder_name="${mpc_src_folder_name}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${mpc_folder_name}"

  local mpc_stamp_file_path="${STAMPS_FOLDER_PATH}/stamp-${mpc_folder_name}-installed"
  if [ ! -f "${mpc_stamp_file_path}" ]
  then

    cd "${SOURCES_FOLDER_PATH}"

    download_and_extract "${mpc_url}" "${mpc_archive}" \
      "${mpc_src_folder_name}"

    (
      mkdir -pv "${LIBS_BUILD_FOLDER_PATH}/${mpc_folder_name}"
      cd "${LIBS_BUILD_FOLDER_PATH}/${mpc_folder_name}"

      xbb_activate_installed_dev

      CPPFLAGS="${XBB_CPPFLAGS}"
      CFLAGS="${XBB_CFLAGS_NO_W}"
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"

      LDFLAGS="${XBB_LDFLAGS_LIB}"
      if [ "${TARGET_PLATFORM}" == "linux" ]
      then
        LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
      fi      

      export CPPFLAGS
      export CFLAGS
      export CXXFLAGS
      export LDFLAGS

      if [ ! -f "config.status" ]
      then 
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running mpc configure..."
        
          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${mpc_src_folder_name}/configure" --help
          fi

          config_options=()

          config_options+=("--prefix=${LIBS_INSTALL_FOLDER_PATH}")
            
          config_options+=("--build=${BUILD}")
          config_options+=("--host=${HOST}")
          config_options+=("--target=${TARGET}")

          config_options+=("--with-gmp=${LIBS_INSTALL_FOLDER_PATH}")
          config_options+=("--with-mpfr=${LIBS_INSTALL_FOLDER_PATH}")

          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${mpc_src_folder_name}/configure" \
            "${config_options[@]}"
            
          cp "config.log" "${LOGS_FOLDER_PATH}/${mpc_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${mpc_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running mpc make..."

        # Build.
        run_verbose make -j ${JOBS}

        if [ "${WITH_TESTS}" == "y" ]
        then
          run_verbose make -j1 check
        fi

        if [ "${WITH_STRIP}" == "y" ]
        then
          run_verbose make install-strip
        else
          run_verbose make install
        fi

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${mpc_folder_name}/make-output.txt"

      copy_license \
        "${SOURCES_FOLDER_PATH}/${mpc_src_folder_name}" \
        "${mpc_folder_name}"

    )
    touch "${mpc_stamp_file_path}"

  else
    echo "Library mpc already installed."
  fi
}

# Depends on gmp.
function build_isl()
{
  # http://isl.gforge.inria.fr

  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=isl

  # 2015-06-12 "0.15"
  # 2016-01-15 "0.16.1"
  # 2016-12-20 "0.18"
  # 2019-03-26 "0.21"
  # 2020-01-16 "0.22"
  # 2020-11-11 "0.23"
  # 2021-05-01 "0.24"

  local isl_version="$1"

  # The folder name as resulted after being extracted from the archive.
  local isl_src_folder_name="isl-${isl_version}"

  local isl_archive="${isl_src_folder_name}.tar.xz"
  if [[ "${isl_version}" =~ 0\.12\.* ]]
  then
    isl_archive="${isl_src_folder_name}.tar.gz"
  fi

  local isl_url="http://isl.gforge.inria.fr/${isl_archive}"

  # The folder name for build, licenses, etc.
  local isl_folder_name="${isl_src_folder_name}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${isl_folder_name}"

  local isl_stamp_file_path="${STAMPS_FOLDER_PATH}/stamp-${isl_folder_name}-installed"
  if [ ! -f "${isl_stamp_file_path}" ]
  then

    cd "${SOURCES_FOLDER_PATH}"

    download_and_extract "${isl_url}" "${isl_archive}" \
      "${isl_src_folder_name}"

    (
      mkdir -pv "${LIBS_BUILD_FOLDER_PATH}/${isl_folder_name}"
      cd "${LIBS_BUILD_FOLDER_PATH}/${isl_folder_name}"

      xbb_activate_installed_dev

      CPPFLAGS="${XBB_CPPFLAGS}"
      CFLAGS="${XBB_CFLAGS_NO_W}"
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"

      LDFLAGS="${XBB_LDFLAGS_LIB}"
      if [ "${TARGET_PLATFORM}" == "linux" ]
      then
        LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
      fi      

      export CPPFLAGS
      export CFLAGS
      export CXXFLAGS
      export LDFLAGS

      if [ ! -f "config.status" ]
      then 
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running isl configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${isl_src_folder_name}/configure" --help
          fi

          config_options=()

          config_options+=("--prefix=${LIBS_INSTALL_FOLDER_PATH}")
            
          config_options+=("--build=${BUILD}")
          config_options+=("--host=${HOST}")
          config_options+=("--target=${TARGET}")

          config_options+=("--with-gmp=${LIBS_INSTALL_FOLDER_PATH}")

          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${isl_src_folder_name}/configure" \
            "${config_options[@]}"
            
          cp "config.log" "${LOGS_FOLDER_PATH}/${isl_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${isl_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running isl make..."

        # Build.
        run_verbose make -j ${JOBS}

        if [ "${WITH_TESTS}" == "y" ]
        then
          if [ "${TARGET_PLATFORM}" == "linux" -a \
            \( "${TARGET_ARCH}" == "x64" -o "${TARGET_ARCH}" == "x32" -o "${TARGET_ARCH}" == "ia32" \) ] 
          then
            # /Host/Users/ilg/Work/gcc-8.4.0-1/linux-x64/build/libs/isl-0.22/.libs/lt-isl_test_cpp: relocation error: /Host/Users/ilg/Work/gcc-8.4.0-1/linux-x64/build/libs/isl-0.22/.libs/lt-isl_test_cpp: symbol _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_createERmm, version GLIBCXX_3.4.21 not defined in file libstdc++.so.6 with link time reference
            # FAIL isl_test_cpp (exit status: 127)
            # /Host/Users/ilg/Work/gcc-8.4.0-1/linux-x32/build/libs/isl-0.22/.libs/lt-isl_test_cpp: relocation error: /Host/Users/ilg/Work/gcc-8.4.0-1/linux-x32/build/libs/isl-0.22/.libs/lt-isl_test_cpp: symbol _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_createERjj, version GLIBCXX_3.4.21 not defined in file libstdc++.so.6 with link time reference
            # FAIL isl_test_cpp (exit status: 127)

            run_verbose make -j1 check || true
          else
            run_verbose make -j1 check
          fi
        fi

        if [ "${WITH_STRIP}" == "y" ]
        then
          run_verbose make install-strip
        else
          run_verbose make install
        fi

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${isl_folder_name}/make-output.txt"

      copy_license \
        "${SOURCES_FOLDER_PATH}/${isl_src_folder_name}" \
        "${isl_folder_name}"

    )
    touch "${isl_stamp_file_path}"

  else
    echo "Library isl already installed."
  fi
}

function build_zstd()
{
  # https://facebook.github.io/zstd/
  # https://github.com/facebook/zstd/releases
  # https://github.com/facebook/zstd/archive/v1.4.4.tar.gz

  # https://archlinuxarm.org/packages/aarch64/zstd/files/PKGBUILD

  # 5 Nov 2019 "1.4.4"
  # 3 Mar 2021 "1.4.9"

  local zstd_version="$1"

  # The folder name as resulted after being extracted from the archive.
  local zstd_src_folder_name="zstd-${zstd_version}"

  local zstd_archive="${zstd_src_folder_name}.tar.gz"

  # GitHub release archive.
  local zstd_github_archive="v${zstd_version}.tar.gz"

  local zstd_url="https://github.com/facebook/zstd/archive/${zstd_github_archive}"

  # The folder name for build, licenses, etc.
  local zstd_folder_name="${zstd_src_folder_name}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${zstd_folder_name}"

  local zstd_stamp_file_path="${STAMPS_FOLDER_PATH}/stamp-${zstd_folder_name}-installed"
  if [ ! -f "${zstd_stamp_file_path}" ]
  then

    cd "${SOURCES_FOLDER_PATH}"

    download_and_extract "${zstd_url}" "${zstd_archive}" \
      "${zstd_src_folder_name}"

    (
      mkdir -pv "${LIBS_BUILD_FOLDER_PATH}/${zstd_folder_name}"
      cd "${LIBS_BUILD_FOLDER_PATH}/${zstd_folder_name}"

      xbb_activate_installed_dev

      CPPFLAGS="${XBB_CPPFLAGS}"
      CFLAGS="${XBB_CFLAGS_NO_W}"
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"

      LDFLAGS="${XBB_LDFLAGS_LIB}"
      if [ "${TARGET_PLATFORM}" == "linux" ]
      then
        LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
      fi      

      export CPPFLAGS
      export CFLAGS
      export CXXFLAGS
      export LDFLAGS

      local build_type
      if [ "${IS_DEBUG}" == "y" ]
      then
        build_type=Debug
      else
        build_type=Release
      fi

      if [ ! -f "CMakeCache.txt" ]
      then 
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running zstd cmake..."
        
          config_options=()

          config_options+=("-LH")
          config_options+=("-G" "Ninja")

          config_options+=("-DCMAKE_INSTALL_PREFIX=${LIBS_INSTALL_FOLDER_PATH}")
          config_options+=("-DZSTD_BUILD_PROGRAMS=OFF")

          if [ "${WITH_TESTS}" == "y" ]
          then
            config_options+=("-DZSTD_BUILD_TESTS=ON")
          fi

          run_verbose cmake \
            "${config_options[@]}" \
            \
            "${SOURCES_FOLDER_PATH}/${zstd_src_folder_name}/build/cmake"
            
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${zstd_folder_name}/zstd-output.txt"
      fi

      (
        echo
        echo "Running zstd build..."

        run_verbose cmake \
          --build . \
          --parallel ${JOBS} \
          --config "${build_type}" \

        if [ "${WITH_TESTS}" == "y" ]
        then
          run_verbose ctest \
            -V \

        fi

        (
          # The install procedure runs some resulted exxecutables, which require
          # the libssl and libcrypt libraries from XBB.
          # xbb_activate_libs

          echo
          echo "Running zstd install..."

          run_verbose cmake \
            --build . \
            --config "${build_type}" \
            -- \
            install

        )
      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${zstd_folder_name}/build-output.txt"

      copy_license \
        "${SOURCES_FOLDER_PATH}/${zstd_src_folder_name}" \
        "${zstd_folder_name}"

      (
        cd "${LIBS_BUILD_FOLDER_PATH}"

        copy_cmake_logs "${zstd_folder_name}"
      )

    )
    touch "${zstd_stamp_file_path}"

  else
    echo "Library zstd already installed."
  fi
}

# -----------------------------------------------------------------------------

function build_libiconv()
{
  # https://www.gnu.org/software/libiconv/
  # https://ftp.gnu.org/pub/gnu/libiconv/

  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=libiconv

  # 2011-08-07 1.14"
  # 2017-02-02 "1.15"
  # 2019-04-26 "1.16"

  local libiconv_version="$1"

  local libiconv_src_folder_name="libiconv-${libiconv_version}"

  local libiconv_archive="${libiconv_src_folder_name}.tar.gz"
  local libiconv_url="https://ftp.gnu.org/pub/gnu/libiconv/${libiconv_archive}"

  local libiconv_folder_name="${libiconv_src_folder_name}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${libiconv_folder_name}"

  local libiconv_stamp_file_path="${STAMPS_FOLDER_PATH}/stamp-${libiconv_folder_name}-installed"
  if [ ! -f "${libiconv_stamp_file_path}" ]
  then

    cd "${SOURCES_FOLDER_PATH}"

    download_and_extract "${libiconv_url}" "${libiconv_archive}" \
      "${libiconv_src_folder_name}"

    (
      mkdir -pv "${LIBS_BUILD_FOLDER_PATH}/${libiconv_folder_name}"
      cd "${LIBS_BUILD_FOLDER_PATH}/${libiconv_folder_name}"

      xbb_activate_installed_dev

      CPPFLAGS="${XBB_CPPFLAGS}"
      # -fgnu89-inline fixes "undefined reference to `aliases2_lookup'"
      #  https://savannah.gnu.org/bugs/?47953
      CFLAGS="${XBB_CFLAGS_NO_W} -fgnu89-inline"
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"

      LDFLAGS="${XBB_LDFLAGS_LIB}"
      if [ "${TARGET_PLATFORM}" == "linux" ]
      then
        LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
      fi      

      export CPPFLAGS
      export CFLAGS
      export CXXFLAGS
      export LDFLAGS

      if [ ! -f "config.status" ]
      then 
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running libiconv configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${libiconv_src_folder_name}/configure" --help
          fi

          config_options=()

          config_options+=("--prefix=${LIBS_INSTALL_FOLDER_PATH}")
            
          config_options+=("--build=${BUILD}")
          config_options+=("--host=${HOST}")
          config_options+=("--target=${TARGET}")

          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${libiconv_src_folder_name}/configure" \
            "${config_options[@]}"

          cp "config.log" "${LOGS_FOLDER_PATH}/${libiconv_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${libiconv_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running libiconv make..."

        # Build.
        run_verbose make -j ${JOBS}

        if [ "${WITH_TESTS}" == "y" ]
        then
          run_verbose make -j1 check
        fi

        if [ "${WITH_STRIP}" == "y" ]
        then
          run_verbose make install-strip
        else
          run_verbose make install
        fi

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${libiconv_folder_name}/make-output.txt"

      copy_license \
        "${SOURCES_FOLDER_PATH}/${libiconv_src_folder_name}" \
        "${libiconv_folder_name}"

    )

    touch "${libiconv_stamp_file_path}"

  else
    echo "Library libiconv already installed."
  fi
}

# -----------------------------------------------------------------------------

# Could not make it work on Windows.
function build_ncurses()
{
  # https://invisible-island.net/ncurses/
  # https://invisible-mirror.net/archives/ncurses/
  # https://invisible-mirror.net/archives/ncurses/ncurses-6.2.tar.gz

  # depends=(glibc gcc-libs)
  # https://archlinuxarm.org/packages/aarch64/ncurses/files/PKGBUILD
  # http://deb.debian.org/debian/pool/main/n/ncurses/ncurses_6.1+20181013.orig.tar.gz.asc

  # https://github.com/msys2/MINGW-packages/blob/master/mingw-w64-ncurses/PKGBUILD
  # https://github.com/msys2/MINGW-packages/blob/master/mingw-w64-ncurses/001-use-libsystre.patch
  # https://github.com/msys2/MSYS2-packages/blob/master/ncurses/PKGBUILD

  # _4421.c:1364:15: error: expected ‘)’ before ‘int’
  # ../include/curses.h:1906:56: note: in definition of macro ‘mouse_trafo’
  # 1906 | #define mouse_trafo(y,x,to_screen) wmouse_trafo(stdscr,y,x,to_screen)

  # 26 Feb 2011, "5.8" # build fails
  # 27 Jan 2018, "5.9" # build fails
  # 27 Jan 2018, "6.1"
  # 12 Feb 2020, "6.2"

  local ncurses_version="$1"
  local ncurses_version_major="$(echo ${ncurses_version} | sed -e 's|\([0-9][0-9]*\)\.\([0-9][0-9]*\)|\1|')"
  local ncurses_version_minor="$(echo ${ncurses_version} | sed -e 's|\([0-9][0-9]*\)\.\([0-9][0-9]*\)|\2|')"

  # The folder name as resulted after being extracted from the archive.
  local ncurses_src_folder_name="ncurses-${ncurses_version}"

  local ncurses_archive="${ncurses_src_folder_name}.tar.gz"
  local ncurses_url="https://invisible-mirror.net/archives/ncurses/${ncurses_archive}"

  # The folder name  for build, licenses, etc.
  local ncurses_folder_name="${ncurses_src_folder_name}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${ncurses_folder_name}"

  local ncurses_stamp_file_path="${STAMPS_FOLDER_PATH}/stamp-${ncurses_folder_name}-installed"
  if [ ! -f "${ncurses_stamp_file_path}" ]
  then

    cd "${SOURCES_FOLDER_PATH}"

    download_and_extract "${ncurses_url}" "${ncurses_archive}" \
      "${ncurses_src_folder_name}"

    (
      mkdir -pv "${LIBS_BUILD_FOLDER_PATH}/${ncurses_folder_name}"
      cd "${LIBS_BUILD_FOLDER_PATH}/${ncurses_folder_name}"

      xbb_activate_installed_dev

      CPPFLAGS="${XBB_CPPFLAGS}"
      CFLAGS="${XBB_CFLAGS_NO_W}"
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"
      LDFLAGS="${XBB_LDFLAGS_LIB}"

      if [ "${TARGET_PLATFORM}" == "linux" ]
      then
        LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
      fi      

      export CPPFLAGS
      export CFLAGS
      export CXXFLAGS
      export LDFLAGS

      if [ ! -f "config.status" ]
      then 
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running ncurses configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${ncurses_src_folder_name}/configure" --help
          fi

          config_options=()

          config_options+=("--prefix=${LIBS_INSTALL_FOLDER_PATH}")
            
          config_options+=("--build=${BUILD}")
          config_options+=("--host=${HOST}")
          config_options+=("--target=${TARGET}")

          # Not yet functional on windows.
          if [ "${TARGET_PLATFORM}" == "win32" ]
          then

            # The build passes, but generally it is not expected to be
            # used on Windows.

            # export PATH_SEPARATOR=";"

            # --with-libtool \
            # /opt/xbb/bin/libtool: line 10548: gcc-8bs: command not found

            # Without --with-pkg-config-libdir= it'll try to write the .pc files in the
            # xbb folder, probbaly by using the dirname of pkg-config.

            config_options+=("--with-build-cc=${NATIVE_CC}")
            config_options+=("--with-build-cflags=${CFLAGS}")
            config_options+=("--with-build-cppflags=${CPPFLAGS}")
            config_options+=("--with-build-ldflags=${LDFLAGS}")
               
            config_options+=("--without-progs")

            # Only for the MinGW port, it provides a way to substitute
            # the low-level terminfo library with different terminal drivers.
            config_options+=("--enable-term-driver")
            
            config_options+=("--disable-termcap")
            config_options+=("--disable-home-terminfo")
            config_options+=("--disable-db-install")

          else

            # Without --with-pkg-config-libdir= it'll try to write the .pc files in the
            # xbb folder, probbaly by using the dirname of pkg-config.

            config_options+=("--with-terminfo-dirs=/etc/terminfo")
            config_options+=("--with-default-terminfo-dir=/etc/terminfo:/lib/terminfo:/usr/share/terminfo")
            config_options+=("--with-gpm")
            config_options+=("--with-versioned-syms")
            config_options+=("--with-xterm-kbs=del")

            config_options+=("--enable-termcap")
            config_options+=("--enable-const")
            config_options+=("--enable-symlinks")

          fi

          config_options+=("--with-shared")
          config_options+=("--with-normal")
          config_options+=("--with-cxx")
          config_options+=("--with-cxx-binding")
          config_options+=("--with-cxx-shared")
          config_options+=("--with-pkg-config-libdir=${LIBS_INSTALL_FOLDER_PATH}/lib/pkgconfig")
          
          # Fails on Linux, with missing _nc_cur_term, which is there.
          config_options+=("--without-pthread")

          config_options+=("--without-ada")
          config_options+=("--without-debug")
          config_options+=("--without-manpages")
          config_options+=("--without-tack")
          config_options+=("--without-tests")

          config_options+=("--enable-pc-files")
          config_options+=("--enable-sp-funcs")
          config_options+=("--enable-ext-colors")
          config_options+=("--enable-interop")

          config_options+=("--disable-lib-suffixes")
          config_options+=("--disable-overwrite")

          NCURSES_DISABLE_WIDEC=${NCURSES_DISABLE_WIDEC:-""}

          if [ "${NCURSES_DISABLE_WIDEC}" == "y" ]
          then
            config_options+=("--disable-widec")
          fi

          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${ncurses_src_folder_name}/configure" \
            "${config_options[@]}"

          cp "config.log" "${LOGS_FOLDER_PATH}/${ncurses_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${ncurses_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running ncurses make..."

        # Build.
        run_verbose make -j ${JOBS}

        # The test-programs are interactive

        # Has no install-strip
        run_verbose make install

        # Expose the library to pkg_config also as `curses`.
        if [ -f "${LIBS_INSTALL_FOLDER_PATH}/lib/pkgconfig/ncurses.pc" ]
        then
          cat "${LIBS_INSTALL_FOLDER_PATH}/lib/pkgconfig/ncurses.pc" | \
            sed -e 's|Name: ncurses|Name: curses|' \
            > "${LIBS_INSTALL_FOLDER_PATH}/lib/pkgconfig/curses.pc"
        fi

        if [ -f "${LIBS_INSTALL_FOLDER_PATH}/lib/pkgconfig/ncurses++.pc" ]
        then
          cat "${LIBS_INSTALL_FOLDER_PATH}/lib/pkgconfig/ncurses++.pc" | \
            sed -e 's|Name: ncurses++|Name: curses++|' \
            > "${LIBS_INSTALL_FOLDER_PATH}/lib/pkgconfig/curses++.pc"
        fi

        if [ -f "${LIBS_INSTALL_FOLDER_PATH}/lib/libncurses.${SHLIB_EXT}" ]
        then
          ln -sfv libncurses.${SHLIB_EXT} "${LIBS_INSTALL_FOLDER_PATH}/lib/libcurses.${SHLIB_EXT}"
        fi

        if [ -f "${LIBS_INSTALL_FOLDER_PATH}/lib/libncurses.a" ]
        then
          ln -sfv libncurses.a "${LIBS_INSTALL_FOLDER_PATH}/lib/libcurses.a"
        fi

        if [ -f "${LIBS_INSTALL_FOLDER_PATH}/lib/libncurses++.${SHLIB_EXT}" ]
        then
          ln -sfv libncurses++.${SHLIB_EXT} "${LIBS_INSTALL_FOLDER_PATH}/lib/libcurses++.${SHLIB_EXT}"
        fi

        if [ -f "${LIBS_INSTALL_FOLDER_PATH}/lib/libncurses++.a" ]
        then
          ln -sfv libncurses++.a "${LIBS_INSTALL_FOLDER_PATH}/lib/libcurses++.a"
        fi

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${ncurses_folder_name}/make-output.txt"

      copy_license \
        "${SOURCES_FOLDER_PATH}/${ncurses_src_folder_name}" \
        "${ncurses_folder_name}"

    )

    touch "${ncurses_stamp_file_path}"

  else
    echo "Library ncurses already installed."
  fi
}

# -----------------------------------------------------------------------------

function build_libffi() 
{
  # http://www.sourceware.org/libffi/
  # ftp://sourceware.org/pub/libffi/
  # https://github.com/libffi/libffi
  # https://github.com/libffi/libffi/archive/v3.2.1.tar.gz

  # https://archlinuxarm.org/packages/aarch64/libffi/files/PKGBUILD
  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=libffi-git
  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=mingw-w64-libffi

  # 12-Nov-2014, "3.2.1"
  # 23 Nov 2019, "3.3"
  # 29 Jun 2021, "3.4.2"

  local libffi_version="$1"

  local libffi_src_folder_name="libffi-${libffi_version}"

  local libffi_archive="${libffi_src_folder_name}.tar.gz"
  # local libffi_url="ftp://sourceware.org/pub/libffi/${libffi_archive}"
  # GitHub release archive.
  local libffi_url="https://github.com/libffi/libffi/archive/v${libffi_version}.tar.gz"

  local libffi_folder_name="${libffi_src_folder_name}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${libffi_folder_name}"

  local libffi_stamp_file_path="${INSTALL_FOLDER_PATH}/stamp-${libffi_folder_name}-installed"
  if [ ! -f "${libffi_stamp_file_path}" ]
  then

    cd "${SOURCES_FOLDER_PATH}"

    download_and_extract "${libffi_url}" "${libffi_archive}" \
      "${libffi_src_folder_name}"

    (
      if [ ! -x "${SOURCES_FOLDER_PATH}/${libffi_src_folder_name}/configure" ]
      then

        cd "${SOURCES_FOLDER_PATH}/${libffi_src_folder_name}"
        
        xbb_activate_installed_dev

        run_verbose bash ${DEBUG} "autogen.sh"

      fi
    ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${libffi_folder_name}/autogen-output.txt"

    (
      mkdir -pv "${LIBS_BUILD_FOLDER_PATH}/${libffi_folder_name}"
      cd "${LIBS_BUILD_FOLDER_PATH}/${libffi_folder_name}"

      xbb_activate_installed_dev

      CPPFLAGS="${XBB_CPPFLAGS}"
      CFLAGS="${XBB_CFLAGS_NO_W}"
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"
      LDFLAGS="${XBB_LDFLAGS_LIB}"

      if [ "${TARGET_PLATFORM}" == "linux" ]
      then
        LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
      fi      

      export CPPFLAGS
      export CFLAGS
      export CXXFLAGS
      export LDFLAGS

      if [ ! -f "config.status" ]
      then 
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running libffi configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${libffi_src_folder_name}/configure" --help
          fi

          config_options=()

          config_options+=("--prefix=${LIBS_INSTALL_FOLDER_PATH}")
            
          config_options+=("--build=${BUILD}")
          config_options+=("--host=${HOST}")
          config_options+=("--target=${TARGET}")

          config_options+=("--enable-pax_emutramp")

          # --enable-pax_emutramp is inspired by AUR
          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${libffi_src_folder_name}/configure" \
            "${config_options[@]}"

          cp "config.log" "${LOGS_FOLDER_PATH}/${libffi_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${libffi_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running libffi make..."

        # Build.
        run_verbose make -j ${JOBS}

        if [ "${WITH_TESTS}" == "y" ]
        then
          run_verbose make -j1 check
        fi

        if [ "${WITH_STRIP}" == "y" ]
        then
          run_verbose make install-strip
        else
          run_verbose make install
        fi

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${libffi_folder_name}/make-output.txt"

      copy_license \
        "${SOURCES_FOLDER_PATH}/${libffi_src_folder_name}" \
        "${libffi_folder_name}"

    )

    touch "${libffi_stamp_file_path}"

  else
    echo "Library libffi already installed."
  fi
}

# -----------------------------------------------------------------------------

function build_gettext() 
{
  # https://www.gnu.org/software/gettext/
  # http://ftp.gnu.org/pub/gnu/gettext/

  # https://archlinuxarm.org/packages/aarch64/gettext/files/PKGBUILD
  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=gettext-git
  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=mingw-w64-gettext

  # 2015-07-14 "0.19.5.1"
  # 2016-06-11 "0.19.8.1"
  # 2020-04-14 "0.20.2"
  # 2020-07-26 "0.21"

  local gettext_version="$1"

  local gettext_src_folder_name="gettext-${gettext_version}"

  local gettext_archive="${gettext_src_folder_name}.tar.gz"
  local gettext_url="http://ftp.gnu.org/pub/gnu/gettext/${gettext_archive}"

  local gettext_folder_name="${gettext_src_folder_name}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${gettext_folder_name}"

  local gettext_stamp_file_path="${INSTALL_FOLDER_PATH}/stamp-${gettext_folder_name}-installed"
  if [ ! -f "${gettext_stamp_file_path}" ]
  then

    cd "${SOURCES_FOLDER_PATH}"

    download_and_extract "${gettext_url}" "${gettext_archive}" \
      "${gettext_src_folder_name}"

    (
      mkdir -pv "${LIBS_BUILD_FOLDER_PATH}/${gettext_folder_name}"
      cd "${LIBS_BUILD_FOLDER_PATH}/${gettext_folder_name}"

      xbb_activate_installed_dev

      CPPFLAGS="${XBB_CPPFLAGS}"
      CFLAGS="${XBB_CFLAGS_NO_W}"      
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"
      LDFLAGS="${XBB_LDFLAGS_LIB}"

      if [ "${TARGET_PLATFORM}" == "linux" ]
      then
        LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
      fi      

      export CPPFLAGS
      export CFLAGS
      export CXXFLAGS
      export LDFLAGS

      if [ ! -f "config.status" ]
      then 
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running gettext configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${gettext_src_folder_name}/gettext-runtime/configure" --help
          fi

          # Build only the /gettext-runtime folder, attempts to build
          # the full package fail with a CXX='no' problem.
          config_options=()

          config_options+=("--prefix=${LIBS_INSTALL_FOLDER_PATH}")
            
          config_options+=("--build=${BUILD}")
          config_options+=("--host=${HOST}")
          config_options+=("--target=${TARGET}")

          if [ "${TARGET_PLATFORM}" == "win32" ]
          then
            config_options+=("--enable-threads=windows")
            config_options+=("--with-gnu-ld")
          elif [ "${TARGET_PLATFORM}" == "linux" ]
          then
            config_options+=("--enable-threads=posix")
            config_options+=("--with-gnu-ld")
          elif [ "${TARGET_PLATFORM}" == "darwin" ]
          then
            config_options+=("--enable-threads=posix")
          fi

          config_options+=("--disable-installed-tests")
          config_options+=("--disable-always-build-tests")
          # config_options+=("--enable-nls")
          config_options+=("--disable-java")
          config_options+=("--disable-native-java")
          config_options+=("--disable-c++")
          config_options+=("--disable-libasprintf")

          #  --enable-nls needed to include libintl
          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${gettext_src_folder_name}/gettext-runtime/configure" \
            "${config_options[@]}"

          cp "config.log" "${LOGS_FOLDER_PATH}/${gettext_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${gettext_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running gettext make..."

        # Build.
        run_verbose make -j ${JOBS}

        if [ "${WITH_TESTS}" == "y" ]
        then
          make -j1 check # || true
        fi

        if [ "${WITH_STRIP}" == "y" ]
        then
          run_verbose make install-strip
        else
          run_verbose make install
        fi

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${gettext_folder_name}/make-output.txt"

      copy_license \
        "${SOURCES_FOLDER_PATH}/${gettext_src_folder_name}" \
        "${gettext_folder_name}"

    )

    touch "${gettext_stamp_file_path}"

  else
    echo "Library gettext already installed."
  fi
}

# -----------------------------------------------------------------------------

function build_libelf()
{
  # https://sourceware.org/elfutils/
  # ftp://sourceware.org/pub/elfutils/
  # ftp://sourceware.org/pub/elfutils//0.178/elfutils-0.178.tar.bz2

  # https://archlinuxarm.org/packages/aarch64/libelf/files/PKGBUILD

  # libelf_version="0.8.13" (deprecated)
  # 26 Nov 2019, 0.178
  # 2020-03-30, 0.179
  # 2020-06-11, 0.180
  # 2020-09-08, 0.181
  # 2020-10-31, 0.182
  # 2021-02-07, "0.183"
  # 2021-05-10, "0.184"

  local libelf_version="$1"

  local libelf_src_folder_name="libelf-${libelf_version}"
  local libelf_archive="${libelf_src_folder_name}.tar.gz"

  # local libelf_url="http://www.mr511.de/software/${libelf_archive}"
  # The original site seems unavailable, use a mirror.
  local libelf_url="https://fossies.org/linux/misc/old/${libelf_archive}"

  local libelf_folder_name="${libelf_src_folder_name}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${libelf_folder_name}"

  local libelf_stamp_file_path="${INSTALL_FOLDER_PATH}/stamp-${libelf_folder_name}-installed"
  if [ ! -f "${libelf_stamp_file_path}" ]
  then

    cd "${SOURCES_FOLDER_PATH}"

    download_and_extract "${libelf_url}" "${libelf_archive}" \
      "${libelf_src_folder_name}"

    (
      mkdir -pv "${LIBS_BUILD_FOLDER_PATH}/${libelf_folder_name}"
      cd "${LIBS_BUILD_FOLDER_PATH}/${libelf_folder_name}"

      xbb_activate_installed_dev

      CPPFLAGS="${XBB_CPPFLAGS}"
      CFLAGS="${XBB_CFLAGS_NO_W}"
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"
      LDFLAGS="${XBB_LDFLAGS_LIB}"

      if [ "${TARGET_PLATFORM}" == "linux" ]
      then
        LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
      fi      

      export CPPFLAGS
      export CFLAGS
      export CXXFLAGS
      export LDFLAGS

      if [ ! -f "config.status" ]
      then 
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running libelf configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${libelf_src_folder_name}/configure" --help
          fi

          config_options=()

          config_options+=("--prefix=${LIBS_INSTALL_FOLDER_PATH}")
            
          config_options+=("--build=${BUILD}")
          config_options+=("--host=${HOST}")
          config_options+=("--target=${TARGET}")

          # config_options+=("--disable-nls")

          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${libelf_src_folder_name}/configure" \
            "${config_options[@]}"

          cp "config.log" "${LOGS_FOLDER_PATH}/${libelf_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${libelf_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running libelf make..."

        # Build.
        run_verbose make -j ${JOBS}

        if [ "${WITH_TESTS}" == "y" ]
        then
          run_verbose make -j1 check
        fi

        if [ "${WITH_STRIP}" == "y" ]
        then
          run_verbose make install-strip
        else
          run_verbose make install
        fi

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${libelf_folder_name}/make-output.txt"

      copy_license \
        "${SOURCES_FOLDER_PATH}/${libelf_src_folder_name}" \
        "${libelf_folder_name}"

    )

    touch "${libelf_stamp_file_path}"

  else
    echo "Library libelf already installed."
  fi
}

# -----------------------------------------------------------------------------

function build_expat()
{
  # https://libexpat.github.io
  # https://github.com/libexpat/libexpat/releases

  # https://archlinuxarm.org/packages/aarch64/expat/files/PKGBUILD
  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=expat-git

  # Oct 21, 2017 "2.1.1"
  # Nov 1, 2017 "2.2.5"
  # 26 Sep 2019 "2.2.9"
  # 3 Oct 2020, "2.2.10"
  # 25 Mar 2021 "2.3.0"

  local expat_version="$1"

  local expat_src_folder_name="expat-${expat_version}"
  local expat_archive="${expat_src_folder_name}.tar.bz2"
  if [[ "${expat_version}" =~ 2\.0\.* ]]
  then
    expat_archive="${expat_src_folder_name}.tar.gz"
  fi
  
  local expat_release="R_$(echo ${expat_version} | sed -e 's|[.]|_|g')"
  local expat_url="https://github.com/libexpat/libexpat/releases/download/${expat_release}/${expat_archive}"

  local expat_folder_name="${expat_src_folder_name}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${expat_folder_name}"

  local expat_stamp_file_path="${INSTALL_FOLDER_PATH}/stamp-${expat_folder_name}-installed"
  if [ ! -f "${expat_stamp_file_path}" ]
  then

    cd "${SOURCES_FOLDER_PATH}"

    download_and_extract "${expat_url}" "${expat_archive}" \
      "${expat_src_folder_name}"

    (
      mkdir -pv "${LIBS_BUILD_FOLDER_PATH}/${expat_folder_name}"
      cd "${LIBS_BUILD_FOLDER_PATH}/${expat_folder_name}"

      xbb_activate_installed_dev

      CPPFLAGS="${XBB_CPPFLAGS}"
      CFLAGS="${XBB_CFLAGS_NO_W}"
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"
      LDFLAGS="${XBB_LDFLAGS_LIB}"

      if [ "${TARGET_PLATFORM}" == "linux" ]
      then
        LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
      fi      

      export CPPFLAGS
      export CFLAGS
      export CXXFLAGS
      export LDFLAGS

      if [ ! -f "config.status" ]
      then 
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running expat configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${expat_src_folder_name}/configure" --help
          fi

          config_options=()

          config_options+=("--prefix=${LIBS_INSTALL_FOLDER_PATH}")
            
          config_options+=("--build=${BUILD}")
          config_options+=("--host=${HOST}")
          config_options+=("--target=${TARGET}")

          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${expat_src_folder_name}/configure" \
            "${config_options[@]}"
            
          cp "config.log" "${LOGS_FOLDER_PATH}/${expat_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${expat_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running expat make..."

        # Build.
        run_verbose make -j ${JOBS}

        if [ "${WITH_TESTS}" == "y" ]
        then
          run_verbose make -j1 check
        fi

        # Has no install-strip
        run_verbose make install

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${expat_folder_name}/make-output.txt"

      copy_license \
        "${SOURCES_FOLDER_PATH}/${expat_src_folder_name}" \
        "${expat_folder_name}"

    )

    touch "${expat_stamp_file_path}"

  else
    echo "Library expat already installed."
  fi
}


# -----------------------------------------------------------------------------

function build_xz()
{
  # https://tukaani.org/xz/
  # https://sourceforge.net/projects/lzmautils/files/

  # https://archlinuxarm.org/packages/aarch64/xz/files/PKGBUILD
  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=xz-git

  # 2016-12-30 "5.2.3"
  # 2018-04-29 "5.2.4"
  # 2020-03-17 "5.2.5"

  local xz_version="$1"

  local xz_src_folder_name="xz-${xz_version}"
  local xz_archive="${xz_src_folder_name}.tar.xz"
  local xz_url="https://sourceforge.net/projects/lzmautils/files/${xz_archive}"

  local xz_folder_name="${xz_src_folder_name}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${xz_folder_name}"

  local xz_stamp_file_path="${INSTALL_FOLDER_PATH}/stamp-${xz_folder_name}-installed"
  if [ ! -f "${xz_stamp_file_path}" ]
  then

    cd "${SOURCES_FOLDER_PATH}"

    download_and_extract "${xz_url}" "${xz_archive}" \
      "${xz_src_folder_name}"

    (
      mkdir -pv "${LIBS_BUILD_FOLDER_PATH}/${xz_folder_name}"
      cd "${LIBS_BUILD_FOLDER_PATH}/${xz_folder_name}"

      xbb_activate_installed_dev

      CPPFLAGS="${XBB_CPPFLAGS}"
      CFLAGS="${XBB_CFLAGS_NO_W}"
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"
      LDFLAGS="${XBB_LDFLAGS_LIB}"

      if [ "${TARGET_PLATFORM}" == "linux" ]
      then
        LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
      fi      

      export CPPFLAGS
      export CFLAGS
      export CXXFLAGS
      export LDFLAGS

      if [ ! -f "config.status" ]
      then 
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running xz configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${xz_src_folder_name}/configure" --help
          fi

          config_options=()

          config_options+=("--prefix=${LIBS_INSTALL_FOLDER_PATH}")
            
          config_options+=("--build=${BUILD}")
          config_options+=("--host=${HOST}")
          config_options+=("--target=${TARGET}")

          # config_options+=("--disable-nls")

          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${xz_src_folder_name}/configure" \
            "${config_options[@]}"

          cp "config.log" "${LOGS_FOLDER_PATH}/${xz_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${xz_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running xz make..."

        # Build.
        run_verbose make -j ${JOBS}

        if [ "${WITH_TESTS}" == "y" ]
        then
          run_verbose make -j1 check
        fi

        if [ "${WITH_STRIP}" == "y" ]
        then
          run_verbose make install-strip
        else
          run_verbose make install
        fi

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${xz_folder_name}/make-output.txt"

      copy_license \
        "${SOURCES_FOLDER_PATH}/${xz_src_folder_name}" \
        "${xz_folder_name}"

    )

    touch "${xz_stamp_file_path}"

  else
    echo "Library xz already installed."
  fi
}

# -----------------------------------------------------------------------------

function build_gpm()
{
  # General purpose mouse. Used by ncurses.
  # https://www.nico.schottelius.org/software/gpm/
  # https://github.com/telmich/gpm
  # https://github.com/telmich/gpm/releases/tag/1.20.7
  # https://github.com/telmich/gpm/archive/1.20.7.tar.gz

  # https://archlinuxarm.org/packages/aarch64/gpm/files/PKGBUILD

  # 27 Oct 2012 "1.20.7"

  local gpm_version="$1"

  local gpm_src_folder_name="gpm-${gpm_version}"
  local gpm_archive="${gpm_src_folder_name}.tar.gz"
  local gpm_github_archive="${gpm_version}.tar.gz"
  
  local gpm_url="https://github.com/telmich/gpm/archive/${gpm_github_archive}"

  local gpm_folder_name="${gpm_src_folder_name}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${gpm_folder_name}"

  local gpm_stamp_file_path="${INSTALL_FOLDER_PATH}/stamp-${gpm_folder_name}-installed"
  if [ ! -f "${gpm_stamp_file_path}" ]
  then

    echo
    echo "gmp in-source building"

    cd "${LIBS_BUILD_FOLDER_PATH}"

    if [ ! -d "${LIBS_BUILD_FOLDER_PATH}/${gpm_folder_name}" ]
    then
      cd "${LIBS_BUILD_FOLDER_PATH}"

      download_and_extract "${gpm_url}" "${gpm_archive}" \
        "${gpm_src_folder_name}"

      if [ "${gpm_src_folder_name}" != "${gpm_folder_name}" ]
      then
        mv -v "${gpm_src_folder_name}" "${gpm_folder_name}"
      fi
    fi

    (
      cd "${LIBS_BUILD_FOLDER_PATH}/${gpm_folder_name}"
      if [ ! -f "stamp-autogen" ]
      then

        run_verbose bash ${DEBUG} "autogen.sh"

        touch "stamp-autogen"
      fi
    ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${gpm_folder_name}/autogen-output.txt"

    (
      cd "${LIBS_BUILD_FOLDER_PATH}/${gpm_folder_name}"

      xbb_activate_installed_dev

      CPPFLAGS="${XBB_CPPFLAGS} -I${LIBS_BUILD_FOLDER_PATH}/${gpm_folder_name}/src/headers"
      CFLAGS="${XBB_CFLAGS_NO_W}"
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"
      LDFLAGS="${XBB_LDFLAGS_LIB}"

      if [ "${TARGET_PLATFORM}" == "linux" ]
      then
        LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
        LDFLAGS+=" -Wl,--allow-multiple-definition"
      fi      

      export CPPFLAGS
      export CFLAGS
      export CXXFLAGS
      export LDFLAGS

      if [ ! -f "config.status" ]
      then 
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running gpm configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "configure" --help
          fi

          config_options=()

          config_options+=("--prefix=${LIBS_INSTALL_FOLDER_PATH}")
            
          config_options+=("--build=${BUILD}")
          config_options+=("--host=${HOST}")
          config_options+=("--target=${TARGET}")

          # config_options+=("--with-pic")

          run_verbose bash ${DEBUG} "configure" \
            "${config_options[@]}"
            
          cp "config.log" "${LOGS_FOLDER_PATH}/${gpm_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${gpm_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running gpm make..."

        # Build.
        run_verbose make -j ${JOBS}

        if [ "${WITH_TESTS}" == "y" ]
        then
          run_verbose make -j1 check
        fi

        if [ "${WITH_STRIP}" == "y" ]
        then
          run_verbose make install-strip
        else
          run_verbose make install
        fi

        if [ "${TARGET_PLATFORM}" == "linux" ]
        then
          (
            mkdir -pv "${APP_PREFIX}/bin"
            cd "${APP_PREFIX}/bin"

            # Manual copy, since it is not refered in the elf.
            cp -v "${LIBS_INSTALL_FOLDER_PATH}/lib/libgpm.so.2.1.0" .
            rm -f "libgpm.so.2"
            ln -s -v "libgpm.so.2.1.0" "libgpm.so.2"
          )
        fi

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${gpm_folder_name}/make-output.txt"

      copy_license \
        "${LIBS_BUILD_FOLDER_PATH}/${gpm_folder_name}" \
        "${gpm_folder_name}"
    )

    touch "${gpm_stamp_file_path}"

  else
    echo "Library gpm already installed."
  fi
}

# -----------------------------------------------------------------------------

function build_libmpdec()
{
  # http://www.bytereef.org/mpdecimal/index.html
  # https://www.bytereef.org/mpdecimal/download.html
  # https://www.bytereef.org/software/mpdecimal/releases/mpdecimal-2.4.2.tar.gz

  # https://archlinuxarm.org/packages/aarch64/mpdecimal/files/PKGBUILD

  # 2016-02-28, "2.4.2"
  # 2020-06-28, "2.5.0"
  # 2021-01-28, "2.5.1"

  local libmpdec_version="$1"

  local libmpdec_src_folder_name="mpdecimal-${libmpdec_version}"

  local libmpdec_archive="${libmpdec_src_folder_name}.tar.gz"
  local libmpdec_url="https://www.bytereef.org/software/mpdecimal/releases/${libmpdec_archive}"

  local libmpdec_folder_name="${libmpdec_src_folder_name}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${libmpdec_folder_name}"

  local libmpdec_stamp_file_path="${STAMPS_FOLDER_PATH}/stamp-${libmpdec_folder_name}-installed"
  if [ ! -f "${libmpdec_stamp_file_path}" ]
  then

    echo
    echo "libmpdec in-source building"

    if [ ! -d "${LIBS_BUILD_FOLDER_PATH}/${libmpdec_folder_name}" ]
    then
      cd "${LIBS_BUILD_FOLDER_PATH}"

      download_and_extract "${libmpdec_url}" "${libmpdec_archive}" \
        "${libmpdec_src_folder_name}"

      if [ "${libmpdec_src_folder_name}" != "${libmpdec_folder_name}" ]
      then
        mv -v "${libmpdec_src_folder_name}" "${libmpdec_folder_name}"
      fi
    fi

    (
      cd "${LIBS_BUILD_FOLDER_PATH}/${libmpdec_src_folder_name}"

      xbb_activate_installed_dev

      CPPFLAGS="${XBB_CPPFLAGS}"
      CFLAGS="${XBB_CFLAGS_NO_W}"
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"
      LDFLAGS="${XBB_LDFLAGS_LIB}"

      if [ "${TARGET_PLATFORM}" == "linux" ]
      then
        LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
      fi      

      LD=${CC}

      export CPPFLAGS
      export CFLAGS
      export CXXFLAGS
      export LDFLAGS
      export LD

      if [ ! -f "config.status" ]
      then
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running libmpdec configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "configure" --help
          fi

          config_options=()

          config_options+=("--prefix=${LIBS_INSTALL_FOLDER_PATH}")
            
          config_options+=("--build=${BUILD}")
          config_options+=("--host=${HOST}")
          config_options+=("--target=${TARGET}")
            
          # C++ tests fail on Linux.
          # config_options+=("--enable-cxx")
          config_options+=("--disable-cxx")

          run_verbose bash ${DEBUG} "configure" \
            "${config_options[@]}"

          cp "config.log" "${LOGS_FOLDER_PATH}/${libmpdec_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${libmpdec_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running libmpdec make..."

        # Build.
        run_verbose make -j ${JOBS}

        # Has no install-strip
        run_verbose make install

        if [ "${WITH_TESTS}" == "y" ]
        then
          if [ "${TARGET_PLATFORM}" == "linux" ]
          then
            # TODO
            # Fails shared on darwin
            run_verbose make -j1 check
          fi
        fi

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${libmpdec_folder_name}/make-output.txt"

      copy_license \
        "${LIBS_BUILD_FOLDER_PATH}/${libmpdec_folder_name}" \
        "${libmpdec_folder_name}"

    )

    touch "${libmpdec_stamp_file_path}"

  else
    echo "Library libmpdec already installed."
  fi
}

# -----------------------------------------------------------------------------

function build_libxcrypt() 
{
  # Replacement for the old libcrypt.so.1.

  # https://github.com/besser82/libxcrypt
  # https://github.com/besser82/libxcrypt/archive/v4.4.15.tar.gz

  # 26 Jul 2018, "4.1.1"
  # 26 Oct 2018, "4.2.3"
  # 14 Nov 2018, "4.3.4"
  # Requires new autotools.
  # m4/ax_valgrind_check.m4:80: warning: macro `AM_EXTRA_RECURSIVE_TARGETS' not found in library
  # Feb 25 2020, "4.4.15"
  # 23 Aug 2020, "4.4.17"
  # 1 May 2021, "4.4.20"

  local libxcrypt_version="$1"

  local libxcrypt_src_folder_name="libxcrypt-${libxcrypt_version}"

  local libxcrypt_archive="${libxcrypt_src_folder_name}.tar.gz"
  # GitHub release archive.
  local libxcrypt_url="https://github.com/besser82/libxcrypt/archive/v${libxcrypt_version}.tar.gz"

  local libxcrypt_folder_name="${libxcrypt_src_folder_name}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${libxcrypt_folder_name}"

  local libxcrypt_patch_file_path="${BUILD_GIT_PATH}/patches/${libxcrypt_folder_name}.patch"
  local libxcrypt_stamp_file_path="${STAMPS_FOLDER_PATH}/stamp-${libxcrypt_folder_name}-installed"
  if [ ! -f "${libxcrypt_stamp_file_path}" ]
  then

    cd "${SOURCES_FOLDER_PATH}"

    set +e
    download_and_extract "${libxcrypt_url}" "${libxcrypt_archive}" \
      "${libxcrypt_src_folder_name}" "${libxcrypt_patch_file_path}"
    set -e

    if [ ! -x "${SOURCES_FOLDER_PATH}/${libxcrypt_src_folder_name}/configure" ]
    then
      (
        cd "${SOURCES_FOLDER_PATH}/${libxcrypt_src_folder_name}"

        xbb_activate_installed_dev

        if [ -f "autogen.sh" ]
        then
          run_verbose bash ${DEBUG} autogen.sh
        elif [ -f "bootstrap" ]
        then
          run_verbose bash ${DEBUG} bootstrap
        else
          # 
          run_verbose autoreconf -fiv
        fi

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${libxcrypt_folder_name}/autogen-output.txt"

    fi

    (
      mkdir -pv "${LIBS_BUILD_FOLDER_PATH}/${libxcrypt_folder_name}"
      cd "${LIBS_BUILD_FOLDER_PATH}/${libxcrypt_folder_name}"

      xbb_activate_installed_dev

      CPPFLAGS="${XBB_CPPFLAGS}"
      CFLAGS="${XBB_CFLAGS_NO_W}"
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"
      LDFLAGS="${XBB_LDFLAGS_LIB}"

      if [ "${TARGET_PLATFORM}" == "linux" ]
      then
        LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
      fi      

      export CPPFLAGS
      export CFLAGS
      export CXXFLAGS
      export LDFLAGS

      if [ ! -f "config.status" ]
      then
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running libxcrypt configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${libxcrypt_src_folder_name}/configure" --help
          fi

          config_options=()

          config_options+=("--prefix=${LIBS_INSTALL_FOLDER_PATH}")
            
          config_options+=("--build=${BUILD}")
          config_options+=("--host=${HOST}")
          config_options+=("--target=${TARGET}")

          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${libxcrypt_src_folder_name}/configure" \
            "${config_options[@]}"

          # patch_all_libtool_rpath

          cp "config.log" "${LOGS_FOLDER_PATH}/${libxcrypt_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${libxcrypt_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running libxcrypt make..."

        # Build.
        run_verbose make -j ${JOBS}

        # install is not able to rewrite them.
        rm -rfv "${LIBS_INSTALL_FOLDER_PATH}"/lib*/libxcrypt.*
        rm -rfv "${LIBS_INSTALL_FOLDER_PATH}"/lib*/libowcrypt.*
        rm -rfv "${LIBS_INSTALL_FOLDER_PATH}"/lib/pkgconfig/libcrypt.pc

        # make install-strip
        if [ "${WITH_STRIP}" == "y" ]
        then
          run_verbose make install-strip
        else
          run_verbose make install
        fi

        if [ "${WITH_TESTS}" == "y" ]
        then
          if [ "${TARGET_PLATFORM}" == "darwin" ]
          then
            # macOS FAIL: test/symbols-static.sh
            # macOS FAIL: test/symbols-renames.sh
            run_verbose make -j1 check || true
          else
            run_verbose make -j1 check
          fi
        fi

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${libxcrypt_folder_name}/make-output.txt"
    )

    (
      test_libxcrypt
    ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${libxcrypt_folder_name}/test-output.txt"

    touch "${libxcrypt_stamp_file_path}"

  else
    echo "Library libxcrypt already installed."
  fi
}

function test_libxcrypt()
{
  (
    xbb_activate

    echo
    echo "Checking the libxcrypt shared libraries..."

    show_libs "${LIBS_INSTALL_FOLDER_PATH}/lib/libcrypt.${SHLIB_EXT}"
  )
}

# -----------------------------------------------------------------------------

function build_openssl() 
{
  # https://www.openssl.org
  # https://www.openssl.org/source/

  # https://archlinuxarm.org/packages/aarch64/openssl/files/PKGBUILD
  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=openssl-static
  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=openssl-git
  
  # 2017-Nov-02 
  # XBB_OPENSSL_VERSION="1.1.0g"
  # The new version deprecated CRYPTO_set_locking_callback, and yum fails with
  # /usr/lib64/python2.6/site-packages/pycurl.so: undefined symbol: CRYPTO_set_locking_callback

  # 2017-Dec-07, "1.0.2n"
  # 2019-Feb-26, "1.0.2r"
  # 2019-Feb-26, "1.1.1b"
  # 2019-Sep-10, "1.1.1d"
  # 2019-Dec-20, "1.0.2u"
  # 2020-Sep-22, "1.1.1h"
  # 2021-Mar-25, "1.1.1k"

  local openssl_version="$1"
  # Numbers
  local openssl_version_major=$(echo ${openssl_version} | sed -e 's|\([0-9][0-9]*\)\.\([0-9][0-9]*\)\..*|\1|')
  local openssl_version_minor=$(echo ${openssl_version} | sed -e 's|\([0-9][0-9]*\)\.\([0-9][0-9]*\)\..*|\2|')

  local openssl_src_folder_name="openssl-${openssl_version}"

  local openssl_archive="${openssl_src_folder_name}.tar.gz"
  local openssl_url="https://www.openssl.org/source/${openssl_archive}"

  local openssl_folder_name="${openssl_src_folder_name}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${openssl_folder_name}"

  local openssl_stamp_file_path="${STAMPS_FOLDER_PATH}/stamp-${openssl_folder_name}-installed"
  if [ ! -f "${openssl_stamp_file_path}" ]
  then

    echo
    echo "openssl in-source building"

    if [ ! -d "${LIBS_BUILD_FOLDER_PATH}/${openssl_folder_name}" ]
    then
      cd "${LIBS_BUILD_FOLDER_PATH}"

      download_and_extract "${openssl_url}" "${openssl_archive}" \
        "${openssl_src_folder_name}"

      if [ "${openssl_src_folder_name}" != "${openssl_folder_name}" ]
      then
        mv -v "${openssl_src_folder_name}" "${openssl_folder_name}"
      fi
    fi

    (
      cd "${LIBS_BUILD_FOLDER_PATH}/${openssl_folder_name}"

      xbb_activate_installed_dev

      #  -Wno-unused-command-line-argument

      # export CPPFLAGS="${XBB_CPPFLAGS} -I${LIBS_BUILD_FOLDER_PATH}/${openssl_folder_name}/include"
      CPPFLAGS="${XBB_CPPFLAGS}"
      CFLAGS="${XBB_CFLAGS_NO_W}"
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"

      LDFLAGS="${XBB_LDFLAGS_APP_STATIC_GCC}"
      if [ "${TARGET_PLATFORM}" == "linux" ]
      then
        LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
      fi      

      export CPPFLAGS
      export CFLAGS
      export CXXFLAGS
      export LDFLAGS

      if [ ! -f config.stamp ]
      then
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running openssl configure..."

          echo
          if [ "${TARGET_PLATFORM}" == "darwin" ]
          then

            # Older versions do not support the KERNEL_BITS trick and require
            # the separate configurator.

            if [ ${openssl_version_minor} -eq 0 ]
            then

              # This config does not use the standard GNU environment definitions.
              # `Configure` is a Perl script.
              if [ "${IS_DEVELOP}" == "y" ]
              then
                run_verbose "./Configure" --help || true
              fi

              run_verbose "./Configure" "darwin64-x86_64-cc" \
                --prefix="${LIBS_INSTALL_FOLDER_PATH}" \
                \
                --openssldir="${LIBS_INSTALL_FOLDER_PATH}/openssl" \
                shared \
                enable-md2 enable-rc5 enable-tls enable-tls1_3 enable-tls1_2 enable-tls1_1 \
                "${CPPFLAGS} ${CFLAGS} ${LDFLAGS}"

              run_verbose make depend 

            else

              if [ "${IS_DEVELOP}" == "y" ]
              then
                run_verbose "./config" --help
              fi

              export KERNEL_BITS=64
              run_verbose "./config" \
                --prefix="${LIBS_INSTALL_FOLDER_PATH}" \
                \
                --openssldir="${LIBS_INSTALL_FOLDER_PATH}/openssl" \
                shared \
                enable-md2 enable-rc5 enable-tls enable-tls1_3 enable-tls1_2 enable-tls1_1 \
                "${CPPFLAGS} ${CFLAGS} ${LDFLAGS}"

            fi

          else

            config_options=()
            if [ "${TARGET_ARCH}" == "x64" ]
            then
              config_options+=("enable-ec_nistp_64_gcc_128")
            elif [ "${TARGET_ARCH}" == "arm64" ]
            then
              config_options+=("no-afalgeng")
            fi

            set +u

            # undefined reference to EVP_md2
            #  enable-md2 

            # perl, do not start with bash.
            run_verbose "./config" \
              --prefix="${LIBS_INSTALL_FOLDER_PATH}" \
              \
              --openssldir="${LIBS_INSTALL_FOLDER_PATH}/openssl" \
              shared \
              enable-md2 \
              enable-rc5 \
              enable-tls \
              enable-tls1_3 \
              enable-tls1_2 \
              enable-tls1_1 \
              "${config_options[@]}" \
              "-Wa,--noexecstack ${CPPFLAGS} ${CFLAGS} ${LDFLAGS}"

            set -u

            if [ ${openssl_version_minor} -eq 0 ]
            then
              run_verbose make depend 
            fi

          fi

          touch config.stamp

          # cp "configure.log" "${LOGS_FOLDER_PATH}/configure-openssl-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${openssl_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running openssl make..."

        # Build.
        run_verbose make -j ${JOBS}

        run_verbose make install_sw

        mkdir -pv "${LIBS_INSTALL_FOLDER_PATH}/openssl"

        if [ -f "${XBB_FOLDER_PATH}/openssl/cert.pem" ]
        then
          install -v -c -m 644 "${XBB_FOLDER_PATH}/openssl/ca-bundle.crt" "${LIBS_INSTALL_FOLDER_PATH}/openssl"
          install -v -c -m 644 "${XBB_FOLDER_PATH}/openssl/cert.pem" "${LIBS_INSTALL_FOLDER_PATH}/openssl"
        elif [ -f "/private/etc/ssl/cert.pem" ]
        then
          install -v -c -m 644 "/private/etc/ssl/cert.pem" "${LIBS_INSTALL_FOLDER_PATH}/openssl"
        fi

        curl -L http://curl.haxx.se/ca/cacert.pem -o cacert.pem
        install -v -c -m 644 cacert.pem "${LIBS_INSTALL_FOLDER_PATH}/openssl"

        if [ "${WITH_TESTS}" == "y" ]
        then
          run_verbose make -j1 test
        fi

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${openssl_folder_name}/make-output.txt"

      (
        test_openssl
      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${openssl_folder_name}/test-output.txt"
    )

    touch "${openssl_stamp_file_path}"

  else
    echo "Component openssl already installed."
  fi
}

function test_openssl()
{
  (
    xbb_activate_installed_bin

    echo
    echo "Testing if the openssl binaries start properly..."

    run_app "${LIBS_INSTALL_FOLDER_PATH}/bin/openssl" version

    echo
    echo "Checking the openssl shared libraries..."

    show_libs "${LIBS_INSTALL_FOLDER_PATH}/bin/openssl"
    
    if [ -f "${LIBS_INSTALL_FOLDER_PATH}/lib64/libcrypto.${SHLIB_EXT}" ]
    then
      show_libs "${LIBS_INSTALL_FOLDER_PATH}/lib64/libcrypto.${SHLIB_EXT}"
    else
      show_libs "${LIBS_INSTALL_FOLDER_PATH}/lib/libcrypto.${SHLIB_EXT}"
    fi

    if [ -f "${LIBS_INSTALL_FOLDER_PATH}/lib64/libssl.${SHLIB_EXT}" ]
    then
      show_libs "${LIBS_INSTALL_FOLDER_PATH}/lib64/libssl.${SHLIB_EXT}"
    else
      show_libs "${LIBS_INSTALL_FOLDER_PATH}/lib/libssl.${SHLIB_EXT}"
    fi
  )
}

# -----------------------------------------------------------------------------

function build_sqlite()
{
  # https://www.sqlite.org/
  # https://www.sqlite.org/2020/sqlite-src-3330000.zip
  # https://www.sqlite.org/src/tarball/7ebdfa80/SQLite-7ebdfa80.tar.gz

  # https://archlinuxarm.org/packages/aarch64/sqlite/files/PKGBUILD

  # 2020-06-18 "3.32.3" 7ebdfa80
  # 2021-04-19 "3.35.5"

  local sqlite_version="$1"

  local sqlite_commit
  if [ "${sqlite_version}" == "3.32.3" ]
  then
    sqlite_commit="7ebdfa80"
  fi

  local sqlite_src_folder_name="SQLite-${sqlite_commit}"

  local sqlite_archive="${sqlite_src_folder_name}.tar.gz"
  local sqlite_url="https://www.sqlite.org/src/tarball/${sqlite_commit}/SQLite-${sqlite_commit}.tar.gz"

  local sqlite_folder_name="sqlite-${sqlite_version}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${sqlite_folder_name}"

  local sqlite_stamp_file_path="${INSTALL_FOLDER_PATH}/stamp-${sqlite_folder_name}-installed"
  if [ ! -f "${sqlite_stamp_file_path}" ]
  then

    cd "${SOURCES_FOLDER_PATH}"

    download_and_extract "${sqlite_url}" "${sqlite_archive}" \
      "${sqlite_src_folder_name}"

    (
      mkdir -pv "${LIBS_BUILD_FOLDER_PATH}/${sqlite_folder_name}"
      cd "${LIBS_BUILD_FOLDER_PATH}/${sqlite_folder_name}"

      xbb_activate_installed_dev

      CPPFLAGS="${XBB_CPPFLAGS}"
      CFLAGS="${XBB_CFLAGS_NO_W}"
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"
      LDFLAGS="${XBB_LDFLAGS_LIB}"

      if [ "${TARGET_PLATFORM}" == "linux" ]
      then
        LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
      fi      

      export CPPFLAGS
      export CFLAGS
      export CXXFLAGS
      export LDFLAGS

      if [ ! -f "config.status" ]
      then 
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running sqlite configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${sqlite_src_folder_name}/configure" --help
          fi

          config_options=()

          config_options+=("--prefix=${LIBS_INSTALL_FOLDER_PATH}")
            
          config_options+=("--build=${BUILD}")
          config_options+=("--host=${HOST}")
          config_options+=("--target=${TARGET}")
   
          config_options+=("--enable-tcl=no")
          # Fails on macOS & Linux.
          # config_options+=("--disable-tcl")
          # Fail on macOS.
          # config_options+=("--disable-readline")
          # config_options+=("--disable-amalgamation")

          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${sqlite_src_folder_name}/configure" \
            "${config_options[@]}"

          cp "config.log" "${LOGS_FOLDER_PATH}/${sqlite_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${sqlite_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running sqlite make..."

        # Build.
        run_verbose make -j ${JOBS}

        # Fails on Linux. And takes way too long.
        # 2 errors out of 249249 tests on docker Linux 64-bit little-endian
        # !Failures on these tests: oserror-1.4.1 oserror-1.4.2
        if false # [ "${WITH_TESTS}" == "y" ]
        then
          (
            # To access the /opt/xbb/lib/libtcl8.6.so
            xbb_activate_libs

            run_verbose make -j1 quicktest
          )
        fi

        # Has no install-strip
        run_verbose make install

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${sqlite_folder_name}/make-output.txt"

      copy_license \
        "${SOURCES_FOLDER_PATH}/${sqlite_src_folder_name}" \
        "${sqlite_folder_name}"

    )

    touch "${sqlite_stamp_file_path}"

  else
    echo "Library sqlite already installed."
  fi
}

# -----------------------------------------------------------------------------

function build_readline()
{
  # https://tiswww.case.edu/php/chet/readline/rltop.html
  # https://ftp.gnu.org/gnu/readline/
  # https://ftp.gnu.org/gnu/readline/readline-8.0.tar.gz

  # depends=(glibc gcc-libs)
  # https://archlinuxarm.org/packages/aarch64/readline/files/PKGBUILD

  # 2019-01-07, "8.0"
  # 2020-12-06, "8.1"

  local readline_version="$1"
  local readline_version_major="$(echo ${readline_version} | sed -e 's|\([0-9][0-9]*\)\.\([0-9][0-9]*\)|\1|')"
  local readline_version_minor="$(echo ${readline_version} | sed -e 's|\([0-9][0-9]*\)\.\([0-9][0-9]*\)|\2|')"

  # The folder name as resulted after being extracted from the archive.
  local readline_src_folder_name="readline-${readline_version}"

  local readline_archive="${readline_src_folder_name}.tar.gz"
  local readline_url="https://ftp.gnu.org/gnu/readline/${readline_archive}"

  # The folder name  for build, licenses, etc.
  local readline_folder_name="${readline_src_folder_name}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${readline_folder_name}"

  local readline_stamp_file_path="${STAMPS_FOLDER_PATH}/stamp-${readline_folder_name}-installed"
  if [ ! -f "${readline_stamp_file_path}" ]
  then

    cd "${SOURCES_FOLDER_PATH}"

    download_and_extract "${readline_url}" "${readline_archive}" \
      "${readline_src_folder_name}"

    (
      mkdir -pv "${LIBS_BUILD_FOLDER_PATH}/${readline_folder_name}"
      cd "${LIBS_BUILD_FOLDER_PATH}/${readline_folder_name}"

      xbb_activate_installed_dev

      CPPFLAGS="${XBB_CPPFLAGS}"
      CFLAGS="${XBB_CFLAGS_NO_W}"
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"

      LDFLAGS="${XBB_LDFLAGS_LIB}"
      if [ "${TARGET_PLATFORM}" == "linux" ]
      then
        LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
      fi      

      export CPPFLAGS
      export CFLAGS
      export CXXFLAGS
      export LDFLAGS

      if [ ! -f "config.status" ]
      then 
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running readline configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${readline_src_folder_name}/configure" --help
          fi

          config_options=()

          config_options+=("--prefix=${LIBS_INSTALL_FOLDER_PATH}")

          config_options+=("--build=${BUILD}")
          config_options+=("--host=${HOST}")
          config_options+=("--target=${TARGET}")

          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${readline_src_folder_name}/configure" \
            "${config_options[@]}"

          cp "config.log" "${LOGS_FOLDER_PATH}/${readline_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${readline_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running readline make..."

        # Build.
        run_verbose make -j ${JOBS}

        # Has no install-strip
        run_verbose make install

        if [ "${WITH_TESTS}" == "y" ]
        then
          run_verbose make -j1 check
        fi

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${readline_folder_name}/make-output.txt"
    )

    (
      test_readline
    ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${readline_folder_name}/test-output.txt"

    touch "${readline_stamp_file_path}"

  else
    echo "Library readline already installed."
  fi
}

function test_readline()
{
  (
    xbb_activate

    echo
    echo "Checking the readline shared libraries..."

    show_libs "${LIBS_INSTALL_FOLDER_PATH}/lib/libreadline.${SHLIB_EXT}"
    show_libs "${LIBS_INSTALL_FOLDER_PATH}/lib/libhistory.${SHLIB_EXT}"
  )
}

# -----------------------------------------------------------------------------

# TODO: add support for dylib
function build_bzip2()
{
  # https://sourceware.org/bzip2/
  # https://sourceware.org/pub/bzip2/
  # https://sourceware.org/pub/bzip2/bzip2-1.0.8.tar.gz

  # https://archlinuxarm.org/packages/aarch64/bzip2/files/PKGBUILD

  # 2019-07-13 "1.0.8"

  local bzip2_version="$1"

  local bzip2_src_folder_name="bzip2-${bzip2_version}"

  local bzip2_archive="${bzip2_src_folder_name}.tar.gz"
  local bzip2_url="https://sourceware.org/pub/bzip2/${bzip2_archive}"

  local bzip2_folder_name="${bzip2_src_folder_name}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${bzip2_folder_name}"

  local bzip2_stamp_file_path="${INSTALL_FOLDER_PATH}/stamp-${bzip2_folder_name}-installed"
  if [ ! -f "${bzip2_stamp_file_path}" ]
  then

    echo
    echo "bzip2 in-source building"

    if [ ! -d "${LIBS_BUILD_FOLDER_PATH}/${bzip2_folder_name}" ]
    then
      cd "${LIBS_BUILD_FOLDER_PATH}"

      download_and_extract "${bzip2_url}" "${bzip2_archive}" \
        "${bzip2_src_folder_name}"

      if [ "${bzip2_src_folder_name}" != "${bzip2_folder_name}" ]
      then
        mv -v "${bzip2_src_folder_name}" "${bzip2_folder_name}"
      fi
    fi

    (
      cd "${LIBS_BUILD_FOLDER_PATH}/${bzip2_folder_name}"

      xbb_activate_installed_dev

      CPPFLAGS="${XBB_CPPFLAGS}"
      # libbz2.a(bzlib.o): relocation R_X86_64_PC32 against symbol `BZ2_crc32Table' can not be used when making a shared object; recompile with -fPIC
      CFLAGS="${XBB_CFLAGS_NO_W} -fPIC"
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"

      LDFLAGS="${XBB_LDFLAGS_LIB}"
      if [ "${TARGET_PLATFORM}" == "linux" ]
      then
        LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
      fi      

      export CPPFLAGS
      export CFLAGS
      export CXXFLAGS
      export LDFLAGS

      (
        if [ "${IS_DEVELOP}" == "y" ]
        then
          env | sort
        fi

        echo
        echo "Running bzip2 make..."

        # Build.
        run_verbose make all -j ${JOBS} \
          PREFIX=${LIBS_INSTALL_FOLDER_PATH} \
          CC=${CC} \
          AR=${AR} \
          RANLIB=${RANLIB} \
          LDFLAGS=${LDFLAGS} \

        run_verbose make install PREFIX=${LIBS_INSTALL_FOLDER_PATH}

        # TODO: add support for creating macOS dylib.
        if [ "${TARGET_PLATFORM}" == "linux" ]
        then
          run_verbose make clean

          run_verbose make all -f Makefile-libbz2_so -j ${JOBS} \
            PREFIX=${LIBS_INSTALL_FOLDER_PATH} \
            CC=${CC} \
            AR=${AR} \
            RANLIB=${RANLIB} \
            LDFLAGS=${LDFLAGS} \

          mkdir -pv "${LIBS_INSTALL_FOLDER_PATH}/lib/"
          install -v -c -m 644 "libbz2.so.${bzip2_version}" "${LIBS_INSTALL_FOLDER_PATH}/lib/"

          (
            cd "${LIBS_INSTALL_FOLDER_PATH}/lib/"

            rm -rfv libbz2.so*
            ln -sv "libbz2.so.${bzip2_version}" libbz2.so.1.0
            ln -sv "libbz2.so.${bzip2_version}" libbz2.so.1
            ln -sv "libbz2.so.${bzip2_version}" libbz2.so
          )
        fi
      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${bzip2_folder_name}/make-output.txt"

      copy_license \
        "${LIBS_BUILD_FOLDER_PATH}/${bzip2_folder_name}" \
        "${bzip2_folder_name}"

    )

    touch "${bzip2_stamp_file_path}"

  else
    echo "Library bzip2 already installed."
  fi
}

# -----------------------------------------------------------------------------

function build_python2() 
{
  # https://www.python.org
  # https://www.python.org/downloads/source/
  # https://www.python.org/ftp/python/
  # https://www.python.org/ftp/python/2.7.18/Python-2.7.18.tar.xz
  
  # https://archlinuxarm.org/packages/aarch64/python/files/PKGBUILD
  # https://git.archlinux.org/svntogit/packages.git/tree/trunk/PKGBUILD?h=packages/python
  # https://git.archlinux.org/svntogit/packages.git/tree/trunk/PKGBUILD?h=packages/python-pip

  # 19-Apr-2020, "2.7.18"

  local python2_version="$1"

  export PYTHON2_VERSION_MAJOR=$(echo ${python2_version} | sed -e 's|\([0-9]\)\..*|\1|')
  export PYTHON2_VERSION_MINOR=$(echo ${python2_version} | sed -e 's|\([0-9]\)\.\([0-9][0-9]*\)\..*|\2|')
  export PYTHON2_VERSION_MAJOR_MINOR=${PYTHON2_VERSION_MAJOR}${PYTHON2_VERSION_MINOR}

  # Used in python27-config.sh.
  export PYTHON2_SRC_FOLDER_NAME="Python-${python2_version}"

  local python2_archive="${PYTHON2_SRC_FOLDER_NAME}.tar.xz"
  local python2_url="https://www.python.org/ftp/python/${python2_version}/${python2_archive}"

  local python2_folder_name="python-${python2_version}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${python2_folder_name}"

  local python2_stamp_file_path="${STAMPS_FOLDER_PATH}/stamp-${python2_folder_name}-installed"
  if [ ! -f "${python2_stamp_file_path}" ]
  then

    cd "${SOURCES_FOLDER_PATH}"

    download_and_extract "${python2_url}" "${python2_archive}" \
      "${PYTHON2_SRC_FOLDER_NAME}"

    (
      mkdir -pv "${LIBS_BUILD_FOLDER_PATH}/${python2_folder_name}"
      cd "${LIBS_BUILD_FOLDER_PATH}/${python2_folder_name}"

      # To pick the new libraries
      xbb_activate_installed_dev

      CPPFLAGS="${XBB_CPPFLAGS} -I${LIBS_INSTALL_FOLDER_PATH}/include/ncurses"
      CFLAGS="${XBB_CFLAGS_NO_W}"
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"

      LDFLAGS="${XBB_LDFLAGS_APP_STATIC_GCC}"

      if [ "${TARGET_PLATFORM}" == "linux" ]
      then
        LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
      fi      

      if [[ "${CC}" =~ gcc* ]]
      then
        # Inspired from Arch; not supported by clang.
        CFLAGS+=" -fno-semantic-interposition"
        CXXFLAGS+=" -fno-semantic-interposition"
        LDFLAGS+=" -fno-semantic-interposition"
      fi

      export CPPFLAGS
      export CFLAGS
      export CXXFLAGS
      export LDFLAGS

      if [ ! -f "config.status" ]
      then
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running python2 configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${PYTHON2_SRC_FOLDER_NAME}/configure" --help
          fi

          # Fail on macOS:
          # --enable-universalsdk
          # --with-lto

          # "... you should not skip tests when using --enable-optimizations as 
          # the data required for profiling is generated by running tests".

          # --enable-optimizations takes too long

          config_options=()
          config_options+=("--prefix=${LIBS_INSTALL_FOLDER_PATH}")

          config_options+=("--with-dbmliborder=gdbm:ndbm")

          config_options+=("--without-ensurepip")
          config_options+=("--without-lto")
          
          # Create the PythonX.Y.so.
          config_options+=("--enable-shared")

          if [ "${TARGET_PLATFORM}" == "darwin" ]
          then
            config_options+=("--enable-unicode=ucs2")
          else
            config_options+=("--enable-unicode=ucs4")
          fi

          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${PYTHON2_SRC_FOLDER_NAME}/configure" \
            "${config_options[@]}"
             
          cp "config.log" "${LOGS_FOLDER_PATH}/${python2_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${python2_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running python2 make..."

        # export LD_RUN_PATH="${LIBS_INSTALL_FOLDER_PATH}/lib"

        # Build.
        run_verbose make -j ${JOBS} # build_all

        run_verbose make altinstall

        # Hundreds of tests, take a lot of time.
        # Many failures.
        if false # [ "${WITH_TESTS}" == "y" ]
        then
          run_verbose make -j1 quicktest
        fi

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${python2_folder_name}/make-output.txt"
    )

    (
      test_python2
    ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${python2_folder_name}/test-output.txt"

    copy_license \
      "${SOURCES_FOLDER_PATH}/${PYTHON2_SRC_FOLDER_NAME}" \
      "${python2_folder_name}"

    touch "${python2_stamp_file_path}"

  else
    echo "Component python2 already installed."
  fi
}


function test_python2()
{
  (
    # xbb_activate_installed_bin

    echo
    echo "Checking the python2 binary shared libraries..."

    show_libs "${LIBS_INSTALL_FOLDER_PATH}/bin/python2.${PYTHON2_VERSION_MINOR}"
    show_libs "${LIBS_INSTALL_FOLDER_PATH}/lib/libpython${PYTHON2_VERSION_MAJOR}.${PYTHON2_VERSION_MINOR}.${SHLIB_EXT}"

    echo
    echo "Testing if the python2 binary starts properly..."

    export LD_LIBRARY_PATH="${LIBS_INSTALL_FOLDER_PATH}/lib"
    run_app "${LIBS_INSTALL_FOLDER_PATH}/bin/python2.${PYTHON2_VERSION_MINOR}" --version

    run_app "${LIBS_INSTALL_FOLDER_PATH}/bin/python2.${PYTHON2_VERSION_MINOR}" -c 'import sys; print(sys.path)'
    run_app "${LIBS_INSTALL_FOLDER_PATH}/bin/python2.${PYTHON2_VERSION_MINOR}" -c 'import sys; print(sys.prefix)'
  )
}


# -----------------------------------------------------------------------------

# Download the Windows Python 2 libraries and headers.
function download_python2_win() 
{
  # https://www.python.org/downloads/release/python-2714/
  # https://www.python.org/ftp/python/2.7.14/python-2.7.14.msi
  # https://www.python.org/ftp/python/2.7.14/python-2.7.14.amd64.msi

  local python2_win_version="$1"

  export PYTHON2_VERSION_MAJOR=$(echo ${python2_win_version} | sed -e 's|\([0-9]\)\..*|\1|')
  export PYTHON2_VERSION_MINOR=$(echo ${python2_win_version} | sed -e 's|\([0-9]\)\.\([0-9][0-9]*\)\..*|\2|')
  export PYTHON2_VERSION_MAJOR_MINOR=${PYTHON2_VERSION_MAJOR}${PYTHON2_VERSION_MINOR}

  local python2_win_pack

  if [ "${TARGET_BITS}" == "32" ]
  then
    PYTHON2_WIN_SRC_FOLDER_NAME="python-${python2_win_version}-embed-win32"
    python2_win_pack="python-${python2_win_version}.msi"
  else
    PYTHON2_WIN_SRC_FOLDER_NAME="python-${python2_win_version}-embed-amd64"
    python2_win_pack="python-${python2_win_version}.amd64.msi"
  fi

  # Used in python27-config.sh.
  export PYTHON2_WIN_SRC_FOLDER_NAME

  local python2_win_url="https://www.python.org/ftp/python/${python2_win_version}/${python2_win_pack}"

  cd "${SOURCES_FOLDER_PATH}"

  download "${python2_win_url}" "${python2_win_pack}"

  (
    if [ ! -d "${PYTHON2_WIN_SRC_FOLDER_NAME}" ]
    then
      cd "${SOURCES_FOLDER_PATH}"

      # Include only the headers and the python library and executable.
      local tmp_path="/tmp/included$$"
      echo '*.h' >"${tmp_path}"
      echo 'python*.dll' >>"${tmp_path}"
      echo 'python*.lib' >>"${tmp_path}"
      7za x -o"${PYTHON2_WIN_SRC_FOLDER_NAME}" "${DOWNLOAD_FOLDER_PATH}/${python2_win_pack}" -i@"${tmp_path}"

      # Patch to disable the macro that renames hypot.
      local patch_path="${BUILD_GIT_PATH}/patches/${PYTHON2_WIN_SRC_FOLDER_NAME}.patch"
      if [ -f "${patch_path}" ]
      then
        (
          cd "${PYTHON2_WIN_SRC_FOLDER_NAME}"
          patch -p0 <"${patch_path}" 
        )
      fi
    else
      echo "Folder ${PYTHON2_WIN_SRC_FOLDER_NAME} already present."
    fi

    echo "Copying python${PYTHON2_VERSION_MAJOR_MINOR}.dll..."
    # From here it'll be copied as dependency.
    mkdir -pv "${LIBS_INSTALL_FOLDER_PATH}/bin/"
    install -v -c -m 644 "${PYTHON2_WIN_SRC_FOLDER_NAME}/python${PYTHON2_VERSION_MAJOR_MINOR}.dll" \
      "${LIBS_INSTALL_FOLDER_PATH}/bin/"

    mkdir -pv "${LIBS_INSTALL_FOLDER_PATH}/lib/"
    install -v -c -m 644 "${PYTHON2_WIN_SRC_FOLDER_NAME}/python${PYTHON2_VERSION_MAJOR_MINOR}.lib" \
      "${LIBS_INSTALL_FOLDER_PATH}/lib/"
  )
}

# -----------------------------------------------------------------------------

# https://stackoverflow.com/questions/44150871/embeded-python3-6-with-mingw-in-c-fail-on-linking

function build_python3() 
{
  # https://www.python.org
  # https://www.python.org/downloads/source/
  # https://www.python.org/ftp/python/
  # https://www.python.org/ftp/python/3.7.3/Python-3.7.3.tar.xz
  
  # https://archlinuxarm.org/packages/aarch64/python/files/PKGBUILD
  # https://git.archlinux.org/svntogit/packages.git/tree/trunk/PKGBUILD?h=packages/python
  # https://git.archlinux.org/svntogit/packages.git/tree/trunk/PKGBUILD?h=packages/python-pip

  # 2018-12-24, "3.7.2"
  # March 25, 2019, "3.7.3"
  # Dec. 18, 2019, "3.8.1"
  # 17-Aug-2020, "3.7.9"
  # 23-Sep-2020, "3.8.6"
  # May 3, 2021 "3.8.10"
  # May 3, 2021 "3.9.5"

  local python3_version="$1"

  PYTHON3_VERSION_MAJOR=$(echo ${python3_version} | sed -e 's|\([0-9]\)\..*|\1|')
  PYTHON3_VERSION_MINOR=$(echo ${python3_version} | sed -e 's|\([0-9]\)\.\([0-9][0-9]*\)\..*|\2|')
  PYTHON3_VERSION_MAJOR_MINOR=${PYTHON3_VERSION_MAJOR}${PYTHON3_VERSION_MINOR}

  PYTHON3_SRC_FOLDER_NAME="Python-${python3_version}"

  local python3_archive="${PYTHON3_SRC_FOLDER_NAME}.tar.xz"
  local python3_url="https://www.python.org/ftp/python/${python3_version}/${python3_archive}"

  local python3_folder_name="python-${python3_version}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${python3_folder_name}"

  local python3_stamp_file_path="${STAMPS_FOLDER_PATH}/stamp-${python3_folder_name}-installed"
  if [ ! -f "${python3_stamp_file_path}" ]
  then

    cd "${SOURCES_FOLDER_PATH}"

    download_and_extract "${python3_url}" "${python3_archive}" \
      "${PYTHON3_SRC_FOLDER_NAME}"

    (
      mkdir -pv "${LIBS_BUILD_FOLDER_PATH}/${python3_folder_name}"
      cd "${LIBS_BUILD_FOLDER_PATH}/${python3_folder_name}"

      # To pick the new libraries
      xbb_activate_installed_dev

      CPPFLAGS="${XBB_CPPFLAGS} -I${LIBS_INSTALL_FOLDER_PATH}/include/ncurses"
      CFLAGS="${XBB_CFLAGS_NO_W}"
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"

      if [ "${TARGET_PLATFORM}" == "darwin" ]
      then
        if [[ "${CC}" =~ gcc* ]]
        then
          # HACK! GCC chokes on dynamic sizes:
          # error: variably modified ‘bytes’ at file scope
          # char bytes[kAuthorizationExternalFormLength];
          # -DkAuthorizationExternalFormLength=32 not working
          export CC=clang
          export CXX=clang++
        fi     
      fi

      LDFLAGS="${XBB_LDFLAGS_APP_STATIC_GCC}"
      if [ "${TARGET_PLATFORM}" == "linux" ]
      then
        LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
      fi      

      if [[ "${CC}" =~ gcc* ]]
      then
        # Inspired from Arch; not supported by clang.
        CFLAGS+=" -fno-semantic-interposition"
        CXXFLAGS+=" -fno-semantic-interposition"
        LDFLAGS+=" -fno-semantic-interposition"
      fi

      export CPPFLAGS
      export CFLAGS
      export CXXFLAGS
      export LDFLAGS

      if [ ! -f "config.status" ]
      then
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running python3 configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${PYTHON3_SRC_FOLDER_NAME}/configure" --help
          fi

          # Fail on macOS:
          # --enable-universalsdk
          # --with-lto

          # "... you should not skip tests when using --enable-optimizations as 
          # the data required for profiling is generated by running tests".

          # --enable-optimizations takes too long

          config_options=()
          config_options+=("--prefix=${LIBS_INSTALL_FOLDER_PATH}")

          config_options+=("--with-universal-archs=${TARGET_BITS}-bit")
          config_options+=("--with-computed-gotos")
          config_options+=("--with-dbmliborder=gdbm:ndbm")

          # Better not, allow configure to choose.
          # config_options+=("--with-system-expat")
          # config_options+=("--with-system-ffi")
          # config_options+=("--with-system-libmpdec")

          # config_options+=("--with-openssl=${INSTALL_FOLDER_PATH}")
          config_options+=("--without-ensurepip")
          config_options+=("--without-lto")
          
          # Create the PythonX.Y.so.
          config_options+=("--enable-shared")

          # config_options+=("--enable-loadable-sqlite-extensions")
          config_options+=("--disable-loadable-sqlite-extensions")

          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${PYTHON3_SRC_FOLDER_NAME}/configure" \
            "${config_options[@]}"
             
          cp "config.log" "${LOGS_FOLDER_PATH}/${python3_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${python3_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running python3 make..."

        # export LD_RUN_PATH="${LIBS_INSTALL_FOLDER_PATH}/lib"

        # Build.
        run_verbose make -j ${JOBS} # build_all

        run_verbose make altinstall

        # Hundreds of tests, take a lot of time.
        # Many failures.
        if false # [ "${WITH_TESTS}" == "y" ]
        then
          run_verbose make -j1 quicktest
        fi

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${python3_folder_name}/make-output.txt"
    )

    (
      test_python3
    ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${python3_folder_name}/test-output.txt"

    copy_license \
      "${SOURCES_FOLDER_PATH}/${PYTHON3_SRC_FOLDER_NAME}" \
      "${python3_folder_name}"

    touch "${python3_stamp_file_path}"

  else
    echo "Component python3 already installed."
  fi
}

function test_python3()
{
  (
    # xbb_activate_installed_bin

    echo
    echo "Checking the python3 binary shared libraries..."

    show_libs "${LIBS_INSTALL_FOLDER_PATH}/bin/python3.${PYTHON3_VERSION_MINOR}"
    if [ -f "${LIBS_INSTALL_FOLDER_PATH}/lib/libpython${PYTHON3_VERSION_MAJOR}.${PYTHON3_VERSION_MINOR}m.${SHLIB_EXT}" ]
    then
      show_libs "${LIBS_INSTALL_FOLDER_PATH}/lib/libpython${PYTHON3_VERSION_MAJOR}.${PYTHON3_VERSION_MINOR}m.${SHLIB_EXT}"
    elif [ -f "${LIBS_INSTALL_FOLDER_PATH}/lib/libpython${PYTHON3_VERSION_MAJOR}.${PYTHON3_VERSION_MINOR}.${SHLIB_EXT}" ]
    then
      show_libs "${LIBS_INSTALL_FOLDER_PATH}/lib/libpython${PYTHON3_VERSION_MAJOR}.${PYTHON3_VERSION_MINOR}.${SHLIB_EXT}"
    fi

    echo
    echo "Testing if the python3 binary starts properly..."

    export LD_LIBRARY_PATH="${LIBS_INSTALL_FOLDER_PATH}/lib"
    run_app "${LIBS_INSTALL_FOLDER_PATH}/bin/python3.${PYTHON3_VERSION_MINOR}" --version

    run_app "${LIBS_INSTALL_FOLDER_PATH}/bin/python3.${PYTHON3_VERSION_MINOR}" -c 'import sys; print(sys.path)'
    run_app "${LIBS_INSTALL_FOLDER_PATH}/bin/python3.${PYTHON3_VERSION_MINOR}" -c 'import sys; print(sys.prefix)'
  )
}

# -----------------------------------------------------------------------------

# Download the Windows Python 3 libraries and headers.
function download_python3_win() 
{
  # https://www.python.org/downloads/windows/
  # https://www.python.org/downloads/release/python-372/
  # https://www.python.org/ftp/python/3.7.2/python-3.7.2.post1-embed-win32.zip
  # https://www.python.org/ftp/python/3.7.2/python-3.7.2.post1-embed-amd64.zip
  # https://www.python.org/ftp/python/3.7.2/python-3.7.2.exe
  # https://www.python.org/ftp/python/3.7.2/python-3.7.2-amd64.exe
  # https://www.python.org/ftp/python/3.7.2/Python-3.7.2.tar.xz
  # https://www.python.org/ftp/python/3.7.6/
  # https://www.python.org/ftp/python/3.7.6/python-3.7.6-embed-amd64.zip
  # https://www.python.org/ftp/python/3.7.6/python-3.7.6-embed-win32.zip

  local python3_win_version="$1"

  PYTHON3_VERSION_MAJOR=$(echo ${python3_win_version} | sed -e 's|\([0-9]\)\..*|\1|')
  PYTHON3_VERSION_MINOR=$(echo ${python3_win_version} | sed -e 's|\([0-9]\)\.\([0-9][0-9]*\)\..*|\2|')
  PYTHON3_VERSION_MAJOR_MINOR=${PYTHON3_VERSION_MAJOR}${PYTHON3_VERSION_MINOR}

  # Version 3.7.2 uses a longer name, like python-3.7.2.post1-embed-amd64.zip.
  if [ "${TARGET_BITS}" == "32" ]
  then
    PYTHON3_WIN_SRC_FOLDER_NAME="python-${python3_win_version}-embed-win32"
  else
    PYTHON3_WIN_SRC_FOLDER_NAME="python-${python3_win_version}-embed-amd64"
  fi

  # Used in python3-config.sh
  export PYTHON3_WIN_SRC_FOLDER_NAME

  local python3_win_embed_pack="${PYTHON3_WIN_SRC_FOLDER_NAME}.zip"
  local python3_win_embed_url="https://www.python.org/ftp/python/${python3_win_version}/${python3_win_embed_pack}"

  (
    if [ ! -d "${SOURCES_FOLDER_PATH}/${PYTHON3_WIN_SRC_FOLDER_NAME}" ]
    then
      mkdir -pv "${SOURCES_FOLDER_PATH}/${PYTHON3_WIN_SRC_FOLDER_NAME}"
      cd "${SOURCES_FOLDER_PATH}/${PYTHON3_WIN_SRC_FOLDER_NAME}"

      download_and_extract "${python3_win_embed_url}" "${python3_win_embed_pack}" "${PYTHON3_WIN_SRC_FOLDER_NAME}"
    else
      echo "Folder ${PYTHON3_WIN_SRC_FOLDER_NAME} already present."
    fi
      
    cd "${SOURCES_FOLDER_PATH}/${PYTHON3_WIN_SRC_FOLDER_NAME}"
    echo "Copying python${PYTHON3_VERSION_MAJOR}${PYTHON3_VERSION_MINOR}.dll..."
    # From here it'll be copied as dependency.
    mkdir -pv "${LIBS_INSTALL_FOLDER_PATH}/bin/"
    install -v -c -m 644 "python${PYTHON3_VERSION_MAJOR}.dll" \
      "${LIBS_INSTALL_FOLDER_PATH}/bin/"
    install -v -c -m 644 "python${PYTHON3_VERSION_MAJOR}${PYTHON3_VERSION_MINOR}.dll" \
      "${LIBS_INSTALL_FOLDER_PATH}/bin/"
  )

  export PYTHON3_SRC_FOLDER_NAME="Python-${python3_win_version}"

  local python3_archive="${PYTHON3_SRC_FOLDER_NAME}.tar.xz"
  local python3_url="https://www.python.org/ftp/python/${python3_win_version}/${python3_archive}"

  # The full source is needed for the headers.
  if [ ! -d "${SOURCES_FOLDER_PATH}/${PYTHON3_SRC_FOLDER_NAME}" ]
  then
    cd "${SOURCES_FOLDER_PATH}"

    download_and_extract "${python3_url}" "${python3_archive}" \
      "${PYTHON3_SRC_FOLDER_NAME}"
  fi
}

# -----------------------------------------------------------------------------

# Used by gdb-py3 on Windows. The default paths on Windows are different
# from POSIX.
function add_python3_win_syslibs()
{
  if [ "${TARGET_PLATFORM}" == "win32" ]
  then
    echo
    echo "Copying .pyd & .dll files from the embedded Python distribution..."
    mkdir -pv "${APP_PREFIX}/bin"
    cp -v "${SOURCES_FOLDER_PATH}/${PYTHON3_WIN_SRC_FOLDER_NAME}/python37.zip"\
      "${APP_PREFIX}/bin"

    mkdir -pv "${APP_PREFIX}/bin/DLLs"
    cp -v "${SOURCES_FOLDER_PATH}/${PYTHON3_WIN_SRC_FOLDER_NAME}"/*.pyd \
      "${APP_PREFIX}/bin/DLLs"
    cp -v "${SOURCES_FOLDER_PATH}/${PYTHON3_WIN_SRC_FOLDER_NAME}"/*.dll \
      "${APP_PREFIX}/bin/DLLs"
  fi
}

# Used by gdb-py3 on POSIX and by packages with full
# control over path (like meson) on all platforms.
function add_python3_syslibs()
{
  local python_with_version="python${PYTHON3_VERSION_MAJOR}.${PYTHON3_VERSION_MINOR}"
  if [ ! -d "${APP_PREFIX}/lib/${python_with_version}/" ]
  then
    (
      mkdir -pv "${APP_PREFIX}/lib/${python_with_version}/"

      (
        echo
        echo "Copying .py files from the standard Python library..."

        # Copy all .py from the original source package.
        cp -r "${SOURCES_FOLDER_PATH}/${PYTHON3_SRC_FOLDER_NAME}"/Lib/* \
          "${APP_PREFIX}/lib/${python_with_version}/"

        echo "Compiling all python sources..."
        if [ "${TARGET_PLATFORM}" == "win32" ]
        then
          run_verbose "${WORK_FOLDER_PATH}/${LINUX_INSTALL_RELATIVE_PATH}/libs/bin/python3.${PYTHON3_VERSION_MINOR}" \
            -m compileall \
            -j "${JOBS}" \
            -f "${APP_PREFIX}/lib/${python_with_version}/" \
            || true
        else
          # Compiling tests fails, ignore the errors.
          run_verbose "${LIBS_INSTALL_FOLDER_PATH}/bin/python3.${PYTHON3_VERSION_MINOR}" \
            -m compileall \
            -j "${JOBS}" \
            -f "${APP_PREFIX}/lib/${python_with_version}/" \
            || true
        fi

        # For just in case.
        find "${APP_PREFIX}/lib/${python_with_version}/" \
          \( -name '*.opt-1.pyc' -o -name '*.opt-2.pyc' \) \
          -exec rm -v {} \;
      )

      echo "Replacing .py files with .pyc files..."
      move_pyc "${APP_PREFIX}/lib/${python_with_version}"

      mkdir -pv "${APP_PREFIX}/lib/${python_with_version}/lib-dynload/"

      echo
      echo "Copying Python shared libraries..."

      if [ "${TARGET_PLATFORM}" == "win32" ]
      then
        # Copy the Windows specific DLLs (.pyd) to the separate folder;
        # they are dynamically loaded by Python.
        cp -v "${SOURCES_FOLDER_PATH}/${PYTHON3_WIN_SRC_FOLDER_NAME}"/*.pyd \
          "${APP_PREFIX}/lib/${python_with_version}/lib-dynload/"
        # Copy the usual DLLs too; the python*.dll are used, do not remove them.
        cp -v "${SOURCES_FOLDER_PATH}/${PYTHON3_WIN_SRC_FOLDER_NAME}"/*.dll \
          "${APP_PREFIX}/lib/${python_with_version}/lib-dynload/"
      else
        # Copy dynamically loaded modules and rename folder.
        cp -rv "${LIBS_INSTALL_FOLDER_PATH}/lib/python${PYTHON3_VERSION_MAJOR}.${PYTHON3_VERSION_MINOR}"/lib-dynload/* \
          "${APP_PREFIX}/lib/${python_with_version}/lib-dynload/"
      fi
    )
  fi
}

function process_pyc()
{
  local file_path="$1"

  # echo bbb "${file_path}"

  local file_full_name="$(basename "${file_path}")"
  local file_name="$(echo "${file_full_name}" | sed -e 's|\.cpython-[0-9]*\.pyc||')"
  local folder_path="$(dirname $(dirname "${file_path}"))"

  # echo "${folder_path}" "${file_name}"

  if [ -f "${folder_path}/${file_name}.py" ] 
  then
    mv "${file_path}" "${folder_path}/${file_name}.pyc"
    rm "${folder_path}/${file_name}.py"
  fi
}

export -f process_pyc

function process_pycache()
{
  local folder_path="$1"

  find ${folder_path} -name '*.pyc' -type f -print0 | xargs -0 -L 1 -I {} bash -c 'process_pyc "{}"'

  if [ $(ls -1 "${folder_path}" | wc -l) -eq 0 ]
  then
    rm -rf "${folder_path}"
  fi
}

export -f process_pycache

function move_pyc()
{
  local folder_path="$1"

  find ${folder_path} -name '__pycache__' -type d -print0 | xargs -0 -L 1 -I {} bash -c 'process_pycache "{}"'
}

# -----------------------------------------------------------------------------

function build_libpng() 
{
  # To ensure builds stability, use slightly older releases.
  # https://sourceforge.net/projects/libpng/files/libpng16/
  # https://sourceforge.net/projects/libpng/files/libpng16/older-releases/

  # https://archlinuxarm.org/packages/aarch64/libpng/files/PKGBUILD
  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=libpng-git
  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=mingw-w64-libpng

  # libpng_version="1.2.53"
  # libpng_version="1.6.17"
  # libpng_version="1.6.23" # 2016-06-09
  # libpng_version="1.6.36" # 2018-12-01
  # 2019-04-15, "1.6.37"
  # libpng_SFOLDER="libpng12"
  # libpng_SFOLDER="libpng16"

  local libpng_version="$1"
  local libpng_major_minor_version="$(echo ${libpng_version} | sed -e 's|\([0-9][0-9]*\)\.\([0-9][0-9]*\)\.[0-9].*|\1\2|')"

  local libpng_src_folder_name="libpng-${libpng_version}"

  local libpng_archive="${libpng_src_folder_name}.tar.xz"
  # local libpng_url="https://sourceforge.net/projects/libpng/files/${libpng_SFOLDER}/older-releases/${libpng_version}/${libpng_archive}"
  local libpng_url="https://sourceforge.net/projects/libpng/files/libpng${libpng_major_minor_version}/${libpng_version}/${libpng_archive}"

  local libpng_folder_name="${libpng_src_folder_name}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${libpng_folder_name}"

  local libpng_stamp_file_path="${INSTALL_FOLDER_PATH}/stamp-libpng-${libpng_version}-installed"
  if [ ! -f "${libpng_stamp_file_path}" ]
  then

    cd "${SOURCES_FOLDER_PATH}"

    download_and_extract "${libpng_url}" "${libpng_archive}" \
      "${libpng_src_folder_name}"

    (
      mkdir -pv "${LIBS_BUILD_FOLDER_PATH}/${libpng_folder_name}"
      cd "${LIBS_BUILD_FOLDER_PATH}/${libpng_folder_name}"

      xbb_activate_installed_dev

      CPPFLAGS="${XBB_CPPFLAGS}"
      CFLAGS="${XBB_CFLAGS_NO_W}"      
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"

      LDFLAGS="${XBB_LDFLAGS_LIB}"
      if [ "${TARGET_PLATFORM}" == "linux" ]
      then
        LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
      fi      

      export CPPFLAGS
      export CFLAGS
      export CXXFLAGS
      export LDFLAGS

      if [ ! -f "config.status" ]
      then 
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running libpng configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${libpng_src_folder_name}/configure" --help
          fi

          config_options=()

          config_options+=("--prefix=${LIBS_INSTALL_FOLDER_PATH}")
            
          config_options+=("--build=${BUILD}")
          config_options+=("--host=${HOST}")
          config_options+=("--target=${TARGET}")

          # From Arch.
          config_options+=("--enable-arm-neon=no")

          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${libpng_src_folder_name}/configure" \
            "${config_options[@]}"

          cp "config.log" "${LOGS_FOLDER_PATH}/${libpng_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${libpng_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running libpng make..."

        # Build.
        run_verbose make -j ${JOBS}

        if [ "${WITH_STRIP}" == "y" ]
        then
          run_verbose make install-strip
        else
          run_verbose make install
        fi

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${libpng_folder_name}/make-output.txt"

      copy_license \
        "${SOURCES_FOLDER_PATH}/${libpng_src_folder_name}" \
        "${libpng_folder_name}"

    )

    touch "${libpng_stamp_file_path}"

  else
    echo "Library libpng already installed."
  fi
}

# See also
# https://archlinuxarm.org/packages/aarch64/libjpeg-turbo/files/PKGBUILD

function build_jpeg() 
{
  # http://www.ijg.org
  # http://www.ijg.org/files/

  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=libjpeg9

  # Jan 19 10:26 2014 "9a"
  # Jan 17 10:46 2016 "9b"
  # Jan 14 10:10 2018 "9c"
  # Jan 12 10:07 2020 "9d"

  local jpeg_version="$1"

  local jpeg_src_folder_name="jpeg-${jpeg_version}"

  local jpeg_archive="jpegsrc.v${jpeg_version}.tar.gz"
  local jpeg_url="http://www.ijg.org/files/${jpeg_archive}"

  local jpeg_folder_name="${jpeg_src_folder_name}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${jpeg_folder_name}"

  local jpeg_stamp_file_path="${INSTALL_FOLDER_PATH}/stamp-jpeg-${jpeg_version}-installed"
  if [ ! -f "${jpeg_stamp_file_path}" ]
  then

    cd "${SOURCES_FOLDER_PATH}"

    download_and_extract "${jpeg_url}" "${jpeg_archive}" \
        "${jpeg_src_folder_name}"

    (
      mkdir -pv "${LIBS_BUILD_FOLDER_PATH}/${jpeg_folder_name}"
      cd "${LIBS_BUILD_FOLDER_PATH}/${jpeg_folder_name}"

      xbb_activate_installed_dev

      CPPFLAGS="${XBB_CPPFLAGS}"
      CFLAGS="${XBB_CFLAGS_NO_W}"      
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"

      LDFLAGS="${XBB_LDFLAGS_LIB}"
      if [ "${TARGET_PLATFORM}" == "linux" ]
      then
        LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
      fi      

      export CPPFLAGS
      export CFLAGS
      export CXXFLAGS
      export LDFLAGS

      if [ ! -f "config.status" ]
      then 
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running jpeg configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${jpeg_src_folder_name}/configure" --help
          fi

          config_options=()

          config_options+=("--prefix=${LIBS_INSTALL_FOLDER_PATH}")
            
          config_options+=("--build=${BUILD}")
          config_options+=("--host=${HOST}")
          config_options+=("--target=${TARGET}")

          # --enable-shared needed by sdl2_image on CentOS 64-bit and Ubuntu.
          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${jpeg_src_folder_name}/configure" \
            "${config_options[@]}"

          cp "config.log" "${LOGS_FOLDER_PATH}/${jpeg_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${jpeg_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running jpeg make..."

        # Build.
        run_verbose make -j ${JOBS}

        if [ "${WITH_STRIP}" == "y" ]
        then
          run_verbose make install-strip
        else
          run_verbose make install
        fi

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${jpeg_folder_name}/make-output.txt"

      copy_license \
        "${SOURCES_FOLDER_PATH}/${jpeg_src_folder_name}" \
        "${jpeg_folder_name}"
    
    )

    touch "${jpeg_stamp_file_path}"

  else
    echo "Library jpeg already installed."
  fi
}

function build_pixman() 
{
  # http://www.pixman.org
  # http://cairographics.org/releases/

  # https://archlinuxarm.org/packages/aarch64/pixman/files/PKGBUILD
  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=pixman-git
  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=mingw-w64-pixman

  # pixman_version="0.32.6"
  # pixman_version="0.34.0" # 2016-01-31
  # pixman_version="0.38.0" # 2019-02-11
  # 2019-04-10, "0.38.4"
  # 2020-04-19, "0.40.0"

  local pixman_version="$1"

  local pixman_src_folder_name="pixman-${pixman_version}"

  local pixman_archive="${pixman_src_folder_name}.tar.gz"
  local pixman_url="http://cairographics.org/releases/${pixman_archive}"

  local pixman_folder_name="${pixman_src_folder_name}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${pixman_folder_name}"

  local pixman_stamp_file_path="${INSTALL_FOLDER_PATH}/stamp-pixman-${pixman_version}-installed"
  if [ ! -f "${pixman_stamp_file_path}" ]
  then

    cd "${SOURCES_FOLDER_PATH}"

    download_and_extract "${pixman_url}" "${pixman_archive}" \
      "${pixman_src_folder_name}"

    (
      mkdir -pv "${LIBS_BUILD_FOLDER_PATH}/${pixman_folder_name}"
      cd "${LIBS_BUILD_FOLDER_PATH}/${pixman_folder_name}"

      # Windows libtool chaks for it.
      mkdir -pv test/lib

      xbb_activate_installed_dev

      CPPFLAGS="${XBB_CPPFLAGS}"
      CFLAGS="${XBB_CFLAGS_NO_W}"      
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"

      LDFLAGS="${XBB_LDFLAGS_LIB}"
      if [ "${TARGET_PLATFORM}" == "linux" ]
      then
        LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
      fi      

      export CPPFLAGS
      export CFLAGS
      export CXXFLAGS
      export LDFLAGS

      if [ ! -f "config.status" ]
      then 
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running pixman configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${pixman_src_folder_name}/configure" --help
          fi

          config_options=()

          config_options+=("--prefix=${LIBS_INSTALL_FOLDER_PATH}")
            
          config_options+=("--build=${BUILD}")
          config_options+=("--host=${HOST}")
          config_options+=("--target=${TARGET}")

          # config_options+=("--with-gnu-ld")

          # The numerous disables were inspired from Arch, after the initial 
          # failed on armhf.
          config_options+=("--disable-static-testprogs")
          config_options+=("--disable-loongson-mmi")
          config_options+=("--disable-vmx")
          config_options+=("--disable-arm-simd")
          config_options+=("--disable-arm-neon")
          config_options+=("--disable-arm-iwmmxt")
          config_options+=("--disable-mmx")
          config_options+=("--disable-sse2")
          config_options+=("--disable-ssse3")
          config_options+=("--disable-mips-dspr2")
          config_options+=("--disable-gtk")

          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${pixman_src_folder_name}/configure" \
            "${config_options[@]}"

          cp "config.log" "${LOGS_FOLDER_PATH}/${pixman_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${pixman_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running pixman make..."

        # Build.
        run_verbose make -j ${JOBS}

        if [ "${WITH_STRIP}" == "y" ]
        then
          run_verbose make install-strip
        else
          run_verbose make install
        fi

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${pixman_folder_name}/make-output.txt"

      copy_license \
        "${SOURCES_FOLDER_PATH}/${pixman_src_folder_name}" \
        "${pixman_folder_name}"

    )

    touch "${pixman_stamp_file_path}"

  else
    echo "Library pixman already installed."
  fi
}

# -----------------------------------------------------------------------------


function build_glib() 
{
  # http://ftp.gnome.org/pub/GNOME/sources/glib
  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=glib2-git
  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=mingw-w64-glib2

  # 2015-May-13, "2.44.1"
  # 2017-Mar-13, "2.51.5"
  # 2018-Sep-21, "2.56.3"
  # 2018-Dec-18, "2.56.4"
  # --- Starting with 2.57, the build was migrated to meson (TODO) ---
  # 2018-Aug-23, "2.57.3"
  # 2019-Sep-02, "2.60.7"
  # 2021-May-11, "2.68.4"
  # 2021-Sep-17, "2.70.0"

  local glib_version="$1"

  local glib_src_folder_name="glib-${glib_version}"

  local glib_archive="${glib_src_folder_name}.tar.xz"
  local glib_MAJOR_MINOR_version="$(echo ${glib_version} | sed -e 's|\([0-9][0-9]*\)\.\([0-9][0-9]*\)\.[0-9].*|\1.\2|')"
  local glib_url="http://ftp.gnome.org/pub/GNOME/sources/glib/${glib_MAJOR_MINOR_version}/${glib_archive}"

  local glib_folder_name="${glib_src_folder_name}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${glib_folder_name}"

  local glib_stamp_file_path="${INSTALL_FOLDER_PATH}/stamp-glib-${glib_version}-installed"
  if [ ! -f "${glib_stamp_file_path}" ]
  then

    cd "${SOURCES_FOLDER_PATH}"

    download_and_extract "${glib_url}" "${glib_archive}" \
      "${glib_src_folder_name}"

    (
      # Hack, /gio/lib added because libtool needs it on Win32.
      mkdir -pv "${LIBS_BUILD_FOLDER_PATH}/${glib_folder_name}"/gio/lib
      cd "${LIBS_BUILD_FOLDER_PATH}/${glib_folder_name}"

      xbb_activate_installed_dev

      CPPFLAGS="${XBB_CPPFLAGS}"
      CFLAGS="${XBB_CFLAGS_NO_W}"      
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"

      LDFLAGS="${XBB_LDFLAGS_LIB}"
      if [ "${TARGET_PLATFORM}" == "linux" ]
      then
        LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
      elif [ "${TARGET_PLATFORM}" == "win32" ]
      then
        LDFLAGS+=" -Wl,--allow-multiple-definition"
      fi      

      export CPPFLAGS
      export CFLAGS
      export CXXFLAGS
      export LDFLAGS

      if [ "${TARGET_PLATFORM}" == "darwin" ]
      then
        # GCC fails with
        # error: unknown type name ‘dispatch_block_t
        export CC=clang
        export CXX=clang++
      fi

      if [ ! -f "config.status" ]
      then 
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running glib configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${glib_src_folder_name}/configure" --help
          fi

          config_options=()

          config_options+=("--prefix=${LIBS_INSTALL_FOLDER_PATH}")
            
          config_options+=("--build=${BUILD}")
          config_options+=("--host=${HOST}")
          config_options+=("--target=${TARGET}")

          # --with-libiconv=gnu required on Linux
          config_options+=("--with-libiconv=gnu")
          config_options+=("--without-pcre")

          config_options+=("--disable-selinux")
          config_options+=("--disable-fam")
          config_options+=("--disable-xattr")
          config_options+=("--disable-libelf")
          config_options+=("--disable-libmount")
          config_options+=("--disable-dtrace")
          config_options+=("--disable-systemtap")
          config_options+=("--disable-coverage")
          config_options+=("--disable-Bsymbolic")
          config_options+=("--disable-znodelete")
          config_options+=("--disable-compile-warnings")
          config_options+=("--disable-installed-tests")
          config_options+=("--disable-always-build-tests")

          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${glib_src_folder_name}/configure" \
            "${config_options[@]}"

          # Disable SPLICE, it fails on CentOS.
          local gsed_path=$(which gsed)
          if [ ! -z "${gsed_path}" ]
          then
            run_verbose gsed -i -e '/#define HAVE_SPLICE 1/d' config.h
          else
            run_verbose sed -i -e '/#define HAVE_SPLICE 1/d' config.h
          fi

          cp "config.log" "${LOGS_FOLDER_PATH}/${glib_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${glib_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running glib make..."

        # Build.
        run_verbose make # -j ${JOBS}

        if [ "${WITH_STRIP}" == "y" ]
        then
          run_verbose make install-strip
        else
          run_verbose make install
        fi

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${glib_folder_name}/make-output.txt"

      copy_license \
        "${SOURCES_FOLDER_PATH}/${glib_src_folder_name}" \
        "${glib_folder_name}"

    )

    touch "${glib_stamp_file_path}"

  else
    echo "Library glib already installed."
  fi
}

# -----------------------------------------------------------------------------

function build_libxml2() 
{
  # http://www.xmlsoft.org
  # ftp://xmlsoft.org/libxml2/

  # https://archlinuxarm.org/packages/aarch64/libxml2/files/PKGBUILD
  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=libxml2-git
  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=mingw-w64-libxml2

  # Mar 05 2018, "2.9.8"
  # Jan 03 2019, "2.9.9"
  # Oct 30 2019, "2.9.10"
  # May 13 2021, "2.9.11"

  local libxml2_version="$1"

  local libxml2_src_folder_name="libxml2-${libxml2_version}"

  local libxml2_archive="${libxml2_src_folder_name}.tar.gz"
  local libxml2_url="ftp://xmlsoft.org/libxml2/${libxml2_archive}"

  local libxml2_folder_name="${libxml2_src_folder_name}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${libxml2_folder_name}"

  local libxml2_stamp_file_path="${INSTALL_FOLDER_PATH}/stamp-libxml2-${libxml2_version}-installed"
  if [ ! -f "${libxml2_stamp_file_path}" ]
  then

    cd "${SOURCES_FOLDER_PATH}"

    download_and_extract "${libxml2_url}" "${libxml2_archive}" \
      "${libxml2_src_folder_name}"

    # Fails if not built in place.
    if [ ! -d "${LIBS_BUILD_FOLDER_PATH}/${libxml2_folder_name}" ]
    then
      (
        cp -r "${libxml2_src_folder_name}" \
          "${LIBS_BUILD_FOLDER_PATH}/${libxml2_folder_name}"

        cd "${LIBS_BUILD_FOLDER_PATH}/${libxml2_folder_name}"

        xbb_activate_installed_dev

        autoreconf -vfi
      )
    fi

    (
      # /lib added due to wrong -Llib used during make.
      mkdir -pv "${LIBS_BUILD_FOLDER_PATH}/${libxml2_folder_name}/lib"
      cd "${LIBS_BUILD_FOLDER_PATH}/${libxml2_folder_name}"

      xbb_activate_installed_dev

      CPPFLAGS="${XBB_CPPFLAGS}"
      CFLAGS="${XBB_CFLAGS_NO_W}"      
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"

      LDFLAGS="${XBB_LDFLAGS_LIB}"
      if [ "${TARGET_PLATFORM}" == "linux" ]
      then
        LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
      fi      

      export CPPFLAGS
      export CFLAGS
      export CXXFLAGS
      export LDFLAGS

      if [ ! -f "config.status" ]
      then 
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running libxml2 configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "configure" --help
          fi

          config_options=()

          config_options+=("--prefix=${LIBS_INSTALL_FOLDER_PATH}")
            
          config_options+=("--build=${BUILD}")
          config_options+=("--host=${HOST}")
          config_options+=("--target=${TARGET}")

          config_options+=("--without-python")
          # Not functional on Windows anyway
          if [ "${TARGET_PLATFORM}" == "win32" ]
          then
            config_options+=("--without-catalog")
          fi

          run_verbose bash ${DEBUG} "configure" \
            "${config_options[@]}"

          cp "config.log" "${LOGS_FOLDER_PATH}/${libxml2_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${libxml2_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running libxml2 make..."

        # Build.
        run_verbose make -j ${JOBS}

        if [ "${WITH_STRIP}" == "y" ]
        then
          run_verbose make install-strip
        else
          run_verbose make install
        fi

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${libxml2_folder_name}/make-output.txt"

      copy_license \
        "${SOURCES_FOLDER_PATH}/${libxml2_src_folder_name}" \
        "${libxml2_folder_name}"

    )

    touch "${libxml2_stamp_file_path}"

  else
    echo "Library libxml2 already installed."
  fi
}

# -----------------------------------------------------------------------------

function build_libedit()
{
  # https://www.thrysoee.dk/editline/
  # https://www.thrysoee.dk/editline/libedit-20210522-3.1.tar.gz

  # https://archlinuxarm.org/packages/aarch64/libedit/files/PKGBUILD

  # 2021-05-22, "20210522-3.1"

  local libedit_version="$1"
  local libedit_version_short="$(echo ${libedit_version} | sed -e 's|[0-9]*-||')"

  local libedit_src_folder_name="libedit-${libedit_version}"
  local libedit_archive="${libedit_src_folder_name}.tar.gz"

  local libedit_url="https://www.thrysoee.dk/editline/${libedit_archive}"

  local libedit_folder_name="libedit-${libedit_version_short}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${libedit_folder_name}"

  local libedit_stamp_file_path="${INSTALL_FOLDER_PATH}/stamp-${libedit_folder_name}-installed"
  if [ ! -f "${libedit_stamp_file_path}" ]
  then

    cd "${SOURCES_FOLDER_PATH}"

    download_and_extract "${libedit_url}" "${libedit_archive}" \
      "${libedit_src_folder_name}"

    (
      mkdir -pv "${LIBS_BUILD_FOLDER_PATH}/${libedit_folder_name}"
      cd "${LIBS_BUILD_FOLDER_PATH}/${libedit_folder_name}"

      xbb_activate_installed_dev

      CPPFLAGS="${XBB_CPPFLAGS}"
      CFLAGS="${XBB_CFLAGS_NO_W}"
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"

      LDFLAGS="${XBB_LDFLAGS_LIB}"
      if [ "${TARGET_PLATFORM}" == "linux" ]
      then
        CPPFLAGS+=" -I${LIBS_INSTALL_FOLDER_PATH}/include/ncurses"
        LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
      fi      

      export CPPFLAGS
      export CFLAGS
      export CXXFLAGS
      export LDFLAGS

      if [ ! -f "config.status" ]
      then 
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running libedit configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${libedit_src_folder_name}/configure" --help
          fi

          config_options=()

          config_options+=("--prefix=${LIBS_INSTALL_FOLDER_PATH}")
            
          config_options+=("--build=${BUILD}")
          config_options+=("--host=${HOST}")
          config_options+=("--target=${TARGET}")

          # config_options+=("--disable-nls")
          # config_options+=("--disable-shared")

          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${libedit_src_folder_name}/configure" \
            "${config_options[@]}"

          cp "config.log" "${LOGS_FOLDER_PATH}/${libedit_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${libedit_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running libedit make..."

        # Build.
        run_verbose make -j ${JOBS}

        if [ "${WITH_TESTS}" == "y" ]
        then
          run_verbose make -j1 check
        fi

        if [ "${WITH_STRIP}" == "y" ]
        then
          run_verbose make install-strip
        else
          run_verbose make install
        fi

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${libedit_folder_name}/make-output.txt"

      copy_license \
        "${SOURCES_FOLDER_PATH}/${libedit_src_folder_name}" \
        "${libedit_folder_name}"

    )

    touch "${libedit_stamp_file_path}"

  else
    echo "Library libedit already installed."
  fi
}

# -----------------------------------------------------------------------------

# Not yet functional.
function build_xar() 
{
  # https://github.com/mackyle/xar
  # https://github.com/mackyle/xar/archive/refs/tags/xar-1.6.1.tar.gz

  # 18 Sep 2012, "1.6.1"

  local xar_version="$1"

  local xar_src_folder_name="xar-xar-${xar_version}"

  local xar_archive="xar-${xar_version}.tar.gz"
  # GitHub release archive.
  local xar_url="https://github.com/mackyle/xar/archive/refs/tags/${xar_archive}"

  local xar_folder_name="xar-${xar_version}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${xar_folder_name}"

  local xar_stamp_file_path="${INSTALL_FOLDER_PATH}/stamp-${xar_folder_name}-installed"
  if [ ! -f "${xar_stamp_file_path}" ]
  then

    echo
    echo "xar in-source building"

    cd "${LIBS_BUILD_FOLDER_PATH}"

    if [ ! -d "${LIBS_BUILD_FOLDER_PATH}/${xar_folder_name}" ]
    then
      cd "${LIBS_BUILD_FOLDER_PATH}"

      download_and_extract "${xar_url}" "${xar_archive}" \
        "${xar_src_folder_name}"

      if [ "${xar_src_folder_name}" != "${xar_folder_name}" ]
      then
        mv -v "${xar_src_folder_name}" "${xar_folder_name}"
      fi
    fi

    (
      cd "${LIBS_BUILD_FOLDER_PATH}/${xar_folder_name}/xar/"

      xbb_activate_installed_dev

      CPPFLAGS="${XBB_CPPFLAGS}"
      CFLAGS="${XBB_CFLAGS_NO_W}"
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"
      
      LDFLAGS="${XBB_LDFLAGS_LIB}"
      if [ "${TARGET_PLATFORM}" == "linux" ]
      then
        LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH}"
      fi      

      export CPPFLAGS
      export CFLAGS
      export CXXFLAGS
      export LDFLAGS

      if [ ! -x "configure" ]
      then

        echo
        echo "Running xar autogen..."
        run_verbose bash ${DEBUG} "autogen.sh"

      fi

      if [ ! -f "config.status" ]
      then 
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running xar configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "./configure" --help
          fi

          config_options=()

          config_options+=("--prefix=${LIBS_INSTALL_FOLDER_PATH}")
            
          config_options+=("--build=${BUILD}")
          config_options+=("--host=${HOST}")
          config_options+=("--target=${TARGET}")

          run_verbose bash ${DEBUG} "./configure" \
            "${config_options[@]}"

          cp "config.log" "${LOGS_FOLDER_PATH}/${xar_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${xar_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running xar make..."

        # Build.
        run_verbose make -j ${JOBS}

        if [ "${WITH_TESTS}" == "y" ]
        then
          run_verbose make -j1 check
        fi

        if [ "${WITH_STRIP}" == "y" ]
        then
          run_verbose make install-strip
        else
          run_verbose make install
        fi

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${xar_folder_name}/make-output.txt"

      copy_license \
        "${SOURCES_FOLDER_PATH}/${xar_src_folder_name}" \
        "${xar_folder_name}"

    )

    touch "${xar_stamp_file_path}"

  else
    echo "Library xar already installed."
  fi
}

# -----------------------------------------------------------------------------
