# -----------------------------------------------------------------------------
# This file is part of the xPack distribution.
#   (https://xpack.github.io)
# Copyright (c) 2020 Liviu Ionescu.
#
# Permission to use, copy, modify, and/or distribute this software 
# for any purpose is hereby granted, under the terms of the MIT license.
# -----------------------------------------------------------------------------

# Helper script used in xPack Developer Tools build scripts. 
# As the name implies, it should contain only functions and 
# should be included with 'source' by the build scripts (both native
# and container).

# -----------------------------------------------------------------------------

function build_patchelf() 
{
  # https://nixos.org/patchelf.html
  # https://github.com/NixOS/patchelf
  # https://github.com/NixOS/patchelf/releases/
  # https://github.com/NixOS/patchelf/releases/download/0.12/patchelf-0.12.tar.bz2
  # https://github.com/NixOS/patchelf/archive/0.12.tar.gz
  
  # 2016-02-29, "0.9"
  # 2019-03-28, "0.10"
  # 2020-06-09, "0.11"
  # 2020-08-27, "0.12"

  local patchelf_version="$1"

  local patchelf_src_folder_name="patchelf-${patchelf_version}"

  local patchelf_archive="${patchelf_src_folder_name}.tar.bz2"
  # GitHub release archive.
  local patchelf_github_archive="${patchelf_version}.tar.gz"
  local patchelf_url="https://github.com/NixOS/patchelf/archive/${patchelf_github_archive}"

  local patchelf_folder_name="${patchelf_src_folder_name}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${patchelf_folder_name}"

  local patchelf_stamp_file_path="${STAMPS_FOLDER_PATH}/stamp-${patchelf_folder_name}-installed"
  if [ ! -f "${patchelf_stamp_file_path}" ]
  then

    cd "${SOURCES_FOLDER_PATH}"

    download_and_extract "${patchelf_url}" "${patchelf_archive}" \
      "${patchelf_src_folder_name}"

    (
      if [ ! -x "${SOURCES_FOLDER_PATH}/${patchelf_src_folder_name}/configure" ]
      then

        cd "${SOURCES_FOLDER_PATH}/${patchelf_src_folder_name}"
        
        xbb_activate_installed_dev

        run_verbose bash ${DEBUG} "bootstrap.sh"

      fi
    ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${patchelf_folder_name}/autogen-output.txt"

    (
      mkdir -pv "${LIBS_BUILD_FOLDER_PATH}/${patchelf_folder_name}"
      cd "${LIBS_BUILD_FOLDER_PATH}/${patchelf_folder_name}"

      xbb_activate_installed_dev

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

      if [ ! -f "config.status" ]
      then
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running patchelf configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${patchelf_src_folder_name}/configure" --help
          fi

          config_options=()

          config_options+=("--prefix=${LIBS_INSTALL_FOLDER_PATH}")
            
          config_options+=("--build=${BUILD}")
          config_options+=("--host=${HOST}")
          config_options+=("--target=${TARGET}")

          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${patchelf_src_folder_name}/configure" \
            "${config_options[@]}"

          cp "config.log" "${LOGS_FOLDER_PATH}/${patchelf_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${patchelf_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running patchelf make..."

        # Build.
        run_verbose make -j ${JOBS}

        if [ "${WITH_STRIP}" == "y" ]
        then
          run_verbose make install-strip
        else
          run_verbose make install
        fi

        show_libs "${LIBS_INSTALL_FOLDER_PATH}/bin/patchelf"

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${patchelf_folder_name}/make-output.txt"

      copy_license \
        "${SOURCES_FOLDER_PATH}/${patchelf_src_folder_name}" \
        "${patchelf_folder_name}"

    )

    (
      test_patchelf
    ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${patchelf_folder_name}/test-output.txt"

    touch "${patchelf_stamp_file_path}"

  else
    echo "Component patchelf already installed."
  fi
}

function test_patchelf()
{
  (
    xbb_activate

    echo
    echo "Checking the patchelf shared libraries..."

    show_libs "${LIBS_INSTALL_FOLDER_PATH}/bin/patchelf"

    echo
    echo "Checking if patchelf starts..."
    "${LIBS_INSTALL_FOLDER_PATH}/bin/patchelf" --version
    "${LIBS_INSTALL_FOLDER_PATH}/bin/patchelf" --help
  )
}

# -----------------------------------------------------------------------------

function build_automake() 
{
  # https://www.gnu.org/software/automake/
  # https://ftp.gnu.org/gnu/automake/

  # https://archlinuxarm.org/packages/any/automake/files/PKGBUILD
  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=automake-git

  # 2015-01-05, "1.15"
  # 2018-02-25, "1.16"
  # 2020-03-21, "1.16.2"
  # 2020-11-18, "1.16.3"

  local automake_version="$1"

  local automake_src_folder_name="automake-${automake_version}"

  local automake_archive="${automake_src_folder_name}.tar.xz"
  local automake_url="https://ftp.gnu.org/gnu/automake/${automake_archive}"

  local automake_folder_name="${automake_src_folder_name}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${automake_folder_name}"

  # help2man: can't get `--help' info from automake-1.16
  # Try `--no-discard-stderr' if option outputs to stderr

  local automake_patch_file_path="${BUILD_GIT_PATH}/patches/${automake_folder_name}.patch"
  local automake_stamp_file_path="${STAMPS_FOLDER_PATH}/stamp-${automake_folder_name}-installed"
  if [ ! -f "${automake_stamp_file_path}" -o ! -d "${BUILD_FOLDER_PATH}/${automake_folder_name}" ]
  then

    cd "${SOURCES_FOLDER_PATH}"

    download_and_extract "${automake_url}" "${automake_archive}" \
      "${automake_src_folder_name}" \
      "${automake_patch_file_path}"

    (
      mkdir -pv "${BUILD_FOLDER_PATH}/${automake_folder_name}"
      cd "${BUILD_FOLDER_PATH}/${automake_folder_name}"

      xbb_activate_installed_dev

      export CPPFLAGS="${XBB_CPPFLAGS}"
      export CFLAGS="${XBB_CFLAGS_NO_W}"
      export CXXFLAGS="${XBB_CXXFLAGS_NO_W}"

      export LDFLAGS="${XBB_LDFLAGS_APP_STATIC_GCC}"

      if [ ! -f "config.status" ]
      then
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running automake configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${automake_src_folder_name}/configure" --help
          fi

          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${automake_src_folder_name}/configure" \
            --prefix="${LIBS_INSTALL_FOLDER_PATH}" \
            \
            --build="${BUILD}" \

          cp "config.log" "${LOGS_FOLDER_PATH}/${automake_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${automake_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running automake make..."

        # Build.
        run_verbose make -j ${JOBS}

        # make install-strip
        run_verbose make install

        # Takes too long and some tests fail.
        # XFAIL: t/pm/Cond2.pl
        # XFAIL: t/pm/Cond3.pl
        # ...
        if false
        then
          run_verbose make -j1 check
        fi

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${automake_folder_name}/make-output.txt"
    )

    (
      test_automake
    ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${automake_folder_name}/test-output.txt"

    hash -r

    touch "${automake_stamp_file_path}"

  else
    echo "Component automake already installed."
  fi

  test_functions+=("test_automake")
}

function test_automake()
{
  (
    xbb_activate_installed_bin

    echo
    echo "Testing if automake binaries start properly..."

    run_verbose "${LIBS_INSTALL_FOLDER_PATH}/bin/automake" --version
  )
}

# -----------------------------------------------------------------------------


function build_findutils() 
{
  # https://www.gnu.org/software/findutils/
  # https://ftp.gnu.org/gnu/findutils/
  # https://ftp.gnu.org/gnu/findutils/findutils-4.8.0.tar.xz
  
  # 2021-01-09, "4.8.0"

  local findutils_version="$1"

  local findutils_src_folder_name="findutils-${findutils_version}"

  local findutils_archive="${findutils_src_folder_name}.tar.xz"
  local findutils_url="https://ftp.gnu.org/gnu/findutils/${findutils_archive}"

  local findutils_folder_name="${findutils_src_folder_name}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${findutils_folder_name}"

  local findutils_stamp_file_path="${STAMPS_FOLDER_PATH}/stamp-${findutils_folder_name}-installed"
  if [ ! -f "${findutils_stamp_file_path}" ]
  then

    cd "${SOURCES_FOLDER_PATH}"

    download_and_extract "${findutils_url}" "${findutils_archive}" \
      "${findutils_src_folder_name}"

    (
      if [ ! -x "${SOURCES_FOLDER_PATH}/${findutils_src_folder_name}/configure" ]
      then

        cd "${SOURCES_FOLDER_PATH}/${findutils_src_folder_name}"
        
        xbb_activate_installed_dev

        run_verbose bash ${DEBUG} "bootstrap.sh"

      fi
    ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${findutils_folder_name}/autogen-output.txt"

    (
      mkdir -pv "${LIBS_BUILD_FOLDER_PATH}/${findutils_folder_name}"
      cd "${LIBS_BUILD_FOLDER_PATH}/${findutils_folder_name}"

      xbb_activate_installed_dev

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

      if [ ! -f "config.status" ]
      then
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running findutils configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${findutils_src_folder_name}/configure" --help
          fi

          config_options=()

          config_options+=("--prefix=${LIBS_INSTALL_FOLDER_PATH}")
            
          config_options+=("--build=${BUILD}")
          # config_options+=("--host=${HOST}")
          # config_options+=("--target=${TARGET}")

          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${findutils_src_folder_name}/configure" \
            "${config_options[@]}"

          cp "config.log" "${LOGS_FOLDER_PATH}/${findutils_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${findutils_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running findutils make..."

        # Build.
        run_verbose make -j ${JOBS}

        if [ "${WITH_STRIP}" == "y" ]
        then
          run_verbose make install-strip
        else
          run_verbose make install
        fi

        show_libs "${LIBS_INSTALL_FOLDER_PATH}/bin/find"

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${findutils_folder_name}/make-output.txt"

      copy_license \
        "${SOURCES_FOLDER_PATH}/${findutils_src_folder_name}" \
        "${findutils_folder_name}"

    )

    (
      test_findutils
    ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${findutils_folder_name}/test-output.txt"

    touch "${findutils_stamp_file_path}"

  else
    echo "Component findutils already installed."
  fi
}

function test_findutils()
{
  (
    xbb_activate

    echo
    echo "Checking the findutils shared libraries..."

    show_libs "${LIBS_INSTALL_FOLDER_PATH}/bin/find"

    echo
    echo "Checking if findutils starts..."
    "${LIBS_INSTALL_FOLDER_PATH}/bin/find" || true
  )
}

# =============================================================================

# Used to initialise options in all mingw builds:
# `config_options=("${config_options_common[@]}")`

function prepare_mingw_config_options_common()
{
  # ---------------------------------------------------------------------------
  # Used in multiple configurations.

  config_options_common=()

  local prefix=${APP_PREFIX}
  if [ $# -ge 1 ]
  then
    config_options_common+=("--prefix=$1")
  else
    echo "prepare_mingw_config_options_common requires a prefix path"
    exit 1
  fi
                
  config_options_common+=("--disable-multilib")

  # https://docs.microsoft.com/en-us/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-160
  # Windows 7
  config_options_common+=("--with-default-win32-winnt=0x601")

  # `ucrt` is the new Microsoft C runtime.
  # https://support.microsoft.com/en-us/topic/update-for-universal-c-runtime-in-windows-c0514201-7fe6-95a3-b0a5-287930f3560c
  config_options_common+=("--with-default-msvcrt=${MINGW_MSVCRT:-ucrt}")
  # config_options_common+=("--with-default-msvcrt=${MINGW_MSVCRT:-msvcrt}")

  config_options_common+=("--enable-wildcard")
  config_options_common+=("--enable-warnings=0")
}

function prepare_mingw_env()
{
  export MINGW_VERSION="$1"
  export MINGW_NAME_SUFFIX=${2-''}

  export MINGW_VERSION_MAJOR=$(echo ${MINGW_VERSION} | sed -e 's|\([0-9][0-9]*\)\..*|\1|')

  # The original SourceForge location.
  export MINGW_SRC_FOLDER_NAME="mingw-w64-v${MINGW_VERSION}"
  export MINGW_FOLDER_NAME="${MINGW_SRC_FOLDER_NAME}${MINGW_NAME_SUFFIX}"
}

function build_mingw_headers() 
{
  # http://mingw-w64.org/doku.php/start
  # https://sourceforge.net/projects/mingw-w64/files/mingw-w64/mingw-w64-release/

  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=mingw-w64-headers
  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=mingw-w64-crt
  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=mingw-w64-winpthreads
  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=mingw-w64-binutils
  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=mingw-w64-gcc

  # https://github.com/msys2/MINGW-packages/blob/master/mingw-w64-headers-git/PKGBUILD
  # https://github.com/msys2/MINGW-packages/blob/master/mingw-w64-crt-git/PKGBUILD
  # https://github.com/msys2/MINGW-packages/blob/master/mingw-w64-winpthreads-git/PKGBUILD
  # https://github.com/msys2/MINGW-packages/blob/master/mingw-w64-binutils/PKGBUILD
  # https://github.com/msys2/MINGW-packages/blob/master/mingw-w64-gcc/PKGBUILD
  
  # https://github.com/msys2/MSYS2-packages/blob/master/gcc/PKGBUILD

  # https://github.com/StephanTLavavej/mingw-distro

  # 2018-06-03, "5.0.4"
  # 2018-09-16, "6.0.0"
  # 2019-11-11, "7.0.0"
  # 2020-09-18, "8.0.0"
  # 2021-05-09, "8.0.2"
  # 2021-05-22, "9.0.0"

  local mingw_archive="${MINGW_SRC_FOLDER_NAME}.tar.bz2"
  local mingw_url="https://sourceforge.net/projects/mingw-w64/files/mingw-w64/mingw-w64-release/${mingw_archive}"
  
  # If SourceForge is down, there is also a GitHub mirror.
  # https://github.com/mirror/mingw-w64
  # MINGW_FOLDER_NAME="mingw-w64-${MINGW_VERSION}"
  # mingw_archive="v${MINGW_VERSION}.tar.gz"
  # mingw_url="https://github.com/mirror/mingw-w64/archive/${mingw_archive}"
 
  # https://sourceforge.net/p/mingw-w64/wiki2/Cross%20Win32%20and%20Win64%20compiler/
  # https://sourceforge.net/p/mingw-w64/mingw-w64/ci/master/tree/configure

  # For binutils/GCC, the official method to build the mingw-w64 toolchain
  # is to set --prefix and --with-sysroot to the same directory to allow
  # the toolchain to be relocatable. 

  # Recommended GCC configuration:
  # (to disable multilib, add `--enable-targets="${TARGET}"`)
  #
  # $ ../gcc-trunk/configure --{host,build}=<build triplet> \
	# --target=x86_64-w64-mingw32 --enable-multilib --enable-64bit \
	# --{prefix,with-sysroot}=<prefix> --enable-version-specific-runtime-libs \
	# --enable-shared --with-dwarf --enable-fully-dynamic-string \
	# --enable-languages=c,ada,c++,fortran,objc,obj-c++ --enable-libgomp \
	# --enable-libssp --with-host-libstdcxx="-lstdc++ -lsupc++" \
	# --with-{gmp,mpfr,mpc,cloog,ppl}=<host dir> --enable-lto
  #
  # $ make all-gcc && make install-gcc
  #
  # build mingw-w64-crt (headers, crt, tools)
  #
  # $ make all-target-libgcc && make install-target-libgcc
  #
  # build mingw-libraries (winpthreads)
  #
  # Continue the GCC build (C++)
  # $ make && make install

  # ---------------------------------------------------------------------------

  # The 'headers' step creates the 'include' folder.

  local mingw_headers_folder_name="mingw-${MINGW_VERSION}-headers${MINGW_NAME_SUFFIX}"

  cd "${SOURCES_FOLDER_PATH}"

  download_and_extract "${mingw_url}" "${mingw_archive}" \
    "${MINGW_SRC_FOLDER_NAME}"

  # The docs recommend to add several links, but for non-multilib
  # configurations there are no target or lib32/lib64 specific folders.

  mkdir -pv "${LOGS_FOLDER_PATH}/${MINGW_FOLDER_NAME}"

  # ---------------------------------------------------------------------------

  # https://github.com/archlinux/svntogit-community/blob/packages/mingw-w64-headers/trunk/PKGBUILD

  local mingw_headers_stamp_file_path="${STAMPS_FOLDER_PATH}/stamp-${mingw_headers_folder_name}-installed"
  if [ ! -f "${mingw_headers_stamp_file_path}" ]
  then
    (
      mkdir -p "${BUILD_FOLDER_PATH}/${mingw_headers_folder_name}"
      cd "${BUILD_FOLDER_PATH}/${mingw_headers_folder_name}"

      if [ ! -f "config.status" ]
      then
        (
          echo
          echo "Running mingw-w64-headers${MINGW_NAME_SUFFIX} configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${MINGW_SRC_FOLDER_NAME}/mingw-w64-headers/configure" --help
          fi

          if [ -n "${MINGW_NAME_SUFFIX}" ]
          then
            prepare_mingw_config_options_common "${APP_PREFIX}${MINGW_NAME_SUFFIX}"
            config_options=("${config_options_common[@]}")
            
            config_options+=("--build=${BUILD}")
            # The bootstrap binaries will run on the build machine.
            config_options+=("--host=${TARGET}")
            config_options+=("--target=${TARGET}")
          else
            prepare_mingw_config_options_common "${APP_PREFIX}${MINGW_NAME_SUFFIX}/${CROSS_COMPILE_PREFIX}"
            config_options=("${config_options_common[@]}")

            config_options+=("--build=${BUILD}")
            config_options+=("--host=${HOST}")
            config_options+=("--target=${TARGET}")
          fi

          config_options+=("--with-tune=generic")

          config_options+=("--enable-sdk=all")
          config_options+=("--enable-idl")
          config_options+=("--without-widl")

          # From Arch, but not recognised.
          # config_options+=("--enable-secure-api")

          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${MINGW_SRC_FOLDER_NAME}/mingw-w64-headers/configure" \
            "${config_options[@]}"

          cp "config.log" "${LOGS_FOLDER_PATH}/${MINGW_FOLDER_NAME}/config-headers-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${MINGW_FOLDER_NAME}/configure-headers-output.txt"
      fi

      (
        echo
        echo "Running mingw-w64-headers${MINGW_NAME_SUFFIX} make..."

        # Build.
        run_verbose make -j ${JOBS}

        run_verbose make install-strip

        if [ -n "${MINGW_NAME_SUFFIX}" ]
        then
          mkdir -pv "${APP_PREFIX}${MINGW_NAME_SUFFIX}/${CROSS_COMPILE_PREFIX}"
          (
            cd "${APP_PREFIX}${MINGW_NAME_SUFFIX}/${CROSS_COMPILE_PREFIX}"
            run_verbose ln -sv ../include include
          )

          # This is this needed by the GCC bootstrap; otherwise:
          # The directory that should contain system headers does not exist:
          # /Host/home/ilg/Work/gcc-11.1.0-1/win32-x64/install/gcc-bootstrap/mingw/include

          rm -rf "${APP_PREFIX}${MINGW_NAME_SUFFIX}/mingw"
          ( 
            cd "${APP_PREFIX}${MINGW_NAME_SUFFIX}"
            run_verbose ln -sv "${CROSS_COMPILE_PREFIX}" "mingw"
          )
        fi

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${MINGW_FOLDER_NAME}/make-headers-output.txt"

      # No need to do it again for each component.
      if [ -z "${MINGW_NAME_SUFFIX}" ]
      then
        copy_license \
          "${SOURCES_FOLDER_PATH}/${MINGW_SRC_FOLDER_NAME}" \
          "${MINGW_FOLDER_NAME}"
      fi

    )

    touch "${mingw_headers_stamp_file_path}"

  else
    echo "Component mingw-w64-headers${MINGW_NAME_SUFFIX} already installed."
  fi
}

function build_mingw_crt() 
{
  # ---------------------------------------------------------------------------

  # The 'crt' step creates the C run-time in the 'lib' folder.

  # https://github.com/archlinux/svntogit-community/blob/packages/mingw-w64-crt/trunk/PKGBUILD

  local mingw_crt_folder_name="mingw-${MINGW_VERSION}-crt${MINGW_NAME_SUFFIX}"

  local mingw_crt_stamp_file_path="${STAMPS_FOLDER_PATH}/stamp-${mingw_crt_folder_name}-installed"
  if [ ! -f "${mingw_crt_stamp_file_path}" ]
  then
    (
      mkdir -p "${BUILD_FOLDER_PATH}/${mingw_crt_folder_name}"
      cd "${BUILD_FOLDER_PATH}/${mingw_crt_folder_name}"

      # -ffunction-sections -fdata-sections fail with:
      # Error: .seh_endproc used in segment '.text' instead of expected '.text$WinMainCRTStartup'
      CPPFLAGS=""
      CFLAGS="-O2 -pipe -w"
      CXXFLAGS="-O2 -pipe -w"

      LDFLAGS=""

      if [ "${IS_DEVELOP}" == "y" ]
      then
        LDFLAGS+=" -v"
      fi

      export CPPFLAGS
      export CFLAGS
      export CXXFLAGS
      export LDFLAGS

      # Without it, apparently a bug in autoconf/c.m4, function AC_PROG_CC, results in:
      # checking for _mingw_mac.h... no
      # configure: error: Please check if the mingw-w64 header set and the build/host option are set properly.
      # (https://github.com/henry0312/build_gcc/issues/1)
      # export CC=""
      # Alternately it is possible to define CC to the mingw-gcc.

      if [ ! -f "config.status" ]
      then
        (
          if [ "${IS_DEVELOP}" == "y" ]
          then
            env | sort
          fi

          echo
          echo "Running mingw-w64-crt${MINGW_NAME_SUFFIX} configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${MINGW_SRC_FOLDER_NAME}/mingw-w64-crt/configure" --help
          fi

          prepare_mingw_config_options_common "${APP_PREFIX}${MINGW_NAME_SUFFIX}/${CROSS_COMPILE_PREFIX}"
          config_options=("${config_options_common[@]}")
          config_options+=("--with-sysroot=${APP_PREFIX}${MINGW_NAME_SUFFIX}")

          if [ -n "${MINGW_NAME_SUFFIX}" ]
          then
            config_options+=("--build=${BUILD}")
            # The bootstrap binaries will run on the build machine.
            config_options+=("--host=${TARGET}")
            config_options+=("--target=${TARGET}")
          else
            config_options+=("--build=${BUILD}")
            config_options+=("--host=${HOST}")
            config_options+=("--target=${TARGET}")
          fi

          if [ "${TARGET_ARCH}" == "x64" ]
          then
            config_options+=("--disable-lib32")
            config_options+=("--enable-lib64")
          elif [ "${TARGET_ARCH}" == "x32" -o "${TARGET_ARCH}" == "ia32" ]
          then
            config_options+=("--enable-lib32")
            config_options+=("--disable-lib64")
          else
            echo "Oops! Unsupported TARGET_ARCH=${TARGET_ARCH}."
            exit 1
          fi

          config_options+=("--enable-wildcard")

          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${MINGW_SRC_FOLDER_NAME}/mingw-w64-crt/configure" \
            "${config_options[@]}"

          cp "config.log" "${LOGS_FOLDER_PATH}/${MINGW_FOLDER_NAME}/config-crt-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${MINGW_FOLDER_NAME}/configure-crt-output.txt"
      fi

      (
        echo
        echo "Running mingw-w64-crt${MINGW_NAME_SUFFIX} make..."

        # Build.
        run_verbose make -j ${JOBS}

        run_verbose make install-strip

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${MINGW_FOLDER_NAME}/make-crt-output.txt"
    )

    touch "${mingw_crt_stamp_file_path}"

  else
    echo "Component mingw-w64-crt${MINGW_NAME_SUFFIX} already installed."
  fi
}


function build_mingw_winpthreads() 
{
  # https://github.com/archlinux/svntogit-community/blob/packages/mingw-w64-winpthreads/trunk/PKGBUILD

  local mingw_winpthreads_folder_name="mingw-${MINGW_VERSION}-winpthreads${MINGW_NAME_SUFFIX}"

  local mingw_winpthreads_stamp_file_path="${STAMPS_FOLDER_PATH}/stamp-${mingw_winpthreads_folder_name}-installed"
  if [ ! -f "${mingw_winpthreads_stamp_file_path}" ]
  then
    (
      mkdir -p "${BUILD_FOLDER_PATH}/${mingw_winpthreads_folder_name}"
      cd "${BUILD_FOLDER_PATH}/${mingw_winpthreads_folder_name}"

      # -ffunction-sections -fdata-sections fail with:
      # Error: .seh_endproc used in segment '.text' instead of expected '.text$WinMainCRTStartup'
      CPPFLAGS=""
      CFLAGS="-O2 -pipe -w"
      CXXFLAGS="-O2 -pipe -w"

      LDFLAGS=""

      if [ "${IS_DEVELOP}" == "y" ]
      then
        LDFLAGS+=" -v"
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
          echo "Running mingw-w64-winpthreads${MINGW_NAME_SUFFIX} configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${MINGW_SRC_FOLDER_NAME}/mingw-w64-libraries/winpthreads/configure" --help
          fi

          config_options=()

          config_options+=("--prefix=${APP_PREFIX}${MINGW_NAME_SUFFIX}/${CROSS_COMPILE_PREFIX}")
          config_options+=("--with-sysroot=${APP_PREFIX}${MINGW_NAME_SUFFIX}")

          config_options+=("--libdir=${APP_PREFIX}${MINGW_NAME_SUFFIX}/${CROSS_COMPILE_PREFIX}/lib")

          config_options+=("--build=${BUILD}")
          config_options+=("--host=${HOST}")
          config_options+=("--target=${TARGET}")

          # This prevents references to libwinpthread-1.dll, which is
          # particularly useful with -static-libstdc++, otherwise the
          # result is not exactly static.
          # This also requires disabling shared in the GCC configuration.
          config_options+=("--disable-shared")

          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${MINGW_SRC_FOLDER_NAME}/mingw-w64-libraries/winpthreads/configure" \
            "${config_options[@]}"

         cp "config.log" "${LOGS_FOLDER_PATH}/${MINGW_FOLDER_NAME}/config-winpthreads-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${MINGW_FOLDER_NAME}/configure-winpthreads-output.txt"
      fi
      
      (
        echo
        echo "Running mingw-w64-winpthreads${MINGW_NAME_SUFFIX} make..."

        # Build.
        run_verbose make -j ${JOBS}

        run_verbose make install-strip

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${MINGW_FOLDER_NAME}/make-winpthreads-output.txt"
    )

    touch "${mingw_winpthreads_stamp_file_path}"

  else
    echo "Component mingw-w64-winpthreads${MINGW_NAME_SUFFIX} already installed."
  fi
}

function build_mingw_winstorecompat() 
{
  local mingw_winstorecompat_folder_name="mingw-${MINGW_VERSION}-winstorecompat${MINGW_NAME_SUFFIX}"

  local mingw_winstorecompat_stamp_file_path="${STAMPS_FOLDER_PATH}/stamp-${mingw_winstorecompat_folder_name}-installed"
  if [ ! -f "${mingw_winstorecompat_stamp_file_path}" ]
  then

    (
      mkdir -p "${BUILD_FOLDER_PATH}/${mingw_winstorecompat_folder_name}"
      cd "${BUILD_FOLDER_PATH}/${mingw_winstorecompat_folder_name}"

      CPPFLAGS=""
      CFLAGS="-O2 -pipe -w"
      CXXFLAGS="-O2 -pipe -w"

      LDFLAGS=""

      if [ "${IS_DEVELOP}" == "y" ]
      then
        LDFLAGS+=" -v"
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
          echo "Running mingw-w64-winstorecompat${MINGW_NAME_SUFFIX} configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${MINGW_SRC_FOLDER_NAME}/mingw-w64-libraries/winstorecompat/configure" --help
          fi

          config_options=()
          # Note: native library.

          config_options+=("--prefix=${APP_PREFIX}${MINGW_NAME_SUFFIX}/${CROSS_COMPILE_PREFIX}")
          config_options+=("--libdir=${APP_PREFIX}${MINGW_NAME_SUFFIX}/${CROSS_COMPILE_PREFIX}/lib")

          config_options+=("--build=${BUILD}")
          config_options+=("--host=${HOST}")
          config_options+=("--target=${TARGET}")

          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${MINGW_SRC_FOLDER_NAME}/mingw-w64-libraries/winstorecompat/configure" \
            "${config_options[@]}"

         cp "config.log" "${LOGS_FOLDER_PATH}/${MINGW_FOLDER_NAME}/config-winstorecompat-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${MINGW_FOLDER_NAME}/configure-winstorecompat-output.txt"
      fi
      
      (
        echo
        echo "Running mingw-w64-winstorecompat${MINGW_NAME_SUFFIX} make..."

        # Build.
        run_verbose make -j ${JOBS}

        run_verbose make install-strip

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${MINGW_FOLDER_NAME}/make-winstorecompat-output.txt"
    )

    touch "${mingw_winstorecompat_stamp_file_path}"

  else
    echo "Component mingw-w64-winstorecompat${MINGW_NAME_SUFFIX} already installed."
  fi
}

function build_mingw_libmangle() 
{
  local mingw_libmangle_folder_name="mingw-${MINGW_VERSION}-libmangle${MINGW_NAME_SUFFIX}"

  local mingw_libmangle_stamp_file_path="${STAMPS_FOLDER_PATH}/stamp-${mingw_libmangle_folder_name}-installed"
  if [ ! -f "${mingw_libmangle_stamp_file_path}" ]
  then

    (
      mkdir -p "${BUILD_FOLDER_PATH}/${mingw_libmangle_folder_name}"
      cd "${BUILD_FOLDER_PATH}/${mingw_libmangle_folder_name}"

      CPPFLAGS="${XBB_CPPFLAGS}"
      CFLAGS="${XBB_CFLAGS_NO_W}"
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"

      LDFLAGS="${XBB_LDFLAGS}" 

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
          echo "Running mingw-w64-libmangle${MINGW_NAME_SUFFIX} configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${MINGW_SRC_FOLDER_NAME}/mingw-w64-libraries/libmangle/configure" --help
          fi

          config_options=()
          # Note: native library.
          config_options+=("--prefix=${LIBS_INSTALL_FOLDER_PATH}${MINGW_NAME_SUFFIX}")

          if [ -n "${MINGW_NAME_SUFFIX}" ]
          then
            config_options+=("--build=${BUILD}")
            config_options+=("--host=${BUILD}")
            config_options+=("--target=${BUILD}")
          else
            config_options+=("--build=${BUILD}")
            config_options+=("--host=${HOST}")
            config_options+=("--target=${TARGET}")
          fi

          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${MINGW_SRC_FOLDER_NAME}/mingw-w64-libraries/libmangle/configure" \
            "${config_options[@]}"

         cp "config.log" "${LOGS_FOLDER_PATH}/${MINGW_FOLDER_NAME}/config-libmangle-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${MINGW_FOLDER_NAME}/configure-libmangle-output.txt"
      fi
      
      (
        echo
        echo "Running mingw-w64-libmangle${MINGW_NAME_SUFFIX} make..."

        # Build.
        run_verbose make -j ${JOBS}

        run_verbose make install-strip

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${MINGW_FOLDER_NAME}/make-libmangle-output.txt"
    )

    touch "${mingw_libmangle_stamp_file_path}"

  else
    echo "Component mingw-w64-libmangle${MINGW_NAME_SUFFIX} already installed."
  fi
}


function build_mingw_gendef()
{
  local mingw_gendef_folder_name="mingw-${MINGW_VERSION}-gendef${MINGW_NAME_SUFFIX}"

  local mingw_gendef_stamp_file_path="${STAMPS_FOLDER_PATH}/stamp-${mingw_gendef_folder_name}-installed"
  if [ ! -f "${mingw_gendef_stamp_file_path}" ]
  then

    (
      mkdir -p "${BUILD_FOLDER_PATH}/${mingw_gendef_folder_name}"
      cd "${BUILD_FOLDER_PATH}/${mingw_gendef_folder_name}"

      CPPFLAGS="${XBB_CPPFLAGS}"
      CFLAGS="${XBB_CFLAGS_NO_W}"
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"

      LDFLAGS="${XBB_LDFLAGS_APP_STATIC_GCC}"

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
          echo "Running mingw-w64-gendef${MINGW_NAME_SUFFIX} configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${MINGW_SRC_FOLDER_NAME}/mingw-w64-tools/gendef/configure" --help
          fi

          config_options+=("--prefix=${APP_PREFIX}${MINGW_NAME_SUFFIX}")

          if [ -n "${MINGW_NAME_SUFFIX}" ]
          then
            config_options+=("--build=${BUILD}")
            config_options+=("--host=${BUILD}")
            config_options+=("--target=${BUILD}")
          else
            config_options+=("--build=${BUILD}")
            config_options+=("--host=${HOST}")
            config_options+=("--target=${TARGET}")
          fi

          config_options+=("--with-mangle=${LIBS_INSTALL_FOLDER_PATH}${MINGW_NAME_SUFFIX}")

          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${MINGW_SRC_FOLDER_NAME}/mingw-w64-tools/gendef/configure" \
            "${config_options[@]}"

         cp "config.log" "${LOGS_FOLDER_PATH}/${MINGW_FOLDER_NAME}/config-gendef-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${MINGW_FOLDER_NAME}/configure-gendef-output.txt"
      fi
      
      (
        echo
        echo "Running mingw-w64-gendef${MINGW_NAME_SUFFIX} make..."

        # Build.
        run_verbose make -j ${JOBS}

        run_verbose make install-strip

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${MINGW_FOLDER_NAME}/make-gendef-output.txt"
    )

    touch "${mingw_gendef_stamp_file_path}"

  else
    echo "Component mingw-w64-gendef${MINGW_NAME_SUFFIX} already installed."
  fi
}


function build_mingw_widl()
{
  local mingw_widl_folder_name="mingw-${MINGW_VERSION}-widl${MINGW_NAME_SUFFIX}"

  local mingw_widl_stamp_file_path="${STAMPS_FOLDER_PATH}/stamp-${mingw_widl_folder_name}-installed"
  if [ ! -f "${mingw_widl_stamp_file_path}" ]
  then

    (
      mkdir -p "${BUILD_FOLDER_PATH}/${mingw_widl_folder_name}"
      cd "${BUILD_FOLDER_PATH}/${mingw_widl_folder_name}"

      CPPFLAGS="${XBB_CPPFLAGS}"
      CFLAGS="${XBB_CFLAGS_NO_W}"
      CXXFLAGS="${XBB_CXXFLAGS_NO_W}"

      LDFLAGS="${XBB_LDFLAGS_APP_STATIC_GCC}"

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
          echo "Running mingw-w64-widl${MINGW_NAME_SUFFIX} configure..."

          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${MINGW_SRC_FOLDER_NAME}/mingw-w64-tools/widl/configure" --help
          fi

          config_options=()
          config_options+=("--prefix=${APP_PREFIX}${MINGW_NAME_SUFFIX}")

          if [ -n "${MINGW_NAME_SUFFIX}" ]
          then
            config_options+=("--build=${BUILD}")
            config_options+=("--host=${BUILD}") # Native!
            config_options+=("--target=${TARGET}")

          else
            config_options+=("--build=${BUILD}")
            config_options+=("--host=${HOST}")
            config_options+=("--target=${TARGET}")

            # To remove any target specific prefix and leave only widl.exe.
            config_options+=("--program-prefix=")
          fi

          config_options+=("--with-widl-includedir=${APP_PREFIX}${MINGW_NAME_SUFFIX}/${CROSS_COMPILE_PREFIX}/include")

          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${MINGW_SRC_FOLDER_NAME}/mingw-w64-tools/widl/configure" \
            "${config_options[@]}"

         cp "config.log" "${LOGS_FOLDER_PATH}/${MINGW_FOLDER_NAME}/config-widl-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${MINGW_FOLDER_NAME}/configure-widl-output.txt"
      fi
      
      (
        echo
        echo "Running mingw-w64-widl${MINGW_NAME_SUFFIX} make..."

        # Build.
        run_verbose make -j ${JOBS}

        run_verbose make install-strip

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${MINGW_FOLDER_NAME}/make-widl-output.txt"
    )

    touch "${mingw_widl_stamp_file_path}"

  else
    echo "Component mingw-w64-widl${MINGW_NAME_SUFFIX} already installed."
  fi
}

# -----------------------------------------------------------------------------

# binutils should not be used on Darwin, the build is ok, but
# there are functional issues, due to the different ld/as/etc.

function build_binutils()
{
  # https://www.gnu.org/software/binutils/
  # https://ftp.gnu.org/gnu/binutils/

  # https://archlinuxarm.org/packages/aarch64/binutils/files/PKGBUILD
  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=gdb-git

  # https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=mingw-w64-binutils
  # https://github.com/msys2/MINGW-packages/blob/master/mingw-w64-binutils/PKGBUILD

  # mingw-w64
  # https://github.com/archlinux/svntogit-community/blob/packages/mingw-w64-binutils/trunk/PKGBUILD


  # 2017-07-24, "2.29"
  # 2018-01-28, "2.30"
  # 2018-07-18, "2.31.1"
  # 2019-02-02, "2.32"
  # 2019-10-12, "2.33.1"
  # 2020-02-01, "2.34"
  # 2020-07-24, "2.35"
  # 2020-09-19, "2.35.1"
  # 2021-01-24, "2.36"
  # 2021-01-30, "2.35.2"
  # 2021-02-06, "2.36.1"

  local binutils_version="$1"
  local name_suffix=${2-''}

  local binutils_src_folder_name="binutils-${binutils_version}"
  local binutils_folder_name="${binutils_src_folder_name}${name_suffix}"

  local binutils_archive="${binutils_src_folder_name}.tar.xz"
  local binutils_url="https://ftp.gnu.org/gnu/binutils/${binutils_archive}"

  mkdir -pv "${LOGS_FOLDER_PATH}/${binutils_folder_name}"

  local binutils_patch_file_name="binutils-${binutils_version}.patch"
  local binutils_stamp_file_path="${INSTALL_FOLDER_PATH}/stamp-${binutils_folder_name}-installed"
  if [ ! -f "${binutils_stamp_file_path}" ]
  then

    cd "${SOURCES_FOLDER_PATH}"

    download_and_extract "${binutils_url}" "${binutils_archive}" \
      "${binutils_src_folder_name}" "${binutils_patch_file_name}"

    local binutils_prerequisites_download_stamp_file_path="${STAMPS_FOLDER_PATH}/stamp-${binutils_folder_name}-prerequisites-downloaded"
    if [ ! -f "${binutils_prerequisites_download_stamp_file_path}" ]
    then
      (
        cd "${SOURCES_FOLDER_PATH}/${binutils_src_folder_name}"

        # Fool the script to think it is in the gcc folder.
        mkdir -p gcc
        touch gcc/BASE-VER

        run_verbose bash "${SOURCES_FOLDER_PATH}/${GCC_SRC_FOLDER_NAME}/contrib/download_prerequisites"  --no-verify

        rm -rf gcc

        touch "${binutils_prerequisites_download_stamp_file_path}"
      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${binutils_folder_name}/prerequisites-download-output.txt"
    fi

    (
      mkdir -p "${BUILD_FOLDER_PATH}/${binutils_folder_name}"
      cd "${BUILD_FOLDER_PATH}/${binutils_folder_name}"

      if [ -n "${name_suffix}" ]
      then

        CPPFLAGS="${XBB_CPPFLAGS}"
        CFLAGS="${XBB_CFLAGS_NO_W}"
        CXXFLAGS="${XBB_CXXFLAGS_NO_W}"

        LDFLAGS="${XBB_LDFLAGS_APP_STATIC_GCC} -Wl,-rpath,${XBB_FOLDER_PATH}/lib"

      else
        # To access the newly compiled libraries.
        xbb_activate_installed_dev

        CPPFLAGS="${XBB_CPPFLAGS}"
        CFLAGS="${XBB_CFLAGS_NO_W}"
        CXXFLAGS="${XBB_CXXFLAGS_NO_W}"

        LDFLAGS="${XBB_LDFLAGS_APP_STATIC_GCC}" 

        if [ "${TARGET_PLATFORM}" == "win32" ]
        then
          if [ "${TARGET_ARCH}" == "x32" -o "${TARGET_ARCH}" == "ia32" ]
          then
            # From MSYS2 MINGW
            LDFLAGS+=" -Wl,--large-address-aware"
          fi

          # Used to enable wildcard; inspired from arm-none-eabi-gcc.
          LDFLAGS+=" -Wl,${XBB_FOLDER_PATH}/usr/${CROSS_COMPILE_PREFIX}/lib/CRT_glob.o"
        elif [ "${TARGET_PLATFORM}" == "linux" ]
        then
          LDFLAGS+=" -Wl,-rpath,${LD_LIBRARY_PATH:-${LIBS_INSTALL_FOLDER_PATH}/lib}"
        fi
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
          echo "Running binutils configure..."
      
          if [ "${IS_DEVELOP}" == "y" ]
          then
            run_verbose bash "${SOURCES_FOLDER_PATH}/${binutils_src_folder_name}/configure" --help

            run_verbose bash "${SOURCES_FOLDER_PATH}/${binutils_src_folder_name}/binutils/configure" --help
            run_verbose bash "${SOURCES_FOLDER_PATH}/${binutils_src_folder_name}/bfd/configure" --help
            run_verbose bash "${SOURCES_FOLDER_PATH}/${binutils_src_folder_name}/gas/configure" --help
            run_verbose bash "${SOURCES_FOLDER_PATH}/${binutils_src_folder_name}/ld/configure" --help
          fi

          # ? --without-python --without-curses, --with-expat
          config_options=()

          if [ -n "${name_suffix}" ]
          then

            config_options+=("--prefix=${APP_PREFIX}${name_suffix}")
            config_options+=("--with-sysroot=${APP_PREFIX}${name_suffix}")

            config_options+=("--build=${BUILD}")
            # The bootstrap binaries will run on the build machine.
            config_options+=("--host=${BUILD}")
            config_options+=("--target=${TARGET}")

            config_options+=("--with-pkgversion=${GCC_BOOTSTRAP_BRANDING}")

            config_options+=("--with-libiconv-prefix=${XBB_FOLDER_PATH}")

            config_options+=("--disable-multilib")
            config_options+=("--disable-werror")
            config_options+=("--disable-shared")
            config_options+=("--disable-nls")

            config_options+=("--enable-static")
            config_options+=("--enable-build-warnings=no")
            config_options+=("--enable-lto")
            config_options+=("--enable-plugins")
            config_options+=("--enable-deterministic-archives")
            config_options+=("--enable-libssp")

          else

            config_options+=("--prefix=${APP_PREFIX}")
            config_options+=("--with-sysroot=${APP_PREFIX}")
            # config_options+=("--with-lib-path=/usr/lib:/usr/local/lib")
            config_options+=("--program-suffix=")

            config_options+=("--infodir=${APP_PREFIX_DOC}/info")
            config_options+=("--mandir=${APP_PREFIX_DOC}/man")
            config_options+=("--htmldir=${APP_PREFIX_DOC}/html")
            config_options+=("--pdfdir=${APP_PREFIX_DOC}/pdf")

            config_options+=("--build=${BUILD}")
            config_options+=("--host=${HOST}")
            config_options+=("--target=${TARGET}")

            config_options+=("--with-pkgversion=${BINUTILS_BRANDING}")

            if [ "${TARGET_PLATFORM}" != "linux" ]
            then
              config_options+=("--with-libiconv-prefix=${LIBS_INSTALL_FOLDER_PATH}")
            fi

            config_options+=("--without-system-zlib")
            
            config_options+=("--with-pic")

            # error: debuginfod is missing or unusable
            config_options+=("--without-debuginfod")

            if [ "${TARGET_PLATFORM}" == "win32" ]
            then

              config_options+=("--enable-ld")

              if [ "${TARGET_ARCH}" == "x64" ]
              then
                # From MSYS2 MINGW
                config_options+=("--enable-64-bit-bfd")
              fi

              config_options+=("--enable-shared")
              config_options+=("--enable-shared-libgcc")

            elif [ "${TARGET_PLATFORM}" == "linux" ]
            then

              config_options+=("--enable-ld")

              config_options+=("--disable-shared")
              config_options+=("--disable-shared-libgcc")

            elif [ "${TARGET_PLATFORM}" == "darwin" ]
            then
              echo
              echo "binutils not supported on macOS"
              exit 1
            else
              echo "Oops! Unsupported ${TARGET_PLATFORM}."
              exit 1
            fi

            config_options+=("--enable-static")

            config_options+=("--enable-gold")
            config_options+=("--enable-lto")
            config_options+=("--enable-libssp")
            config_options+=("--enable-relro")
            config_options+=("--enable-threads")
            config_options+=("--enable-interwork")
            config_options+=("--enable-plugins")
            config_options+=("--enable-build-warnings=no")
            config_options+=("--enable-deterministic-archives")
            
            # TODO
            # config_options+=("--enable-nls")
            config_options+=("--disable-nls")

            config_options+=("--disable-new-dtags")

            config_options+=("--disable-multilib")
            config_options+=("--disable-werror")
            config_options+=("--disable-sim")

          fi

          run_verbose bash ${DEBUG} "${SOURCES_FOLDER_PATH}/${binutils_src_folder_name}/configure" \
            ${config_options[@]}
            
          cp "config.log" "${LOGS_FOLDER_PATH}/${binutils_folder_name}/config-log.txt"
        ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${binutils_folder_name}/configure-output.txt"
      fi

      (
        echo
        echo "Running binutils make..."
      
        # Build.
        run_verbose make -j ${JOBS} 

        if [ "${WITH_TESTS}" == "y" ]
        then
          : # run_verbose make check
        fi
      
        # Avoid strip here, it may interfere with patchelf.
        # make install-strip
        run_verbose make install

        if [ -n "${name_suffix}" ]
        then
          
          show_native_libs "${APP_PREFIX}${name_suffix}/bin/${CROSS_COMPILE_PREFIX}-ar"
          show_native_libs "${APP_PREFIX}${name_suffix}/bin/${CROSS_COMPILE_PREFIX}-as"
          show_native_libs "${APP_PREFIX}${name_suffix}/bin/${CROSS_COMPILE_PREFIX}-ld"
          show_native_libs "${APP_PREFIX}${name_suffix}/bin/${CROSS_COMPILE_PREFIX}-strip"
          show_native_libs "${APP_PREFIX}${name_suffix}/bin/${CROSS_COMPILE_PREFIX}-nm"
          show_native_libs "${APP_PREFIX}${name_suffix}/bin/${CROSS_COMPILE_PREFIX}-objcopy"
          show_native_libs "${APP_PREFIX}${name_suffix}/bin/${CROSS_COMPILE_PREFIX}-objdump"
          show_native_libs "${APP_PREFIX}${name_suffix}/bin/${CROSS_COMPILE_PREFIX}-ranlib"
          show_native_libs "${APP_PREFIX}${name_suffix}/bin/${CROSS_COMPILE_PREFIX}-size"
          show_native_libs "${APP_PREFIX}${name_suffix}/bin/${CROSS_COMPILE_PREFIX}-strings"

        else

          if [ "${TARGET_PLATFORM}" == "darwin" ]
          then
            : # rm -rv "${APP_PREFIX}/bin/strip"
          fi

          (
            xbb_activate_tex

            if [ "${WITH_PDF}" == "y" ]
            then
              run_verbose make pdf
              run_verbose make install-pdf
            fi

            if [ "${WITH_HTML}" == "y" ]
            then
              run_verbose make html
              run_verbose make install-html
            fi
          )

          show_libs "${APP_PREFIX}/bin/ar"
          show_libs "${APP_PREFIX}/bin/as"
          show_libs "${APP_PREFIX}/bin/ld"
          show_libs "${APP_PREFIX}/bin/strip"
          show_libs "${APP_PREFIX}/bin/nm"
          show_libs "${APP_PREFIX}/bin/objcopy"
          show_libs "${APP_PREFIX}/bin/objdump"
          show_libs "${APP_PREFIX}/bin/ranlib"
          show_libs "${APP_PREFIX}/bin/size"
          show_libs "${APP_PREFIX}/bin/strings"

        fi

      ) 2>&1 | tee "${LOGS_FOLDER_PATH}/${binutils_folder_name}/make-output.txt"

      copy_license \
        "${SOURCES_FOLDER_PATH}/${binutils_src_folder_name}" \
        "${binutils_folder_name}"

    )

    touch "${binutils_stamp_file_path}"
  else
    echo "Component binutils already installed."
  fi

  if [ -n "${name_suffix}" ]
  then
    :
  else
    tests_add "test_binutils"
  fi
}

function test_binutils()
{
  (
    show_libs "${APP_PREFIX}/bin/ar"
    show_libs "${APP_PREFIX}/bin/as"
    show_libs "${APP_PREFIX}/bin/elfedit"
    show_libs "${APP_PREFIX}/bin/gprof"
    show_libs "${APP_PREFIX}/bin/ld"
    show_libs "${APP_PREFIX}/bin/ld.gold"
    show_libs "${APP_PREFIX}/bin/strip"
    show_libs "${APP_PREFIX}/bin/nm"
    show_libs "${APP_PREFIX}/bin/objcopy"
    show_libs "${APP_PREFIX}/bin/objdump"
    show_libs "${APP_PREFIX}/bin/ranlib"
    show_libs "${APP_PREFIX}/bin/readelf"
    show_libs "${APP_PREFIX}/bin/size"
    show_libs "${APP_PREFIX}/bin/strings"
    show_libs "${APP_PREFIX}/bin/strip"

    echo
    echo "Testing if binutils starts properly..."

    run_app "${APP_PREFIX}/bin/ar" --version
    run_app "${APP_PREFIX}/bin/as" --version
    run_app "${APP_PREFIX}/bin/elfedit" --version
    run_app "${APP_PREFIX}/bin/gprof" --version
    run_app "${APP_PREFIX}/bin/ld" --version
    if [ -f  "${APP_PREFIX}/bin/ld.gold${DOT_EXE}" ]
    then
      # No ld.gold on Windows.
      run_app "${APP_PREFIX}/bin/ld.gold" --version
    fi
    run_app "${APP_PREFIX}/bin/strip" --version
    run_app "${APP_PREFIX}/bin/nm" --version
    run_app "${APP_PREFIX}/bin/objcopy" --version
    run_app "${APP_PREFIX}/bin/objdump" --version
    run_app "${APP_PREFIX}/bin/ranlib" --version
    run_app "${APP_PREFIX}/bin/readelf" --version
    run_app "${APP_PREFIX}/bin/size" --version
    run_app "${APP_PREFIX}/bin/strings" --version
    run_app "${APP_PREFIX}/bin/strip" --version
  )

  echo
  echo "Local binutils tests completed successfuly."
}

# -----------------------------------------------------------------------------
