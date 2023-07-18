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

# Default empty definition, if XBB is available, it should
# redefine it.
function xbb_activate()
{
  :
}

function xbb_activate_dev()
{
  # Add XBB include in front of XBB_CPPFLAGS.
  XBB_CPPFLAGS="-I${XBB_FOLDER_PATH}/include ${XBB_CPPFLAGS}"

  if [ -d "${XBB_FOLDER_PATH}/lib" ]
  then
    # Add XBB lib in front of XBB_LDFLAGS.
    XBB_LDFLAGS="-L${XBB_FOLDER_PATH}/lib ${XBB_LDFLAGS}"
    XBB_LDFLAGS_LIB="-L${XBB_FOLDER_PATH}/lib ${XBB_LDFLAGS_LIB}"
    XBB_LDFLAGS_APP="-L${XBB_FOLDER_PATH}/lib ${XBB_LDFLAGS_APP}"
    XBB_LDFLAGS_APP_STATIC_GCC="-L${XBB_FOLDER_PATH}/lib ${XBB_LDFLAGS_APP_STATIC_GCC}"

    # Add XBB lib in front of PKG_CONFIG_PATH.
    if [ -d "${XBB_FOLDER_PATH}/lib/pkgconfig" ]
    then
      if [ -z "${PKG_CONFIG_PATH}" ]
      then
        PKG_CONFIG_PATH="${XBB_FOLDER_PATH}/lib/pkgconfig"
      else
        PKG_CONFIG_PATH="${XBB_FOLDER_PATH}/lib/pkgconfig:${PKG_CONFIG_PATH}"
      fi
    fi
  fi

  if [ -d "${XBB_FOLDER_PATH}/lib64" ]
  then
    # For 64-bit systems, add XBB lib64 in front of paths.
    XBB_LDFLAGS="-L${XBB_FOLDER_PATH}/lib64 ${XBB_LDFLAGS_LIB}"
    XBB_LDFLAGS_LIB="-L${XBB_FOLDER_PATH}/lib64 ${XBB_LDFLAGS_LIB}"
    XBB_LDFLAGS_APP="-L${XBB_FOLDER_PATH}/lib64 ${XBB_LDFLAGS_APP}"
    XBB_LDFLAGS_APP_STATIC_GCC="-L${XBB_FOLDER_PATH}/lib64 ${XBB_LDFLAGS_APP_STATIC_GCC}"

    if [ -d "${XBB_FOLDER_PATH}/lib64/pkgconfig" ]
    then
      if [ -z "${PKG_CONFIG_PATH}" ]
      then
        PKG_CONFIG_PATH="${XBB_FOLDER_PATH}/lib64/pkgconfig"
      else
        PKG_CONFIG_PATH="${XBB_FOLDER_PATH}/lib64/pkgconfig:${PKG_CONFIG_PATH}"
      fi
    fi
  fi

  export XBB_CPPFLAGS

  export XBB_LDFLAGS
  export XBB_LDFLAGS_LIB
  export XBB_LDFLAGS_APP
  export XBB_LDFLAGS_APP_STATIC_GCC

  export PKG_CONFIG_PATH
}

function xbb_activate_libs()
{
  LD_LIBRARY_PATH=${LD_LIBRARY_PATH:-""}

  if [ -d "${XBB_FOLDER_PATH}/lib" ]
  then
    if [ -z "${LD_LIBRARY_PATH}" ]
    then
      LD_LIBRARY_PATH="${XBB_FOLDER_PATH}/lib"
    else
      LD_LIBRARY_PATH="${XBB_FOLDER_PATH}/lib:${LD_LIBRARY_PATH}"
    fi
  fi

  if [ -d "${XBB_FOLDER_PATH}/lib64" ]
  then
    if [ -z "${LD_LIBRARY_PATH}" ]
    then
      LD_LIBRARY_PATH="${XBB_FOLDER_PATH}/lib64"
    else
      LD_LIBRARY_PATH="${XBB_FOLDER_PATH}/lib64:${LD_LIBRARY_PATH}"
    fi
  fi

  export LD_LIBRARY_PATH
}

# Add the freshly built binaries.
function xbb_activate_installed_bin()
{
  # Add the XBB bin to the PATH.
  PATH="${LIBS_INSTALL_FOLDER_PATH}/bin:${PATH}"

  export PATH
}

# Add the freshly built headers and libraries.
function xbb_activate_installed_dev()
{
  # Add XBB include in front of XBB_CPPFLAGS.
  XBB_CPPFLAGS="-I${LIBS_INSTALL_FOLDER_PATH}/include ${XBB_CPPFLAGS}"

  if [ -d "${LIBS_INSTALL_FOLDER_PATH}/lib" ]
  then
    # Add XBB lib in front of XBB_LDFLAGS.
    XBB_LDFLAGS="-L${LIBS_INSTALL_FOLDER_PATH}/lib ${XBB_LDFLAGS}"
    XBB_LDFLAGS_LIB="-L${LIBS_INSTALL_FOLDER_PATH}/lib ${XBB_LDFLAGS_LIB}"
    XBB_LDFLAGS_APP="-L${LIBS_INSTALL_FOLDER_PATH}/lib ${XBB_LDFLAGS_APP}"
    XBB_LDFLAGS_APP_STATIC_GCC="-L${LIBS_INSTALL_FOLDER_PATH}/lib ${XBB_LDFLAGS_APP_STATIC_GCC}"

    # Add XBB lib in front of PKG_CONFIG_PATH.
    PKG_CONFIG_PATH="${LIBS_INSTALL_FOLDER_PATH}/lib/pkgconfig:${PKG_CONFIG_PATH}"

    # Needed by internal binaries, like the bootstrap compiler, which do not
    # have a rpath.
    if [ -z "${LD_LIBRARY_PATH}" ]
    then
      LD_LIBRARY_PATH="${LIBS_INSTALL_FOLDER_PATH}/lib"
    else
      LD_LIBRARY_PATH="${LIBS_INSTALL_FOLDER_PATH}/lib:${LD_LIBRARY_PATH}"
    fi
  fi

  # Used by libffi, for example.
  if [ -d "${LIBS_INSTALL_FOLDER_PATH}/lib64" ]
  then
    # For 64-bit systems, add XBB lib64 in front of paths.
    XBB_LDFLAGS="-L${LIBS_INSTALL_FOLDER_PATH}/lib64 ${XBB_LDFLAGS_LIB}"
    XBB_LDFLAGS_LIB="-L${LIBS_INSTALL_FOLDER_PATH}/lib64 ${XBB_LDFLAGS_LIB}"
    XBB_LDFLAGS_APP="-L${LIBS_INSTALL_FOLDER_PATH}/lib64 ${XBB_LDFLAGS_APP}"
    XBB_LDFLAGS_APP_STATIC_GCC="-L${LIBS_INSTALL_FOLDER_PATH}/lib64 ${XBB_LDFLAGS_APP_STATIC_GCC}"

    PKG_CONFIG_PATH="${LIBS_INSTALL_FOLDER_PATH}/lib64/pkgconfig:${PKG_CONFIG_PATH}"

    if [ -z "${LD_LIBRARY_PATH}" ]
    then
      LD_LIBRARY_PATH="${LIBS_INSTALL_FOLDER_PATH}/lib64"
    else
      LD_LIBRARY_PATH="${LIBS_INSTALL_FOLDER_PATH}/lib64:${LD_LIBRARY_PATH}"
    fi
  fi

  export XBB_CPPFLAGS

  export XBB_LDFLAGS
  export XBB_LDFLAGS_LIB
  export XBB_LDFLAGS_APP
  export XBB_LDFLAGS_APP_STATIC_GCC

  export PKG_CONFIG_PATH
  export LD_LIBRARY_PATH

  if [ "${IS_DEVELOP}" == "y" ]
  then
    echo
    env | sort
  fi
}

function xbb_activate_tex()
{
  :
}

function copy_custom_files()
{
  :
}

# -----------------------------------------------------------------------------

function get_current_version()
{
  local version_file_path="${scripts_folder_path}/VERSION"
  if [ $# -ge 1 ]
  then
    version_file_path="$1"
  fi

  # Extract only the first line
  cat "${version_file_path}" | sed -e '2,$d'
}

function get_current_package_version()
{
  # Hack to get the 'repo' path.
  local package_file_path="${scripts_folder_path}/../package.json"
  if [ $# -ge 1 ]
  then
    package_file_path="$1"
  fi

  # Extract only the first line
  grep '"version":' "${package_file_path}" | sed -e 's|.*"version": "\(.*\)".*|\1|'
}

function do_config_guess() 
{
  BUILD="$(bash ${helper_folder_path}/config.guess)"
}

function set_xbb_env() 
{
  # Defaults, to ensure the variables are defined.
  PATH="${PATH:-""}"
  LD_LIBRARY_PATH="${LD_LIBRARY_PATH:-""}"

  if [ -f "${HOME}/.local/xbb/xbb-source.sh" ]
  then
    XBB_FOLDER_PATH="${HOME}/.local/xbb"
    echo
    echo "Sourcing ${HOME}/.local/xbb/xbb-source.sh..."
    source "${HOME}/.local/xbb/xbb-source.sh"
  elif [ -f "${HOME}/opt/xbb/xbb-source.sh" ]
  then
    XBB_FOLDER_PATH="${HOME}/opt/xbb"
    echo
    echo "Sourcing ${HOME}/opt/xbb/xbb-source.sh..."
    source "${HOME}/opt/xbb/xbb-source.sh"
  elif [ -f "${HOME}/opt/homebrew/xbb/xbb-source.sh" ]
  then
    XBB_FOLDER_PATH="${HOME}/opt/homebrew/xbb"
    # Deprecated, on macOS it was moved to HOME/opt/xbb
    echo
    echo "Sourcing ${HOME}/opt/homebrew/xbb/xbb-source.sh..."
    source "${HOME}/opt/homebrew/xbb/xbb-source.sh"
  elif [ -f "/opt/xbb/xbb-source.sh" ]
  then
    XBB_FOLDER_PATH="/opt/xbb"
    echo
    echo "Sourcing /opt/xbb/xbb-source.sh..."
    source "/opt/xbb/xbb-source.sh"
  else
    XBB_FOLDER_PATH=""
  fi

  HOST_MACHINE="$(uname -m)"

  TARGET_FOLDER_NAME="${TARGET_PLATFORM}-${TARGET_ARCH}"

  DOT_EXE=""

  # Compute the BUILD/HOST/TARGET for configure.
  CROSS_COMPILE_PREFIX=""
  if [ "${TARGET_PLATFORM}" == "win32" ]
  then

    # Disable test when cross compiling for Windows.
    WITH_TESTS="n"

    # For Windows targets, decide which cross toolchain to use.
    if [ "${TARGET_ARCH}" == "x32" -o "${TARGET_ARCH}" == "ia32" ]
    then
      CROSS_COMPILE_PREFIX="i686-w64-mingw32"
    elif [ "${TARGET_ARCH}" == "x64" ]
    then
      CROSS_COMPILE_PREFIX="x86_64-w64-mingw32"
    else
      echo "Oops! Unsupported TARGET_ARCH=${TARGET_ARCH}."
      exit 1
    fi

    do_config_guess

    DOT_EXE=".exe"

    HOST="${CROSS_COMPILE_PREFIX}"
    TARGET="${HOST}"

  elif [ "${TARGET_PLATFORM}" == "darwin" ]
  then

    do_config_guess

    HOST="${BUILD}"
    TARGET="${HOST}"

  elif [ "${TARGET_PLATFORM}" == "linux" ]
  then

    do_config_guess

    HOST="${BUILD}"
    TARGET="${HOST}"

  else
    echo "Oops! Unsupported TARGET_PLATFORM=${TARGET_PLATFORM}."
    exit 1
  fi

  if [ -f "/.dockerenv" ]
  then
    WORK_FOLDER_PATH="${CONTAINER_WORK_FOLDER_PATH}"
    DOWNLOAD_FOLDER_PATH="${CONTAINER_CACHE_FOLDER_PATH}"
  else
    WORK_FOLDER_PATH="${HOST_WORK_FOLDER_PATH}"
    DOWNLOAD_FOLDER_PATH="${HOST_CACHE_FOLDER_PATH}"
  fi

  # Develop builds use the host folder.
  BUILD_FOLDER_PATH="${WORK_FOLDER_PATH}/${TARGET_FOLDER_NAME}/build"
  if [ -f "/.dockerenv" ]
  then 
    if [ "${IS_DEVELOP}" != "y" ]
    then
      # Docker builds use a temporary folder.
      BUILD_FOLDER_PATH="/tmp/${TARGET_FOLDER_NAME}/build"
    fi
  fi

  LIBS_BUILD_FOLDER_PATH="${BUILD_FOLDER_PATH}/libs"
  mkdir -pv "${LIBS_BUILD_FOLDER_PATH}"

  APP_BUILD_FOLDER_PATH="${BUILD_FOLDER_PATH}/${APP_LC_NAME}"
  # Do it later, only if needed.
  # mkdir -pv "${APP_BUILD_FOLDER_PATH}"

  INSTALL_FOLDER_PATH="${WORK_FOLDER_PATH}/${TARGET_FOLDER_NAME}/install"
  STAMPS_FOLDER_PATH="${INSTALL_FOLDER_PATH}"

  LIBS_INSTALL_FOLDER_PATH="${INSTALL_FOLDER_PATH}/libs"
  # Use explicit include & especially lib, to prevent compiler complaining
  # for missing folders.
  mkdir -pv "${LIBS_INSTALL_FOLDER_PATH}/include"
  mkdir -pv "${LIBS_INSTALL_FOLDER_PATH}/lib"

  APP_INSTALL_FOLDER_PATH="${INSTALL_FOLDER_PATH}/${APP_LC_NAME}"
  mkdir -pv "${APP_INSTALL_FOLDER_PATH}"

  LOGS_FOLDER_NAME=${LOGS_FOLDER_NAME:-"logs"}
  LOGS_FOLDER_PATH="${WORK_FOLDER_PATH}/${TARGET_FOLDER_NAME}/${LOGS_FOLDER_NAME}"
  mkdir -pv "${LOGS_FOLDER_PATH}"

  DEPLOY_FOLDER_NAME=${DEPLOY_FOLDER_NAME:-"deploy"}
  DEPLOY_FOLDER_PATH="${WORK_FOLDER_PATH}/${DEPLOY_FOLDER_NAME}"
  # Do it later, only if needed.
  # mkdir -pv "${DEPLOY_FOLDER_PATH}"

  DISTRO_INFO_NAME=${DISTRO_INFO_NAME:-"distro-info"}
  
  BUILD_GIT_PATH="${WORK_FOLDER_PATH}/build.git"

  # ---------------------------------------------------------------------------

  APP_PREFIX="${INSTALL_FOLDER_PATH}/${APP_LC_NAME}"
  # The documentation location is now the same on all platforms.
  APP_PREFIX_DOC="${APP_PREFIX}/share/doc"

  # ---------------------------------------------------------------------------

  SOURCES_FOLDER_PATH=${SOURCES_FOLDER_PATH:-"${WORK_FOLDER_PATH}/${TARGET_FOLDER_NAME}/sources"}
  mkdir -pv "${SOURCES_FOLDER_PATH}"

  # Empty defaults.
  IS_DEVELOP=${IS_DEVELOP:-""}
  IS_DEBUG=${IS_DEBUG:-""}
  WITH_PDF=${WITH_PDF:-""}
  WITH_HTML=${WITH_HTML:-""}
  WITH_TESTS=${WITH_TESTS:-"y"}

  # Redefine this to "y" to create files that include the architecture.
  HAS_NAME_ARCH=${HAS_NAME_ARCH:-"y"}

  USE_TAR_GZ=${USE_TAR_GZ:-"y"}

  # ---------------------------------------------------------------------------

  export BUILD
  export HOST
  export TARGET
  
  export LANGUAGE="en_US:en"
  export LANG="en_US.UTF-8"
  export LC_ALL="en_US.UTF-8"
  export LC_COLLATE="en_US.UTF-8"
  export LC_CTYPE="UTF-8"
  export LC_MESSAGES="en_US.UTF-8"
  export LC_MONETARY="en_US.UTF-8"
  export LC_NUMERIC="en_US.UTF-8"
  export LC_TIME="en_US.UTF-8"

  export PATH
  export LD_LIBRARY_PATH

  export APP_PREFIX
  export SOURCES_FOLDER_PATH
  export DOT_EXE

  # libtool fails with the Ubuntu /bin/sh.
  export SHELL="/bin/bash"
  export CONFIG_SHELL="/bin/bash"

  echo
  env | sort
}

function set_compiler_env()
{
  if [ ! -z "$(xbb_activate; which "clang++-xbb")" ]
  then
    prepare_clang_env "" "-xbb"
  elif [ ! -z "$(xbb_activate; which "g++-xbb")" ]
  then
    prepare_gcc_env "" "-xbb"
  elif [ ! -z "$(xbb_activate; which "g++-9")" ]
  then
    prepare_gcc_env "" "-9"
  elif [ ! -z "$(xbb_activate; which "g++-8")" ]
  then
    prepare_gcc_env "" "-8"
  elif [ ! -z "$(xbb_activate; which "g++-7")" ]
  then
    prepare_gcc_env "" "-7"
  else
    if [ "${TARGET_PLATFORM}" == "darwin" ]
    then
      prepare_clang_env "" ""
    else
      prepare_gcc_env "" ""
    fi
  fi

  if [ "${TARGET_PLATFORM}" == "win32" ]
  then
    export NATIVE_CC=${CC}
    export NATIVE_CXX=${CXX}
  fi

  (
    xbb_activate

    echo

    which ${CC}
    ${CC} --version

    which make
    make --version
  )
}

function set_xbb_extras()
{
  # ---------------------------------------------------------------------------

  XBB_CPPFLAGS=""

  XBB_CFLAGS="-ffunction-sections -fdata-sections -pipe"
  XBB_CXXFLAGS="-ffunction-sections -fdata-sections -pipe"

  if [ "${TARGET_ARCH}" == "x64" -o "${TARGET_ARCH}" == "x32" -o "${TARGET_ARCH}" == "ia32" ]
  then
    XBB_CFLAGS+=" -m${TARGET_BITS}"
    XBB_CXXFLAGS+=" -m${TARGET_BITS}"
  fi

  XBB_LDFLAGS=""

  if [ "${IS_DEBUG}" == "y" ]
  then
    XBB_CFLAGS+=" -g -O0"
    XBB_CXXFLAGS+=" -g -O0"
    XBB_LDFLAGS+=" -g -O0"
  else
    XBB_CFLAGS+=" -O2"
    XBB_CXXFLAGS+=" -O2"
    XBB_LDFLAGS+=" -O2"
  fi

  if [ "${IS_DEVELOP}" == "y" ]
  then
    XBB_LDFLAGS+=" -v"
  fi

  if [ "${TARGET_PLATFORM}" == "linux" ]
  then
    SHLIB_EXT="so"

    # Do not add -static here, it fails.
    # Do not try to link pthread statically, it must match the system glibc.
    XBB_LDFLAGS_LIB="${XBB_LDFLAGS}"
    XBB_LDFLAGS_APP="${XBB_LDFLAGS} -Wl,--gc-sections"
    XBB_LDFLAGS_APP_STATIC_GCC="${XBB_LDFLAGS_APP} -static-libgcc -static-libstdc++"
  elif [ "${TARGET_PLATFORM}" == "darwin" ]
  then
    SHLIB_EXT="dylib"

    export MACOSX_DEPLOYMENT_TARGET="10.10"

    if [[ "${CC}" =~ *clang* ]]
    then
      XBB_CFLAGS+=" -mmacosx-version-min=${MACOSX_DEPLOYMENT_TARGET}"
      XBB_CXXFLAGS+=" -mmacosx-version-min=${MACOSX_DEPLOYMENT_TARGET}"
    fi

    # Note: macOS linker ignores -static-libstdc++, so 
    # libstdc++.6.dylib should be handled.
    XBB_LDFLAGS+=" -Wl,-macosx_version_min,${MACOSX_DEPLOYMENT_TARGET}"

    XBB_LDFLAGS_LIB="${XBB_LDFLAGS}"
    XBB_LDFLAGS_APP="${XBB_LDFLAGS} -Wl,-dead_strip"
    XBB_LDFLAGS_APP_STATIC_GCC="${XBB_LDFLAGS_APP} -static-libstdc++"
    if [[ "${CC}" =~ *gcc* ]]
    then
      XBB_LDFLAGS_APP_STATIC_GCC+=" -static-libgcc"
    fi
  elif [ "${TARGET_PLATFORM}" == "win32" ]
  then
    SHLIB_EXT="dll"

    # Note: use this explcitly in the application.
    # prepare_gcc_env "${CROSS_COMPILE_PREFIX}-"

    # CRT_glob is from Arm script
    # -static avoids libwinpthread-1.dll 
    # -static-libgcc avoids libgcc_s_sjlj-1.dll 
    XBB_LDFLAGS_LIB="${XBB_LDFLAGS}"
    XBB_LDFLAGS_APP="${XBB_LDFLAGS} -Wl,--gc-sections"
    XBB_LDFLAGS_APP_STATIC_GCC="${XBB_LDFLAGS_APP} -static-libgcc -static-libstdc++"
  else
    echo "Oops! Unsupported TARGET_PLATFORM=${TARGET_PLATFORM}."
    exit 1
  fi

  XBB_CFLAGS_NO_W="${XBB_CFLAGS} -w"
  XBB_CXXFLAGS_NO_W="${XBB_CXXFLAGS} -w"

  set +u
  if [ ! -z "${XBB_FOLDER_PATH}" -a -x "${XBB_FOLDER_PATH}/bin/pkg-config-verbose" ]
  then
    PKG_CONFIG="${XBB_FOLDER_PATH}/bin/pkg-config-verbose"
  else
    PKG_CONFIG="$(which pkg-config)"
  fi
  set -u

  # Hopefully defining it empty would be enough...
  PKG_CONFIG_PATH=${PKG_CONFIG_PATH:-""}

  # Prevent pkg-config to search the system folders (configured in the
  # pkg-config at build time).
  PKG_CONFIG_LIBDIR=${PKG_CONFIG_LIBDIR:-""}

  set +u
  echo
  echo "CC=${CC}"
  echo "CXX=${CXX}"
  echo "XBB_CPPFLAGS=${XBB_CPPFLAGS}"
  echo "XBB_CFLAGS=${XBB_CFLAGS}"
  echo "XBB_CXXFLAGS=${XBB_CXXFLAGS}"

  echo "XBB_LDFLAGS_LIB=${XBB_LDFLAGS_LIB}"
  echo "XBB_LDFLAGS_APP=${XBB_LDFLAGS_APP}"
  echo "XBB_LDFLAGS_APP_STATIC_GCC=${XBB_LDFLAGS_APP_STATIC_GCC}"

  echo "PKG_CONFIG=${PKG_CONFIG}"
  echo "PKG_CONFIG_PATH=${PKG_CONFIG_PATH}"
  echo "PKG_CONFIG_LIBDIR=${PKG_CONFIG_LIBDIR}"
  set -u

  # ---------------------------------------------------------------------------

  export SHLIB_EXT

  # CC & co were exported by prepare_gcc_env.
  export XBB_CPPFLAGS

  export XBB_CFLAGS
  export XBB_CXXFLAGS

  export XBB_CFLAGS_NO_W
  export XBB_CXXFLAGS_NO_W

  export XBB_LDFLAGS
  export XBB_LDFLAGS_LIB
  export XBB_LDFLAGS_APP
  export XBB_LDFLAGS_APP_STATIC_GCC

  export PKG_CONFIG
  export PKG_CONFIG_PATH
  export PKG_CONFIG_LIBDIR
}

function prepare_gcc_env()
{
  local prefix="$1"

  local suffix
  if [ $# -ge 2 ]
  then
    suffix="$2"
  else
    suffix=""
  fi

  unset_compiler_env

  export CC="${prefix}gcc${suffix}"
  export CXX="${prefix}g++${suffix}"

  # These are the special GCC versions, not the binutils ones.
  export AR="${prefix}gcc-ar${suffix}"
  export NM="${prefix}gcc-nm${suffix}"
  export RANLIB="${prefix}gcc-ranlib${suffix}"

  # From binutils.
  export AS="${prefix}as"
  export DLLTOOL="${prefix}dlltool"
  export LD="${prefix}ld"
  export OBJCOPY="${prefix}objcopy"
  export OBJDUMP="${prefix}objdump"
  export READELF="${prefix}readelf"
  export SIZE="${prefix}size"
  export STRIP="${prefix}strip"
  export WINDRES="${prefix}windres"
  export WINDMC="${prefix}windmc"
  export RC="${prefix}windres"

  set_xbb_extras
}

function unset_compiler_env()
{
  unset CC
  unset CXX
  unset AR
  unset AS
  unset DLLTOOL
  unset LD
  unset NM
  unset OBJCOPY
  unset OBJDUMP
  unset RANLIB
  unset READELF
  unset SIZE
  unset STRIP
  unset WINDRES
  unset WINDMC
  unset RC

  unset XBB_CPPFLAGS

  unset XBB_CFLAGS
  unset XBB_CXXFLAGS

  unset XBB_CFLAGS_NO_W
  unset XBB_CXXFLAGS_NO_W

  unset XBB_LDFLAGS
  unset XBB_LDFLAGS_LIB
  unset XBB_LDFLAGS_APP
  unset XBB_LDFLAGS_APP_STATIC_GCC
}

function prepare_clang_env()
{
  local prefix="$1"

  local suffix
  if [ $# -ge 2 ]
  then
    suffix="$2"
  else
    suffix=""
  fi

  unset_compiler_env

  export CC="${prefix}clang${suffix}"
  export CXX="${prefix}clang++${suffix}"

  export AR="${prefix}ar"
  export AS="${prefix}as"
  # export DLLTOOL="${prefix}dlltool"
  export LD="${prefix}ld"
  export NM="${prefix}nm"
  # export OBJCOPY="${prefix}objcopy"
  export OBJDUMP="${prefix}objdump"
  export RANLIB="${prefix}ranlib"
  # export READELF="${prefix}readelf"
  export SIZE="${prefix}size"
  export STRIP="${prefix}strip"
  # export WINDRES="${prefix}windres"
  # export WINDMC="${prefix}windmc"
  # export RC="${prefix}windres"

  set_xbb_extras
}

# -----------------------------------------------------------------------------

function do_actions()
{
  if [ "${ACTION}" == "clean" ]
  then
    echo

    if [ "${IS_NATIVE}" == "y" ]
    then
      echo "Removing the ${TARGET_FOLDER_NAME} build and install ${APP_LC_NAME} folders..."

      rm -rf "${HOST_WORK_FOLDER_PATH}/${TARGET_FOLDER_NAME}/build/${APP_LC_NAME}"
      rm -rf "${HOST_WORK_FOLDER_PATH}/${TARGET_FOLDER_NAME}/install/${APP_LC_NAME}"
    elif [ ! -z "${DO_BUILD_WIN32}${DO_BUILD_WIN64}${DO_BUILD_LINUX32}${DO_BUILD_LINUX64}${DO_BUILD_OSX}" ]
    then
      if [ "${DO_BUILD_WIN32}" == "y" ]
      then
        echo "Removing the win32 32-bit build and install ${APP_LC_NAME} folders..."

        rm -rf "${HOST_WORK_FOLDER_PATH}/win32-x32/build/${APP_LC_NAME}"
        rm -rf "${HOST_WORK_FOLDER_PATH}/win32-x32/install/${APP_LC_NAME}"
        rm -rf "${HOST_WORK_FOLDER_PATH}/win32-ia32/build/${APP_LC_NAME}"
        rm -rf "${HOST_WORK_FOLDER_PATH}/win32-ia32/install/${APP_LC_NAME}"
      fi
      if [ "${DO_BUILD_WIN64}" == "y" ]
      then
        echo "Removing the win32 64-bit build and install ${APP_LC_NAME} folders..."

        rm -rf "${HOST_WORK_FOLDER_PATH}/win32-x64/build/${APP_LC_NAME}"
        rm -rf "${HOST_WORK_FOLDER_PATH}/win32-x64/install/${APP_LC_NAME}"
      fi
      if [ "${DO_BUILD_LINUX32}" == "y" ]
      then
        echo "Removing the linux 32-bit build and install ${APP_LC_NAME} folders..."

        rm -rf "${HOST_WORK_FOLDER_PATH}/linux-x32/build/${APP_LC_NAME}"
        rm -rf "${HOST_WORK_FOLDER_PATH}/linux-x32/install/${APP_LC_NAME}"
        rm -rf "${HOST_WORK_FOLDER_PATH}/linux-ia32/build/${APP_LC_NAME}"
        rm -rf "${HOST_WORK_FOLDER_PATH}/linux-ia32/install/${APP_LC_NAME}"
      fi
      if [ "${DO_BUILD_LINUX64}" == "y" ]
      then
        echo "Removing the linux 64-bit build and install ${APP_LC_NAME} folders..."

        rm -rf "${HOST_WORK_FOLDER_PATH}/linux-x64/build/${APP_LC_NAME}"
        rm -rf "${HOST_WORK_FOLDER_PATH}/linux-x64/install/${APP_LC_NAME}"
      fi
      if [ "${DO_BUILD_OSX}" == "y" ]
      then
        echo "Removing the darwin build and install ${APP_LC_NAME} folders..."

        rm -rf "${HOST_WORK_FOLDER_PATH}/darwin-x64/build/${APP_LC_NAME}"
        rm -rf "${HOST_WORK_FOLDER_PATH}/darwin-x64/install/${APP_LC_NAME}"
      fi
    else
      echo "Removing the ${HOST_NODE_PLATFORM}-${HOST_NODE_ARCH} build and install ${APP_LC_NAME} folders..."

      rm -rf "${HOST_WORK_FOLDER_PATH}/${HOST_NODE_PLATFORM}-${HOST_NODE_ARCH}/build/${APP_LC_NAME}"
      rm -rf "${HOST_WORK_FOLDER_PATH}/${HOST_NODE_PLATFORM}-${HOST_NODE_ARCH}/install/${APP_LC_NAME}"
    fi
  fi

  if [ "${ACTION}" == "cleanlibs" ]
  then
    echo

    if [ "${IS_NATIVE}" == "y" ]
    then
      echo "Removing the ${TARGET_FOLDER_NAME} build and install libs folders..."

      rm -rf "${HOST_WORK_FOLDER_PATH}/${TARGET_FOLDER_NAME}/build/libs"
      rm -rf "${HOST_WORK_FOLDER_PATH}/${TARGET_FOLDER_NAME}/install/libs"
      rm -rf "${HOST_WORK_FOLDER_PATH}/${TARGET_FOLDER_NAME}/install"/stamp-*-installed
    elif [ ! -z "${DO_BUILD_WIN32}${DO_BUILD_WIN64}${DO_BUILD_LINUX32}${DO_BUILD_LINUX64}${DO_BUILD_OSX}" ]
    then
      if [ "${DO_BUILD_WIN32}" == "y" ]
      then
        echo "Removing the win32 32-bit build and install libs folders..."

        rm -rf "${HOST_WORK_FOLDER_PATH}/win32-x32/build/libs"
        rm -rf "${HOST_WORK_FOLDER_PATH}/win32-x32/install/libs"
        rm -rf "${HOST_WORK_FOLDER_PATH}/win32-x32/install"/stamp-*-installed
        rm -rf "${HOST_WORK_FOLDER_PATH}/win32-ia32/build/libs"
        rm -rf "${HOST_WORK_FOLDER_PATH}/win32-ia32/install/libs"
        rm -rf "${HOST_WORK_FOLDER_PATH}/win32-ia32/install"/stamp-*-installed
      fi
      if [ "${DO_BUILD_WIN64}" == "y" ]
      then
        echo "Removing the win32 64-bit build and install libs folders..."

        rm -rf "${HOST_WORK_FOLDER_PATH}/win32-x64/build/libs"
        rm -rf "${HOST_WORK_FOLDER_PATH}/win32-x64/install/libs"
        rm -rf "${HOST_WORK_FOLDER_PATH}/win32-x64/install"/stamp-*-installed
      fi
      if [ "${DO_BUILD_LINUX32}" == "y" ]
      then
        echo "Removing the linux 32-bit build and install libs folders..."

        rm -rf "${HOST_WORK_FOLDER_PATH}/linux-x32/build/libs"
        rm -rf "${HOST_WORK_FOLDER_PATH}/linux-x32/install/libs"
        rm -rf "${HOST_WORK_FOLDER_PATH}/linux-x32/install"/stamp-*-installed
        rm -rf "${HOST_WORK_FOLDER_PATH}/linux-ia32/build/libs"
        rm -rf "${HOST_WORK_FOLDER_PATH}/linux-ia32/install/libs"
        rm -rf "${HOST_WORK_FOLDER_PATH}/linux-ia32/install"/stamp-*-installed
      fi
      if [ "${DO_BUILD_LINUX64}" == "y" ]
      then
        echo "Removing the linux 64-bit build and install libs folders..."

        rm -rf "${HOST_WORK_FOLDER_PATH}/linux-x64/build/libs"
        rm -rf "${HOST_WORK_FOLDER_PATH}/linux-x64/install/libs"
        rm -rf "${HOST_WORK_FOLDER_PATH}/linux-x64/install"/stamp-*-installed
      fi
      if [ "${DO_BUILD_OSX}" == "y" ]
      then
        echo "Removing the darwin build and install libs folders..."

        rm -rf "${HOST_WORK_FOLDER_PATH}/darwin-x64/build/libs"
        rm -rf "${HOST_WORK_FOLDER_PATH}/darwin-x64/install/libs"
        rm -rf "${HOST_WORK_FOLDER_PATH}/darwin-x64/install"/stamp-*-installed
      fi
    else
      echo "Removing the ${HOST_NODE_PLATFORM}-${HOST_NODE_ARCH} build and install libs folders..."

      rm -rf "${HOST_WORK_FOLDER_PATH}/${HOST_NODE_PLATFORM}-${HOST_NODE_ARCH}/build/libs"
      rm -rf "${HOST_WORK_FOLDER_PATH}/${HOST_NODE_PLATFORM}-${HOST_NODE_ARCH}/install/libs"
      rm -rf "${HOST_WORK_FOLDER_PATH}/${HOST_NODE_PLATFORM}-${HOST_NODE_ARCH}/install"/stamp-*-installed
    fi
  fi

  if [ "${ACTION}" == "cleanall" ]
  then
    echo
    if [ "${IS_NATIVE}" == "y" ]
    then
      echo "Removing the ${TARGET_FOLDER_NAME} folder..."
  
      rm -rf "${HOST_WORK_FOLDER_PATH}/${TARGET_FOLDER_NAME}"
    elif [ ! -z "${DO_BUILD_WIN32}${DO_BUILD_WIN64}${DO_BUILD_LINUX32}${DO_BUILD_LINUX64}${DO_BUILD_OSX}" ]
    then
      if [ "${DO_BUILD_WIN32}" == "y" ]
      then
        echo "Removing the win32 32-bit folder..."

        rm -rf "${HOST_WORK_FOLDER_PATH}/win32-x32"
        rm -rf "${HOST_WORK_FOLDER_PATH}/win32-ia32"
      fi
      if [ "${DO_BUILD_WIN64}" == "y" ]
      then
        echo "Removing the win32 64-bit folder..."

        rm -rf "${HOST_WORK_FOLDER_PATH}/win32-x64"
      fi
      if [ "${DO_BUILD_LINUX32}" == "y" ]
      then
        echo "Removing the linux 32-bit folder..."

        rm -rf "${HOST_WORK_FOLDER_PATH}/linux-x32"
        rm -rf "${HOST_WORK_FOLDER_PATH}/linux-ia32"
      fi
      if [ "${DO_BUILD_LINUX64}" == "y" ]
      then
        echo "Removing the linux 64-bit folder..."

        rm -rf "${HOST_WORK_FOLDER_PATH}/linux-x64"
      fi
      if [ "${DO_BUILD_OSX}" == "y" ]
      then
        echo "Removing the darwin folder..."

        rm -rf "${HOST_WORK_FOLDER_PATH}/darwin-x64"
      fi
    else
      echo "Removing the ${HOST_NODE_PLATFORM}-${HOST_NODE_ARCH} folder..."

      rm -rf "${HOST_WORK_FOLDER_PATH}/${HOST_NODE_PLATFORM}-${HOST_NODE_ARCH}"
    fi
  fi

  if [ "${ACTION}" == "clean" -o "${ACTION}" == "cleanlibs" -o "${ACTION}" == "cleanall" ]
  then
    echo
    echo "Clean completed. Proceed with a regular build."

    exit 0
  fi

  # Not used for native buils. Otherwise the names of the docker images
  # must be set.
  if [ "${ACTION}" == "preload-images" ]
  then
    host_start_timer

    host_prepare_docker

    echo
    echo "Check/Preload Docker images..."

    if [ "${HOST_MACHINE}" == "aarch64" -o "${HOST_MACHINE}" == "armv7l" -o "${HOST_MACHINE}" == "armv8l" ]
    then
      echo
      docker run --interactive --tty "${docker_linux_arm64_image}" \
        lsb_release --description --short

      echo
      docker run --interactive --tty "${docker_linux_arm32_image}" \
        lsb_release --description --short
    else
      echo
      docker run --interactive --tty "${docker_linux64_image}" \
        lsb_release --description --short

      echo
      docker run --interactive --tty "${docker_linux32_image}" \
        lsb_release --description --short
    fi

    echo
    docker images

    host_stop_timer

    exit 0
  fi
}

# -----------------------------------------------------------------------------

function echo_develop()
{
  if [ "${IS_DEVELOP}" == "y" ]
  then
    echo "$@"
  fi
}

# -----------------------------------------------------------------------------

function run_verbose()
{
  # Does not include the .exe extension.
  local app_path=$1
  shift

  echo
  echo "[${app_path} $@]"
  "${app_path}" "$@" 2>&1
}

function run_verbose_develop()
{
  # Does not include the .exe extension.
  local app_path=$1
  shift

  if [ "${IS_DEVELOP}" == "y" ]
  then
    echo
    echo "[${app_path} $@]"
  fi
  "${app_path}" "$@" 2>&1
}

function run_verbose_timed()
{
  # Does not include the .exe extension.
  local app_path=$1
  shift

  echo
  echo "[${app_path} $@]"
  time "${app_path}" "$@" 2>&1
}

function run_app()
{
  # Does not include the .exe extension.
  local app_path=$1
  shift

  if [ "${TARGET_PLATFORM}" == "linux" ]
  then
    run_verbose "${app_path}" "$@"
  elif [ "${TARGET_PLATFORM}" == "darwin" ]
  then
    run_verbose "${app_path}" "$@"
  elif [ "${TARGET_PLATFORM}" == "win32" ]
  then
    if [ -x "${app_path}" ]
    then
      # When testing native variants, like llvm.
      run_verbose "${app_path}" "$@"
      return
    fi

    if [ "$(uname -o)" == "Msys" ]
    then
      run_verbose "${app_path}.exe" "$@"
      return
    fi

    local wsl_path=$(which wsl.exe 2>/dev/null)
    if [ ! -z "${wsl_path}" ]
    then
      run_verbose "${app_path}.exe" "$@"
      return
    fi

    (
      xbb_activate
      
      local wine_path=$(which wine 2>/dev/null)
      if [ ! -z "${wine_path}" ]
      then
        if [ -f "${app_path}.exe" ]
        then
          run_verbose wine "${app_path}.exe" "$@"
        else
          echo "${app_path}.exe not found"
          exit 1
        fi
      else
        echo "Install wine if you want to run the .exe binaries on Linux."
      fi
    )

  else
    echo "Oops! Unsupported TARGET_PLATFORM=${TARGET_PLATFORM}."
    exit 1
  fi
}

function run_app_silent()
{
  # Does not include the .exe extension.
  local app_path=$1
  shift

  if [ "${TARGET_PLATFORM}" == "linux" ]
  then
    "${app_path}" "$@" 2>&1
  elif [ "${TARGET_PLATFORM}" == "darwin" ]
  then
    "${app_path}" "$@" 2>&1
  elif [ "${TARGET_PLATFORM}" == "win32" ]
  then
    if [ "$(uname -o)" == "Msys" ]
    then
      "${app_path}.exe" "$@"
      return
    fi

    local wsl_path=$(which wsl.exe 2>/dev/null)
    if [ ! -z "${wsl_path}" ]
    then
      "${app_path}.exe" "$@" 2>&1
      return
    fi
    (
      xbb_activate

      local wine_path=$(which wine 2>/dev/null)
      if [ ! -z "${wine_path}" ]
      then
        if [ -f "${app_path}.exe" ]
        then
          wine "${app_path}.exe" "$@" 2>&1
        else
          echo "${app_path}.exe not found"
          exit 1
        fi
      else
        echo "Install wine if you want to run the .exe binaries on Linux."
      fi
    )

  else
    echo "Oops! Unsupported TARGET_PLATFORM=${TARGET_PLATFORM}."
    exit 1
  fi
}

function run_app_exit()
{
  local expected_exit_code=$1
  shift
  local app_path=$1
  shift
  if [ "${node_platform}" == "win32" ]
  then
    app_path+='.exe'
  fi
  
  (
    set +e
    echo
    echo "${app_path} $@"
    "${app_path}" "$@" 2>&1
    local actual_exit_code=$?
    echo "exit(${actual_exit_code})"
    set -e
    if [ ${actual_exit_code} -ne ${expected_exit_code} ]
    then
      exit ${actual_exit_code}
    fi
  )
}

function test_expect()
{
  local app_name="$1"
  local expected="$2"

  show_libs "${app_name}"

  # Remove the trailing CR present on Windows.
  local output
  if [ "${app_name:0:1}" == "/" ]
  then
    output="$(run_app_silent "${app_name}" "$@" | sed 's/\r$//')"
  else
    output="$(run_app_silent "./${app_name}" "$@" | sed 's/\r$//')"
  fi

  if [ "x${output}x" == "x${expected}x" ]
  then
    echo
    echo "Test \"${app_name}\" passed :-)"
  else
    echo "expected ${#expected}: \"${expected}\""
    echo "got ${#output}: \"${output}\""
    echo
    exit 1
  fi
}

# -----------------------------------------------------------------------------

function show_libs()
{
  # Does not include the .exe extension.
  local app_path=$1
  shift

  (
    xbb_activate

    if [ "${TARGET_PLATFORM}" == "linux" ]
    then
      run_verbose ls -l "${app_path}"
      echo
      echo "[readelf -d ${app_path} | egrep ...]"
      # Ignore errors in case it is not using shared libraries.
      set +e 
      readelf -d "${app_path}" | egrep '(SONAME)' || true
      readelf -d "${app_path}" | egrep '(RUNPATH|RPATH)' || true
      readelf -d "${app_path}" | egrep '(NEEDED)' || true
      echo
      echo "[ldd -v ${app_path}]"
      ldd -v "${app_path}" || true
      set -e
    elif [ "${TARGET_PLATFORM}" == "darwin" ]
    then
      run_verbose ls -l "${app_path}"
      echo
      echo "[otool -L ${app_path}]"
      otool -L "${app_path}"
    elif [ "${TARGET_PLATFORM}" == "win32" ]
    then
      if is_elf "${app_path}"
      then
        run_verbose ls -l "${app_path}"
        echo
        echo "[readelf -d ${app_path} | egrep ...]"
        # Ignore errors in case it is not using shared libraries.
        set +e 
        readelf -d "${app_path}" | egrep '(SONAME)' || true
        readelf -d "${app_path}" | egrep '(RUNPATH|RPATH)' || true
        readelf -d "${app_path}" | egrep '(NEEDED)' || true
        echo
        echo "[ldd -v ${app_path}]"
        ldd -v "${app_path}" || true
        set -e
      else
        if [ -f "${app_path}" ]
        then
          run_verbose ls -l "${app_path}"
          echo
          echo "[${CROSS_COMPILE_PREFIX}-objdump -x ${app_path}]"
          ${CROSS_COMPILE_PREFIX}-objdump -x ${app_path} | grep -i 'DLL Name' || true
        elif [ -f "${app_path}.exe" ]
        then
          run_verbose ls -l "${app_path}.exe"
          echo
          echo "[${CROSS_COMPILE_PREFIX}-objdump -x ${app_path}.exe]"
          ${CROSS_COMPILE_PREFIX}-objdump -x ${app_path}.exe | grep -i 'DLL Name' || true
        else
          echo
          echo "${app_path} "
        fi
      fi
    else
      echo "Oops! Unsupported TARGET_PLATFORM=${TARGET_PLATFORM}."
      exit 1
    fi
  )
}

function show_native_libs()
{
  # Does not include the .exe extension.
  local app_path=$1
  shift

  (
    xbb_activate

    echo
    echo "[readelf -d ${app_path} | egrep ...]"
    # Ignore errors in case it is not using shared libraries.
    set +e 
    readelf -d "${app_path}" | egrep '(SONAME)' || true
    readelf -d "${app_path}" | egrep '(RUNPATH|RPATH)' || true
    readelf -d "${app_path}" | egrep '(NEEDED)' || true
    echo
    echo "[ldd -v ${app_path}]"
    ldd -v "${app_path}" || true
    set -e
  )
}

# -----------------------------------------------------------------------------

function do_patch()
{
  if [ ! -z "$1" ]
  then
    local patch_file_name="$1"
    local patch_path="${BUILD_GIT_PATH}/patches/${patch_file_name}"
    if [ -f "${patch_path}" ]
    then
      echo "Applying \"${patch_path}\"..."
      if [[ ${patch_path} == *.patch.diff ]]
      then
        # Sourcetree creates patch.diff files, which require -p1.
        run_verbose_develop patch -p1 < "${patch_path}"
      else
        # Manually created patches.
        run_verbose_develop patch -p0 < "${patch_path}"
      fi
    fi
  fi
}

function extract()
{
  local archive_name="$1"
  local folder_name="$2"
  # local patch_file_name="$3"
  local pwd="$(pwd)"

  if [ ! -d "${folder_name}" ]
  then
    (
      xbb_activate

      echo
      echo "Extracting \"${archive_name}\" -> \"${pwd}/${folder_name}\"..."
      if [[ "${archive_name}" == *zip ]]
      then
        run_verbose_develop unzip "${archive_name}" 
      else
        # On macOS Docker seems to have a problem and extracting symlinks
        # fails, but a second atempt is successful.
        if [ ! -z "${DEBUG}" ]
        then
          run_verbose_develop tar -x -v -f "${archive_name}" --no-same-owner || tar -x -v -f "${archive_name}" --no-same-owner
        else
          run_verbose_develop tar -x -f "${archive_name}" --no-same-owner || tar -x -f "${archive_name}" --no-same-owner
        fi
      fi

      if [ $# -ge 3 ]
      then
        cd "${folder_name}"
        do_patch "$3"
      fi
    )
  else
    echo "Folder \"${pwd}/${folder_name}\" already present."
  fi
}


function download()
{
  local url="$1"
  local archive_name="$2"

  if [ ! -f "${DOWNLOAD_FOLDER_PATH}/${archive_name}" ]
  then
    (
      xbb_activate

      echo
      echo "Downloading \"${archive_name}\" from \"${url}\"..."
      rm -f "${DOWNLOAD_FOLDER_PATH}/${archive_name}.download"
      mkdir -pv "${DOWNLOAD_FOLDER_PATH}"
      run_verbose curl --fail -L -o "${DOWNLOAD_FOLDER_PATH}/${archive_name}.download" "${url}"
      mv "${DOWNLOAD_FOLDER_PATH}/${archive_name}.download" "${DOWNLOAD_FOLDER_PATH}/${archive_name}"
    )
  else
    echo "File \"${DOWNLOAD_FOLDER_PATH}/${archive_name}\" already downloaded."
  fi
}

# $4 is the patch file name
function download_and_extract()
{
  local url="$1"
  local archive_name="$2"
  local folder_name="$3"

  download "${url}" "${archive_name}"
  if [ $# -ge 4 ]
  then
    extract "${DOWNLOAD_FOLDER_PATH}/${archive_name}" "${folder_name}" "$4"
  else
    extract "${DOWNLOAD_FOLDER_PATH}/${archive_name}" "${folder_name}"
  fi
}

function git_clone()
{
  local url="$1"
  local branch="$2"
  local commit="$3"
  local folder_name="$4"

  (
    echo
    echo "Cloning \"${folder_name}\" from \"${url}\"..."
    run_verbose git clone --branch="${branch}" "${url}" "${folder_name}"
    if [ -n "${commit}" ]
    then
      cd "${folder_name}"
      run_verbose git checkout -qf "${commit}"
    fi
  )
}

# Copy the build files to the Work area, to make them easily available. 
function copy_build_git()
{
  rm -rf "${HOST_WORK_FOLDER_PATH}/build.git"
  mkdir -pv "${HOST_WORK_FOLDER_PATH}/build.git"
  echo ${scripts_folder_path}
  cp -r "$(dirname ${scripts_folder_path})"/* "${HOST_WORK_FOLDER_PATH}/build.git"
  rm -rf "${HOST_WORK_FOLDER_PATH}/build.git/scripts/helper/.git"
  rm -rf "${HOST_WORK_FOLDER_PATH}/build.git/scripts/helper/build-helper.sh"
}

# -----------------------------------------------------------------------------

# Output the result of an elaborate find.
function find_binaries()
{
  local folder_path
  if [ $# -ge 1 ]
  then
    folder_path="$1"
  else
    folder_path="${APP_PREFIX}"
  fi

  if [ "${TARGET_PLATFORM}" == "win32" ]
  then
    find "${folder_path}" \( -name \*.exe -o -name \*.dll -o -name \*.pyd \)
  elif [ "${TARGET_PLATFORM}" == "darwin" ]
  then
    find "${folder_path}" -name \* -type f ! -iname "*.cmake" ! -iname "*.txt" ! -iname "*.rst" ! -iname "*.html" ! -iname "*.json" ! -iname "*.py" ! -iname "*.pyc" ! -iname "*.h" ! -iname "*.xml" ! -iname "*.a" ! -iname "*.la" ! -iname "*.spec" ! -iname "*.specs" ! -iname "*.decTest" ! -iname "*.exe" | grep -v "/ldscripts/" | grep -v "/doc/" | grep -v "/locale/" | grep -v "/include/" | grep -v 'MacOSX.*\.sdk' | grep -v 'macOS.*\.sdk' | grep -v "/distro-info/"
  elif [ "${TARGET_PLATFORM}" == "linux" ]
  then
    find "${folder_path}" -name \* -type f ! -iname "*.cmake" ! -iname "*.txt" ! -iname "*.rst" ! -iname "*.html" ! -iname "*.json" ! -iname "*.py" ! -iname "*.pyc" ! -iname "*.h" ! -iname "*.xml" ! -iname "*.a" ! -iname "*.la" ! -iname "*.spec" ! -iname "*.specs" ! -iname "*.decTest" ! -iname "*.exe" | grep -v "/ldscripts/" | grep -v "/doc/" | grep -v "/locale/" | grep -v "/include/" | grep -v "/distro-info/"
  else
    echo "Oops! Unsupported TARGET_PLATFORM=${TARGET_PLATFORM}."
    exit 1
  fi
}

# Output the result of a filtered otool.
function get_darwin_lc_rpaths()
{
  local file_path="$1"

  otool -l "${file_path}" | grep LC_RPATH -A2 | grep '(offset ' | sed -e 's|.*path \(.*\) (offset.*)|\1|'
}

function get_linux_rpaths_line()
{
  local file_path="$1"

  readelf -d "${file_path}" \
    | egrep '(RUNPATH|RPATH)' \
    | sed -e 's|.*\[\(.*\)\]|\1|' 
  
}

# -----------------------------------------------------------------------------

function check_binary()
{
  local file_path="$1"

  if file --mime "${file_path}" | grep -q text
  then
    echo "${file_path} has no text"
    return 0
  fi

  check_binary_for_libraries "$1"
}

function check_binary_for_libraries()
{
  local file_path="$1"
  local file_name="$(basename ${file_path})"
  local folder_path="$(dirname ${file_path})"

  (
    xbb_activate

    if [ "${TARGET_PLATFORM}" == "win32" ]
    then
      echo
      echo "${file_name}: (${file_path})"
      set +e
      ${CROSS_COMPILE_PREFIX}-objdump -x "${file_path}" | grep -i 'DLL Name'

      local dll_names=$(${CROSS_COMPILE_PREFIX}-objdump -x "${file_path}" \
        | grep -i 'DLL Name' \
        | sed -e 's/.*DLL Name: \(.*\)/\1/' \
      )

      local n
      for n in ${dll_names}
      do
        if [ ! -f "${folder_path}/${n}" ] 
        then
          if is_win_sys_dll "${n}"
          then
            :
          elif [ "${n}${HAS_WINPTHREAD}" == "libwinpthread-1.dlly" ]
          then
            :
          else
            echo "Unexpected |${n}|"
            exit 1
          fi
        fi
      done
      set -e
    elif [ "${TARGET_PLATFORM}" == "darwin" ]
    then
      local lc_rpaths=$(get_darwin_lc_rpaths "${file_path}")

      echo
      (
        set +e
        cd ${folder_path}
        local lc_rpaths_line=$(echo ${lc_rpaths} | tr '\n' ';' | sed -e 's|;$||')
        if [ ! -z "${lc_rpaths_line}" ]
        then
          echo "${file_name}: (${file_path}, LC_RPATH=${lc_rpaths_line})"
        else
          echo "${file_name}: (${file_path})"
        fi

        otool -L "${file_name}" | sed -e '1d'
        set -e
      )

      # Skip the first line which is the binary itself.
      local libs
      if is_darwin_dylib "${file_path}"
      then
        # Skip the second line too, which is the library again.
        lib_paths=$(otool -L "${file_path}" \
              | sed '1d' \
              | sed '1d' \
              | sed -e 's|[[:space:]]*\(.*\) (.*)|\1|' \
            )
      else
        lib_paths=$(otool -L "${file_path}" \
              | sed '1d' \
              | sed -e 's|[[:space:]]*\(.*\) (.*)|\1|' \
            )
      fi

      # For debug, use DYLD_PRINT_LIBRARIES=1 
      # https://medium.com/@donblas/fun-with-rpath-otool-and-install-name-tool-e3e41ae86172

      for lib_path in ${lib_paths}
      do
        if [ "${lib_path:0:1}" == "/" ]
        then
          # If an absolute path, it must be in the system.
          if is_darwin_allowed_sys_dylib "${lib_path}"
          then
            :
          else
            echo ">>> absolute \"${lib_path}\" not one of the allowed libs"
            exit 1
          fi

        elif [ "${lib_path:0:1}" == "@" ]
        then

          local executable_prefix="@executable_path/"
          local loader_prefix="@loader_path/"
          local rpath_prefix="@rpath/"

          if [ "${lib_path:0:${#executable_prefix}}" == "${executable_prefix}" ]
          then
            echo ">>> \"${lib_path}\" is relative to unknown executable"
            exit 1
          elif [ "${lib_path:0:${#loader_prefix}}" == "${loader_prefix}" ]
          then
            if [ ! -f "${folder_path}/${lib_path:${#loader_prefix}}" ]
            then
              echo ">>> \"${lib_path:${#loader_prefix}}\" is expected in \"${folder_path}\""
              exit 1
            fi
          elif [ "${lib_path:0:${#rpath_prefix}}" == "${rpath_prefix}" ]
          then
            # ???
            local file_relative_path="${lib_path:${#rpath_prefix}}"
            local actual_folder_path
            for lc_rpath in ${lc_rpaths}
            do
              if [ "${lc_rpath:0:${#loader_prefix}}" == "${loader_prefix}" ]
              then
                actual_folder_path=${folder_path}/${lc_rpath:${#loader_prefix}}
                if [ ! -f "${actual_folder_path}/${lib_path:${#rpath_prefix}}" ]
                then
                  echo ">>> \"${lib_path:${#rpath_prefix}}\" is expected in \"${actual_folder_path}\""
                  exit 1
                fi
              else
                echo ">>> LC_RPATH=${lc_rpath} syntax not supported"
                exit 1
              fi
            done

          else
            echo ">>> special relative \"${lib_path}\" not supported"
            exit 1
          fi

        else
          echo ">>> \"${lib_path}\" with unsupported syntax"
          exit 1
        fi
      done

      (
        # More or less deprecated by the above, but kept for just in case.
        set +e
        local unxp
        if [[ "${file_name}" == *\.dylib ]]
        then
          unxp=$(otool -L "${file_path}" | sed '1d' | sed '1d' | grep -v "${file_name}" | egrep -e "(macports|homebrew|opt|install)/")
        else
          unxp=$(otool -L "${file_path}" | sed '1d' | grep -v "${file_name}" | egrep -e "(macports|homebrew|opt|install)/")
        fi

        # echo "|${unxp}|"
        if [ ! -z "$unxp" ]
        then
          echo "Unexpected |${unxp}|"
          exit 1
        fi
        set -e
      )
    elif [ "${TARGET_PLATFORM}" == "linux" ]
    then
      echo
      echo "${file_name}: (${file_path})"
      set +e
      readelf -d "${file_path}" | egrep '(SONAME)'
      readelf -d "${file_path}" | egrep '(RUNPATH|RPATH)'
      readelf -d "${file_path}" | egrep '(NEEDED)'

      local so_names=$(readelf -d "${file_path}" \
        | grep -i 'Shared library' \
        | sed -e 's/.*Shared library: \[\(.*\)\]/\1/' \
      )

      # local relative_path=$(readelf -d "${file_path}" | egrep '(RUNPATH|RPATH)' | sed -e 's/.*\[\$ORIGIN//' | sed -e 's/\].*//')
      # echo $relative_path
      local linux_rpaths_line=$(get_linux_rpaths_line "${file_path}")
      local origin_prefix="\$ORIGIN"

      for so_name in ${so_names}
      do
        if is_linux_allowed_sys_so "${so_name}"
        then
          continue
        elif [[ ${so_name} == libpython* ]] && [[ ${file_name} == *-gdb-py ]]
        then
          continue
        else
          local found=""
          for rpath in $(echo "${linux_rpaths_line}" | tr ":" "\n")
          do
            if  [ "${rpath:0:${#origin_prefix}}" == "${origin_prefix}" ]
            then
              # Looks like "", "/../lib"
              local folder_relative_path="${rpath:${#origin_prefix}}"

              if [ -f "${folder_path}${folder_relative_path}/${so_name}" ] 
              then
                found="y"
                break
              fi
            else
              echo ">>> DT_RPATH \"${rpath}\" not supported"
            fi
          done

          if [ "${found}" != "y" ]
          then
            echo ">>> Library \"${so_name}\" not found in DT_RPATH"
            exit 1
          fi
        fi
      done
      set -e
    else
      echo "Oops! Unsupported TARGET_PLATFORM=${TARGET_PLATFORM}."
      exit 1
    fi
  )
}

function is_win_sys_dll() 
{
  local dll_name="$(echo "$1" | tr "[:upper:]" "[:lower:]")"

  # DLLs that are expected to be present on any Windows.
  # Be sure all names are lower case!
  local sys_dlls=( \
    advapi32.dll \
    bcrypt.dll \
    cabinet.dll \
    cfgmgr32.dll \
    comctl32.dll
    crypt32.dll \
    dbghelp.dll \
    dnsapi.dll \
    gdi32.dll \
    imm32.dll \
    imm32.dll \
    iphlpapi.dll \
    iphlpapi.dll \
    kernel32.dll \
    msi.dll \
    msvcr71.dll \
    msvcr80.dll \
    msvcr90.dll \
    msvcrt.dll \
    ole32.dll \
    oleaut32.dll \
    psapi.dll \
    rpcrt4.dll \
    setupapi.dll \
    shell32.dll \
    shlwapi.dll \
    user32.dll \
    userenv.dll \
    vcruntime140.dll \
    version.dll \
    winmm.dll \
    winmm.dll \
    ws2_32.dll \
    \
    api-ms-win-crt-conio-l1-1-0.dll \
    api-ms-win-crt-convert-l1-1-0.dll \
    api-ms-win-crt-environment-l1-1-0.dll \
    api-ms-win-crt-filesystem-l1-1-0.dll \
    api-ms-win-crt-heap-l1-1-0.dll \
    api-ms-win-crt-locale-l1-1-0.dll \
    api-ms-win-crt-math-l1-1-0.dll \
    api-ms-win-crt-multibyte-l1-1-0.dll \
    api-ms-win-crt-private-l1-1-0.dll \
    api-ms-win-crt-process-l1-1-0.dll \
    api-ms-win-crt-runtime-l1-1-0.dll \
    api-ms-win-crt-string-l1-1-0.dll \
    api-ms-win-crt-stdio-l1-1-0.dll \
    api-ms-win-crt-time-l1-1-0.dll \
    api-ms-win-crt-utility-l1-1-0.dll \
  )

  # The Python DLL were a permanent source of trouble.
  # python27.dll \
  # The latest Python 2.7.18 has no DLL at all, so it cannot be skipped.
  # python37.dll \
  # The Python 3 seems better, allow to copy it in the archive,
  # to be sure it matches the version used during build.

  local dll
  for dll in "${sys_dlls[@]}"
  do
    if [ "${dll}" == "${dll_name}" ]
    then
      return 0 # True
    fi
  done
  return 1 # False
}

function is_linux_allowed_sys_so() 
{
  local lib_name="$1"

  # Do not add these two, they are present if the toolchain is installed, 
  # but this is not guaranteed, so better copy them from the xbb toolchain.
  # libstdc++.so.6 
  # libgcc_s.so.1 

  # Shared libraries that are expected to be present on any Linux.
  # Note the X11 libraries.
  local sys_lib_names=(\
    librt.so.1 \
    libm.so.6 \
    libc.so.6 \
    libnsl.so.1 \
    libutil.so.1 \
    libpthread.so.0 \
    libdl.so.2 \
    ld-linux.so.2 \
    ld-linux.so.3 \
    ld-linux-x86-64.so.2 \
    ld-linux-armhf.so.3 \
    ld-linux-arm64.so.1 \
    ld-linux-aarch64.so.1 \
    libX11.so.6 \
    libXau.so.6 \
    libxcb.so.1 \
  )

  local sys_lib_name
  for sys_lib_name in "${sys_lib_names[@]}"
  do
    if [ "${lib_name}" == "${sys_lib_name}" ]
    then
      return 0 # True
    fi
  done
  return 1 # False
}

# Links are automatically followed.
function is_darwin_sys_dylib() 
{
  local lib_name="$1"

  if [[ ${lib_name} == /usr/lib* ]]
  then
    return 0 # True
  fi
  if [[ ${lib_name} == /System/Library/Frameworks/* ]]
  then
    return 0 # True
  fi
  if [[ ${lib_name} == /Library/Frameworks/* ]]
  then
    return 0 # True
  fi

  return 1 # False
}

function is_darwin_allowed_sys_dylib() 
{
  local lib_name="$1"

  # Since there is no -static-libc++, the first attempt was to not 
  # define these here and have the 10.10 ones copied to the application. 
  # Building CMake proved that this is ok with 10.11 and 10.12, but 
  # failes on 10.13 and 10.14 with:
  # dyld: Symbol not found: __ZNSt3__118shared_timed_mutex13unlock_sharedEv
  # Referenced from: /System/Library/Frameworks/CoreDisplay.framework/Versions/A/CoreDisplay
  # Expected in: /Users/travis/test-cmake/xpack-cmake-3.17.1-1/bin/libc++.1.dylib
  # in /System/Library/Frameworks/CoreDisplay.framework/Versions/A/CoreDisplay
  #
  # /usr/lib/libc++.dylib \
  # /usr/lib/libc++.1.dylib \
  # /usr/lib/libc++abi.dylib \

  # Same for -static-libgcc; there were no cases which failed on later releases,
  # but for consistency, they are also included here.
  #
  # /usr/lib/libgcc_s.1.dylib \

# /usr/lib/libz.1.dylib \
# /usr/lib/libedit.3.dylib \

  local sys_libs=(\
    /usr/lib/libgcc_s.1.dylib \
    \
    /usr/lib/libc++.dylib \
    /usr/lib/libc++.1.dylib \
    /usr/lib/libc++abi.dylib \
    \
    /usr/lib/libSystem.B.dylib \
    /usr/lib/libobjc.A.dylib \
    \
    /usr/lib/libutil.dylib \
    /usr/lib/libcompression.dylib \
    \
  )

  if [[ ${lib_name} == /System/Library/Frameworks/* ]]
  then
    # Allow all system frameworks.
    return 0 # True
  fi

  local lib
  for lib in "${sys_libs[@]}"
  do
    if [ "${lib}" == "${lib_name}" ]
    then
      return 0 # True
    fi
  done
  return 1 # False
}

# -----------------------------------------------------------------------------

function has_origin()
{
  if [ $# -lt 1 ]
  then
    warning "has_origin: Missing file argument"
    exit 1
  fi

  local elf="$1"
  if [ "${TARGET_PLATFORM}" == "linux" ]
  then
    local origin=$(readelf -d ${elf} | egrep '(RUNPATH|RPATH)' | egrep '\$ORIGIN')
    if [ ! -z "${origin}" ]
    then
      return 0 # true
    fi
  fi
  return 1 # false
}

function has_rpath_origin()
{
  if [ $# -lt 1 ]
  then
    warning "has_rpath_origin: Missing file argument"
    exit 1
  fi

  local elf="$1"
  if [ "${TARGET_PLATFORM}" == "linux" ]
  then
    local origin=$(readelf -d ${elf} | grep 'Library rpath: \[' | grep '\$ORIGIN')
    if [ ! -z "${origin}" ]
    then
      return 0 # true
    fi
  fi
  return 1 # false
}

# DT_RPATH is searchd before LD_LIBRARY_PATH and DT_RUNPATH.
function has_rpath()
{
  if [ $# -lt 1 ]
  then
    warning "has_rpath: Missing file argument"
    exit 1
  fi

  local elf="$1"
  if [ "${TARGET_PLATFORM}" == "linux" ]
  then
    
    local rpath=$(readelf -d ${elf} | egrep '(RUNPATH|RPATH)')
    if [ ! -z "${rpath}" ]
    then
      return 0 # true
    fi
    
  fi
  return 1 # false
}


function strip_binaries()
{
  local folder_path="${APP_PREFIX}"
  if [ $# -ge 1 ]
  then
    folder_path="$1"
  fi

  if [ "${WITH_STRIP}" == "y" ]
  then
    (
      xbb_activate

      echo
      echo "# Stripping binaries..."

      # Otherwise `find` may fail.
      cd "${WORK_FOLDER_PATH}"

      local binaries
      if [ "${TARGET_PLATFORM}" == "win32" ]
      then

        binaries=$(find "${folder_path}" \( -name \*.exe -o -name \*.dll -o -name \*.pyd \))
        for bin in ${binaries} 
        do
          strip_binary "${bin}"
        done

      elif [ "${TARGET_PLATFORM}" == "darwin" ]
      then

        binaries=$(find "${folder_path}" -name \* -perm +111 -type f ! -type l | grep -v 'MacOSX.*\.sdk' | grep -v 'macOS.*\.sdk' )
        for bin in ${binaries} 
        do
          if is_elf "${bin}"
          then
            if is_target "${bin}"
            then
              strip_binary "${bin}"
            else
              if [ "${IS_DEVELOP}" == "y" ]
              then
                echo "$(file "${bin}") (not for target architecture)"
              fi
            fi
          fi
        done

      elif [ "${TARGET_PLATFORM}" == "linux" ]
      then

        binaries=$(find "${folder_path}" -name \* -type f ! -type l)
        for bin in ${binaries} 
        do
          if is_elf "${bin}"
          then
            if is_target "${bin}"
            then
              strip_binary "${bin}"
            else
              if [ "${IS_DEVELOP}" == "y" ]
              then
                echo "$(file "${bin}") (not for target architecture)"
              fi
            fi
          fi
        done

      fi
    )
  fi
}


# Strip binary files as in "strip binary" form, for both native
# (linux/mac) and mingw.
function strip_binary2() 
{
  (
    set +e
    if [ $# -lt 2 ]
    then
        warning "strip_binary: Missing arguments"
        exit 1
    fi

    local strip="$1"
    local bin="$2"

    if is_elf "${bin}"
    then
      if has_origin "${bin}"
      then
        if [ "${IS_DEVELOP}" == "y" ]
        then
          echo "${strip} ${bin} skipped (patched)"
        fi
      else
        echo "${strip} ${bin}"
        # ${strip} ${bin} 2>/dev/null || true
        "${strip}" -S "${bin}" || true
      fi
    else
      echo $(file "${bin}")
    fi

    set -e
  )
}

function strip_binary() 
{
  if [ $# -lt 1 ]
  then
    warning "strip_binary: Missing file argument"
    exit 1
  fi

  local file_path="$1"

  local strip
  set +u
  strip="${STRIP}"
  if [ "${TARGET_PLATFORM}" == "win32" ]
  then
    if [ -z "${strip}" ]
    then
      strip="${CROSS_COMPILE_PREFIX}-strip"
    fi
    if [[ "${file_path}" != *.exe ]] && [[ "${file_path}" != *.dll ]] && [[ "${file_path}" != *.pyd ]]
    then
      file_path="${file_path}.exe"
    fi
  else
    if [ -L "${file_path}" ]
    then
      echo "??? '${file_path}' should not strip links"
      exit 1
    fi
    if [ -z "${strip}" ]
    then
      strip="strip"
    fi
  fi
  set -u

  if is_elf "${file_path}" || is_pe "${file_path}"
  then
    :
  else
    echo $(file "${file_path}")
    return
  fi  

  if has_origin "${file_path}"
  then
    # If the file was patched, skip strip, otherwise
    # we may damage the binary due to a bug in strip.
    echo "${strip} ${file_path} skipped (patched)"
    return
  fi

  echo "[${strip} ${file_path}]"
  "${strip}" -S "${file_path}" || true
}

# -----------------------------------------------------------------------------

function strip_libs()
{
  if [ "${WITH_STRIP}" == "y" ]
  then
    (
      xbb_activate

      echo
      echo "Stripping libraries..."

      cd "${APP_PREFIX}"

      if [ "${TARGET_PLATFORM}" == "linux" -o "${TARGET_PLATFORM}" == "darwin" ]
      then
        run_verbose which strip

        local libs=$(find "${APP_PREFIX}" -type f \( -name \*.a -o -name \*.o -o -name \*.so -o -name \*.dylib \))
        for lib in ${libs}
        do
          # Do not use --strip-debug here, since the macOS strip does not support it.
          echo "strip -S ${lib}"
          strip -S "${lib}" || true
        done
      elif [ "${TARGET_PLATFORM}" == "win32" ]
      then
        run_verbose which ${CROSS_COMPILE_PREFIX}-strip

        local libs=$(find "${APP_PREFIX}" -type f \( -name \*.a -o -name \*.o -o -name \*.dll -o -name \*.pyd \))
        for lib in ${libs}
        do
          echo "${CROSS_COMPILE_PREFIX}-strip --strip-debug ${lib}"
          ${CROSS_COMPILE_PREFIX}-strip --strip-debug "${lib}" || true
        done
      fi
    ) 2>&1 | tee "${LOGS_FOLDER_PATH}/strip-libs-output-$(date -u +%Y%m%d-%H%M).txt"
  fi
}

# -----------------------------------------------------------------------------

# Windows binaries?!
function _is_elf()
{
  if [ $# -lt 1 ]
  then
    warning "is_elf: Missing arguments"
    exit 1
  fi

  local bin_path="$1"

  # Symlinks do not match.
  if [ -L "${bin_path}" ]
  then
    return 1
  fi

  if [ -f "${bin_path}" ]
  then
    # Return 0 (true) if found.
    if [ "${TARGET_PLATFORM}" == "linux" ]
    then
      file ${bin_path} | egrep -q "( ELF )"
    elif [ "${TARGET_PLATFORM}" == "darwin" ]
    then
      file ${bin_path} | egrep -q "Mach-O .*x86_64"
    elif [ "${TARGET_PLATFORM}" == "win32" ]
    then
      file ${bin_path} | egrep -q "( PE )|( PE32 )|( PE32\+ )"
    else
      return 1
    fi
  else
    return 1
  fi
}

function is_pe()
{
  if [ $# -lt 1 ]
  then
    warning "is_pe: Missing arguments"
    exit 1
  fi

  local bin_path="$1"

  # Symlinks do not match.
  if [ -L "${bin_path}" ]
  then
    return 1
  fi

  if [ -f "${bin_path}" ]
  then
    if [ "${TARGET_PLATFORM}" == "win32" ]
    then
      file ${bin_path} | egrep -q "( PE )|( PE32 )|( PE32\+ )"
    else
      return 1
    fi
  else
    return 1
  fi
}

function is_elf()
{
  if [ $# -lt 1 ]
  then
    warning "is_elf: Missing arguments"
    exit 1
  fi

  local bin_path="$1"

  # Symlinks do not match.
  if [ -L "${bin_path}" ]
  then
    return 1
  fi

  if [ -f "${bin_path}" ]
  then
    # Return 0 (true) if found.
    if [ "${TARGET_PLATFORM}" == "linux" ]
    then
      file ${bin_path} | egrep -q "( ELF )"
    elif [ "${TARGET_PLATFORM}" == "darwin" ]
    then
      file ${bin_path} | egrep -q "Mach-O .*x86_64"
    else
      return 1
    fi
  else
    return 1
  fi
}

function is_target()
{
  if [ $# -lt 1 ]
  then
    warning "is_target: Missing arguments"
    exit 1
  fi

  local bin_path="$1"

  # Symlinks do not match.
  if [ -L "${bin_path}" ]
  then
    return 1
  fi

  if [ -f "${bin_path}" ]
  then
    # Return 0 (true) if found.
    if [ "${TARGET_PLATFORM}" == "linux" -a "${TARGET_ARCH}" == "x64" ]
    then
      file ${bin_path} | egrep -q ", x86-64, "
    elif [ "${TARGET_PLATFORM}" == "linux" -a \( "${TARGET_ARCH}" == "x32" -o "${TARGET_ARCH}" == "ia32" \) ]
    then
      file ${bin_path} | egrep -q ", Intel 80386, "
    elif [ "${TARGET_PLATFORM}" == "linux" -a "${TARGET_ARCH}" == "arm64" ]
    then
      file ${bin_path} | egrep -q ", ARM aarch64, "
    elif [ "${TARGET_PLATFORM}" == "linux" -a "${TARGET_ARCH}" == "arm" ]
    then
      file ${bin_path} | egrep -q ", ARM, "
    elif [ "${TARGET_PLATFORM}" == "darwin" -a "${TARGET_ARCH}" == "x64" ]
    then
      file ${bin_path} | egrep -q " x86_64"
    elif [ "${TARGET_PLATFORM}" == "win32" -a "${TARGET_ARCH}" == "x64" ]
    then
      file ${bin_path} | egrep -q " x86-64 "
    elif [ "${TARGET_PLATFORM}" == "win32" -a \( "${TARGET_ARCH}" == "x32" -o "${TARGET_ARCH}" == "ia32" \) ]
    then
      file ${bin_path} | egrep -q " Intel 80386"
    else
      return 1
    fi
  else
    return 1
  fi
}

function is_elf_dynamic()
{
  if [ $# -lt 1 ]
  then
    warning "is_elf_dynamic: Missing arguments"
    exit 1
  fi

  local bin_path="$1"

  if is_elf "${bin_path}"
  then
    # Return 0 (true) if found.
    file ${bin_path} | egrep -q "dynamically"
  else
    return 1
  fi

}

function is_dynamic()
{
  if [ $# -lt 1 ]
  then
    warning "is_dynamic: Missing arguments"
    exit 1
  fi

  local bin_path="$1"

  if [ -f "${bin_path}" ]
  then
    # Return 0 (true) if found.
    file ${bin_path} | egrep -q "dynamically"
  else
    return 1
  fi
}

function is_darwin_dylib()
{
  if [ $# -lt 1 ]
  then
    warning "is_darwin_dylib: Missing arguments"
    exit 1
  fi

  local bin_path="$1"
  local real_path

  # Follow symlinks.
  if [ -L "${bin_path}" ]
  then
    real_path="$(realpath "${bin_path}")"
  else
    real_path="${bin_path}"
  fi

  if [ -f "${real_path}" ]
  then
    # Return 0 (true) if found.
    file ${real_path} | egrep -q "dynamically linked shared library"
  else
    return 1
  fi
}

function is_ar()
{
  if [ $# -lt 1 ]
  then
    warning "is_ar: Missing arguments"
    exit 1
  fi

  local bin_path="$1"

  # Symlinks do not match.
  if [ -L "${bin_path}" ]
  then
    return 1
  fi

  if [ -f "${bin_path}" ]
  then
    # Return 0 (true) if found.
    file ${bin_path} | egrep -q "ar archive"
  else
    return 1
  fi
}

# -----------------------------------------------------------------------------

# Deprecated, use copy_dependencies_recursive().
function copy_win_gcc_dll() 
{
  local dll_name="$1"

  # Identify the current cross gcc version, to locate the specific dll folder.
  local cross_gcc_version=$(${CROSS_COMPILE_PREFIX}-gcc --version | grep 'gcc' | sed -e 's/.*\s\([0-9]*\)[.]\([0-9]*\)[.]\([0-9]*\).*/\1.\2.\3/')
  local cross_gcc_version_short=$(echo ${cross_gcc_version} | sed -e 's/\([0-9]*\)[.]\([0-9]*\)[.]\([0-9]*\).*/\1.\2/')
  local SUBLOCATION="-win32"

  # First try Ubuntu specific locations,
  # then do a long full search.

  if [ -f "${XBB_FOLDER_PATH}/${CROSS_COMPILE_PREFIX}/lib/${dll_name}" ]
  then
    cp -v "${XBB_FOLDER_PATH}/${CROSS_COMPILE_PREFIX}/lib/${dll_name}" \
      "${APP_PREFIX}/bin"
  elif [ -f "/usr/lib/gcc/${CROSS_COMPILE_PREFIX}/${cross_gcc_version}/${dll_name}" ]
  then
    cp -v "/usr/lib/gcc/${CROSS_COMPILE_PREFIX}/${cross_gcc_version}/${dll_name}" \
      "${APP_PREFIX}/bin"
  elif [ -f "/usr/lib/gcc/${CROSS_COMPILE_PREFIX}/${cross_gcc_version_short}/${dll_name}" ]
  then
    cp -v "/usr/lib/gcc/${CROSS_COMPILE_PREFIX}/${cross_gcc_version_short}/${dll_name}" \
      "${APP_PREFIX}/bin"
  elif [ -f "/usr/lib/gcc/${CROSS_COMPILE_PREFIX}/${cross_gcc_version_short}${SUBLOCATION}/${dll_name}" ]
  then
    cp -v "/usr/lib/gcc/${CROSS_COMPILE_PREFIX}/${cross_gcc_version_short}${SUBLOCATION}/${dll_name}" \
      "${APP_PREFIX}/bin"
  else
    echo "Searching /usr for ${dll_name}..."
    SJLJ_PATH=$(find "${XBB_FOLDER_PATH}/${CROSS_COMPILE_PREFIX}" /usr \! -readable -prune -o -name ${dll_name} -print | grep ${CROSS_COMPILE_PREFIX})
    cp -v ${SJLJ_PATH} "${APP_PREFIX}/bin"
  fi
}

# Deprecated, use copy_dependencies_recursive().
function copy_win_libwinpthread_dll() 
{
  if [ -f "${XBB_FOLDER_PATH}/${CROSS_COMPILE_PREFIX}/bin/libwinpthread-1.dll" ]
  then
    cp "${XBB_FOLDER_PATH}/${CROSS_COMPILE_PREFIX}/bin/libwinpthread-1.dll" \
      "${APP_PREFIX}/bin"
  else
    echo "No libwinpthread-1.dll"
    exit 1
  fi
}

# -----------------------------------------------------------------------------

# https://wincent.com/wiki/%40executable_path%2C_%40load_path_and_%40rpath
# @loader_path = the path of the elf refering it (like $ORIGIN) (since 10.4)
# @rpath = one of the LC_RPATH array stored in the elf (since 10.5)
# @executable_path = the path of the application loading the shared library

function change_dylib()
{
  local dylib_name="$1"
  local file_path="$2"

  if [ -L "${file_path}" ]
  then
    echo "Oops! change_dylib should not change links! (${file_path})" 
    exit 1
  fi

  local dylib_path="$(otool -L "${file_path}" | sed '1d' | sed -e 's|[[:space:]]*\(.*\)[[:space:]][(].*[)]|\1|' | grep "${dylib_name}")"

  if [ -z "${dylib_path}" ]
  then
    echo "Oops! dylib ${dylib_name} not refered by ${file_path}..."
    # for example libftdi1.2.4.0.dylib has the name libftdi1.2.dylib.
    exit 1
  elif [ $(printf "${dylib_path}" | wc -l) -gt 1 ]
  then
    echo "Oops! ${file_path} has multiple ${dylib_name} libraries..."
    exit 1
  fi

  # Hack to bring Python library name in line with the other libraries.
  if [ "${dylib_name}" == "Python" ]
  then
    local version="$(otool -L "${file_path}" | grep "${dylib_name}" | sed -e 's|.*current version \([0-9][0-9]*\.[0-9][0-9]*\).*|\1|')"
    dylib_name="libpython${version}.dylib"
    rm -rfv "$(dirname ${file_path})/Python"
  fi

  # Compute the first rpath, if present.
  local lc_rpath_first=$(otool -l "${file_path}" | grep LC_RPATH -A2 | grep '(offset ' | sed -e 's|.*path \(.*\) (offset.*)|\1|' | sed -n -e'1p')

  if [ ! -z "${lc_rpath_first}" ]
  then
    if [[ ${lc_rpath_first} =~ @loader_path/* ]]
    then
      if [ "${dylib_path}" != "@rpath/${dylib_name}" ]
      then
        
        chmod +w "${file_path}"
        run_verbose install_name_tool \
          -change "${dylib_path}" \
          "@rpath/${dylib_name}" \
          "${file_path}"
      else
        echo "Cannot set \"${dylib_path}\" over \"@rpath/${dylib_name}\""
        exit 1
      fi
    else
      echo "LC_RPATH ${lc_rpath_first} not supported"
      exit 1
    fi
  else
    if [ "${dylib_path}" != "@loader_path/${dylib_name}" ]
    then
      local dest="$(dirname ${file_path})/${dylib_name}"
      if [ ! -f "${dest}" ]
      then
        echo "change dylib to missing ${dest}"
        exit 1
      fi

      chmod +w "${file_path}"
      run_verbose install_name_tool \
        -change "${dylib_path}" \
        "@loader_path/${dylib_name}" \
        "${file_path}"
    fi
  fi
}

# Remove non relative LC_RPATH entries.

# $1 = file path
function clean_rpaths()
{
  local file_path="$1"

  if [ "${TARGET_PLATFORM}" == "darwin" ]
  then

    local lc_rpaths=$(get_darwin_lc_rpaths "${file_path}")
    if [ -z "${lc_rpaths}" ]
    then
      return
    fi

    for rpath in ${lc_rpaths}
    do
      if [ "${rpath:0:1}" != "@" ]
      then
        run_verbose install_name_tool \
          -delete_rpath "${rpath}" \
          "${file_path}"
      fi
    done

  elif [ "${TARGET_PLATFORM}" == "linux" ]
  then

      local origin_prefix="\$ORIGIN"
      local new_rpath=""

      local linux_rpaths_line=$(get_linux_rpaths_line "${file_path}")

      if [ -z "${linux_rpaths_line}" ]
      then
        return
      fi

      for rpath in $(echo "${linux_rpaths_line}" | tr ":" "\n")
      do
        if [ "${rpath:0:${#origin_prefix}}" == "${origin_prefix}" ]
        then
          if [ ! -z "${new_rpath}" ]
          then
            new_rpath+=":"
          fi
          new_rpath+="${rpath}"
        fi
      done

      if [ -z "${new_rpath}" ]
      then
        new_rpath="${origin_prefix}"
      fi

      patch_linux_elf_set_rpath \
        "${file_path}" \
        "${new_rpath}"

  else
    echo "Oops! Unsupported TARGET_PLATFORM=${TARGET_PLATFORM} in clean_rpaths."
    exit 1
  fi
}

# Workaround to Docker error on 32-bit image:
# stat: Value too large for defined data type (requires -D_FILE_OFFSET_BITS=64)
function patch_linux_elf_origin()
{
  if [ $# -lt 1 ]
  then 
    echo "patch_linux_elf_origin requires 1 args." 
    exit 1
  fi

  local file_path="$1"
  local libexec_path
  if [ $# -ge 2 ]
  then
    libexec_path="$2"
  else
    libexec_path="$(dirname "${file_path}")"
  fi

  local do_require_rpath="${DO_REQUIRE_RPATH:-"y"}"

  local patchelf=${PATCHELF:-$(which patchelf)}
  # run_verbose "${patchelf}" --version
  # run_verbose "${patchelf}" --help

  local patchelf_has_output=""
  local use_copy_hack="${USE_COPY_HACK:-"n"}"
  if [ "${use_copy_hack}" == "y" ]
  then
    local tmp_path=$(mktemp)
    rm -rf "${tmp_path}"
    cp "${file_path}" "${tmp_path}"
    if "${patchelf}" --help 2>&1 | egrep -q -e '--output'
    then
      patchelf_has_output="y"
    fi
  else
    local tmp_path="${file_path}"
  fi

  if file "${tmp_path}" | grep statically
  then
    file "${file_path}"
  else
    if ! has_rpath "${file_path}"
    then
      echo "patch_linux_elf_origin: ${file_path} has no rpath!"
      if [ "${do_require_rpath}" == "y" ]
      then
        exit 1
      fi
    fi

    if [ "${patchelf_has_output}" == "y" ]
    then
      echo "[${patchelf} --force-rpath --set-rpath \"\$ORIGIN\" --output \"${file_path}\" \"${tmp_path}\"]" 
      ${patchelf} --force-rpath --set-rpath "\$ORIGIN" --output "${file_path}" "${tmp_path}" 
    else
      echo "[${patchelf} --force-rpath --set-rpath \"\$ORIGIN\" \"${file_path}\"]"
      ${patchelf} --force-rpath --set-rpath "\$ORIGIN" "${tmp_path}"
      if [ "${use_copy_hack}" == "y" ]
      then
        cp "${tmp_path}" "${file_path}"
      fi
    fi

    if [ "${IS_DEVELOP}" == "y" ]
    then
      readelf -d "${tmp_path}" | egrep '(RUNPATH|RPATH)'
      ldd "${tmp_path}"
    fi

  fi
  if [ "${use_copy_hack}" == "y" ]
  then
    rm -rf "${tmp_path}"
  fi
}

function patch_linux_elf_set_rpath()
{
  if [ $# -lt 2 ]
  then 
    echo "patch_linux_elf_set_rpath requires 2 args." 
    exit 1
  fi

  local file_path="$1"
  local new_rpath="$2"

  if [ "${new_rpath:(-2)}" == "/." ]
  then
    let remaining=${#new_rpath}-2
    new_rpath=${new_rpath:0:${remaining}}
  fi

  local do_require_rpath="${DO_REQUIRE_RPATH:-"y"}"

  if file "${file_path}" | grep statically
  then
    file "${file_path}"
  else
    local patchelf=${PATCHELF:-$(which patchelf)}
    # run_verbose "${patchelf}" --version
    # run_verbose "${patchelf}" --help

    local patchelf_has_output=""
    local use_copy_hack="${USE_COPY_HACK:-"n"}"
    if [ "${use_copy_hack}" == "y" ]
    then
      local tmp_path=$(mktemp)
      rm -rf "${tmp_path}"
      cp "${file_path}" "${tmp_path}"
      if "${patchelf}" --help 2>&1 | egrep -q -e '--output'
      then
        patchelf_has_output="y"
      fi
    else
      local tmp_path="${file_path}"
    fi

    if ! has_rpath "${file_path}"
    then
      echo "patch_linux_elf_set_rpath: ${file_path} has no rpath!"
      if [ "${do_require_rpath}" == "y" ]
      then
        exit 1
      fi
    fi

    if [ "${patchelf_has_output}" == "y" ]
    then
      echo "[${patchelf} --force-rpath --set-rpath \"${new_rpath}\" --output \"${file_path}\" \"${tmp_path}\"]" 
      ${patchelf} --force-rpath --set-rpath "${new_rpath}" --output "${file_path}" "${tmp_path}" 
    else
      echo "[${patchelf} --force-rpath --set-rpath \"${new_rpath}\" \"${file_path}\"]"
      ${patchelf} --force-rpath --set-rpath "${new_rpath}" "${tmp_path}"
      if [ "${use_copy_hack}" == "y" ]
      then
        cp "${tmp_path}" "${file_path}"
      fi
    fi

    if [ "${IS_DEVELOP}" == "y" ]
    then
      readelf -d "${tmp_path}" | egrep '(RUNPATH|RPATH)'
      ldd "${tmp_path}"
    fi

    if [ "${use_copy_hack}" == "y" ]
    then
      rm -rf "${tmp_path}"
    fi
  fi
}

function patch_linux_elf_add_rpath()
{
  if [ $# -lt 2 ]
  then 
    echo "patch_linux_elf_add_rpath requires 2 args." 
    exit 1
  fi

  local file_path="$1"
  local new_rpath="$2"

  if [ "${new_rpath:(-2)}" == "/." ]
  then
    let remaining=${#new_rpath}-2
    new_rpath=${new_rpath:0:${remaining}}
  fi

  local do_require_rpath="${DO_REQUIRE_RPATH:-"y"}"

  if file "${file_path}" | grep statically
  then
    file "${file_path}"
  else
    if [ -z "${new_rpath}" ]
    then
      echo "patch_linux_elf_add_rpath new path cannot be empty." 
      exit 1
    fi

    local linux_rpaths_line=$(get_linux_rpaths_line "${file_path}")

    if [ -z "${linux_rpaths_line}" ]
    then
      echo "patch_linux_elf_add_rpath: ${file_path} has no rpath!"
      if [ "${do_require_rpath}" == "y" ]
      then
        exit 1
      fi
    else
      for rpath in $(echo "${linux_rpaths_line}" | tr ":" "\n")
      do
        if [ "${rpath}" == "${new_rpath}" ]
        then
          # Already there.
          return
        fi
      done

      new_rpath="${linux_rpaths_line}:${new_rpath}"
    fi

    local patchelf=${PATCHELF:-$(which patchelf)}
    # run_verbose "${patchelf}" --version
    # run_verbose "${patchelf}" --help

    local patchelf_has_output=""
    local use_copy_hack="${USE_COPY_HACK:-"n"}"
    if [ "${use_copy_hack}" == "y" ]
    then
      local tmp_path=$(mktemp)
      rm -rf "${tmp_path}"
      cp "${file_path}" "${tmp_path}"
      if "${patchelf}" --help 2>&1 | egrep -q -e '--output'
      then
        patchelf_has_output="y"
      fi
    else
      local tmp_path="${file_path}"
    fi

    if [ "${patchelf_has_output}" == "y" ]
    then
      echo "[${patchelf} --force-rpath --set-rpath \"${new_rpath}\" --output \"${file_path}\" \"${tmp_path}\"]" 
      ${patchelf} --force-rpath --set-rpath "${new_rpath}" --output "${file_path}" "${tmp_path}" 
    else
      echo "[${patchelf} --force-rpath --set-rpath \"${new_rpath}\" \"${file_path}\"]"
      ${patchelf} --force-rpath --set-rpath "${new_rpath}" "${tmp_path}"
      if [ "${use_copy_hack}" == "y" ]
      then
        cp "${tmp_path}" "${file_path}"
      fi
    fi
    
    if [ "${IS_DEVELOP}" == "y" ]
    then
      readelf -d "${tmp_path}" | egrep '(RUNPATH|RPATH)'
      ldd "${tmp_path}"
    fi

    if [ "${use_copy_hack}" == "y" ]
    then
      rm -rf "${tmp_path}"
    fi
  fi
}

# Compute the $ORIGIN from the given folder path to libexec.
function compute_origin_relative_to_libexec()
{
  if [ $# -lt 1 ]
  then 
    echo "compute_origin_relative_to_libexec requires 1 arg." 
    exit 1
  fi

  local folder_path="$1"

  local relative_folder_path="$(realpath --relative-to="${folder_path}" "${APP_PREFIX}/libexec")"

  echo "\$ORIGIN/${relative_folder_path}"
}

# -----------------------------------------------------------------------------

# Prepare libraries for a single elf
# (otherwise use prepare_app_folder_libraries)
# Note: Does not use libexec.

# $1 - absolute path to executable
function prepare_app_libraries()
{
  local app_path="$1"
  shift

  (
    xbb_activate

    local app_folder_path="$(dirname "${app_path}")"

    if [ "${WITH_STRIP}" == "y" ]
    then
      strip_binary "${app_path}"
    fi

    if [ "${TARGET_PLATFORM}" == "linux" ]
    then
      echo
      echo "Shared libraries:"
      echo "${app_path}"
      set +e
      readelf -d "${app_path}" | grep 'Shared library:'
      set -e

      echo
      echo "Preparing libraries..."
      patch_linux_elf_origin "${app_path}"

      echo
      copy_dependencies_recursive "${app_path}" "${app_folder_path}"

      echo
      echo "Updated shared libraries:"
      echo ldd -v "${app_path}"
      ldd -v "${app_path}"
    elif [ "${TARGET_PLATFORM}" == "darwin" ]
    then
      echo
      echo "Initial dynamic libraries:"
      otool -L "${app_path}"

      echo
      echo "Preparing libraries..."
      copy_dependencies_recursive "${app_path}" "${app_folder_path}"

      echo
      echo "Updated dynamic libraries:"
      otool -L "${app_path}"
    elif [ "${TARGET_PLATFORM}" == "win32" ]
    then
      echo
      echo "Dynamic libraries:"
      echo "${app_path}.exe"
      ${CROSS_COMPILE_PREFIX}-objdump -x "${app_path}.exe" | grep -i 'DLL Name'

      echo
      echo "Preparing libraries..."
      copy_dependencies_recursive "${app_path}.exe" "${app_folder_path}"
    else
      echo "Oops! Unsupported TARGET_PLATFORM=${TARGET_PLATFORM}."
      exit 1
    fi
  )
}

# Process all elf files in a folder (executables and shared libraries).

# $1 = folder path (default ${APP_PREFIX})
function prepare_app_folder_libraries()
{
  local folder_path="${APP_PREFIX}"
  if [ $# -ge 1 ]
  then
    folder_path="$1"
  fi

  (
    xbb_activate

    echo
    echo "# Preparing ${folder_path} libraries..."

    # Otherwise `find` may fail.
    cd "${WORK_FOLDER_PATH}"

    local binaries
    if [ "${TARGET_PLATFORM}" == "win32" ]
    then

      binaries=$(find_binaries "${folder_path}")
      for bin in ${binaries} 
      do
        echo
        echo "## Preparing $(basename "${bin}") ${bin} libraries..."
        # On Windows the DLLs are copied in the same folder.
        copy_dependencies_recursive "${bin}" "$(dirname "${bin}")"
      done

    elif [ "${TARGET_PLATFORM}" == "darwin" ]
    then

      binaries=$(find_binaries "${folder_path}")
      for bin in ${binaries} 
      do
        if is_elf "${bin}"
        then
          echo
          echo "## Preparing $(basename "${bin}") ${bin} libraries..."
          copy_dependencies_recursive "${bin}" "$(dirname "${bin}")"
        fi
      done

    elif [ "${TARGET_PLATFORM}" == "linux" ]
    then

      binaries=$(find_binaries "${folder_path}")
      for bin_path in ${binaries} 
      do
        if is_elf_dynamic "${bin_path}"
        then
          echo
          echo "## Preparing $(basename "${bin_path}") (${bin_path}) libraries..."
          copy_dependencies_recursive "${bin_path}" \
            "$(dirname "${bin_path}")"
 
          # echo $(basename "${bin_path}") $(readelf -d "${bin_path}" | egrep '(RUNPATH|RPATH)')
        fi
      done

    else
      echo "Oops! Unsupported TARGET_PLATFORM=${TARGET_PLATFORM}."
      exit 1
    fi
  ) 2>&1 | tee "${LOGS_FOLDER_PATH}/prepare-app-folder-libraries-output-$(date -u +%Y%m%d-%H%M).txt"
}

# The initial call uses the binary path (app or library, no links)
# and its folder path,
# so there is nothing to copy, only to process the dependencies.
#
# Subsequent calls may copy dependencies from other folders
# (like the installed/libs, or the compiler folders).
#
# On macOS, the destination may also be changed by existing LC_RPATH.
#
# Another complication is that the sources may be links, which must
# be preserved, but also the destinations must be copied.
#
# If needed, set PATCHELF to a custom 0.12.

# $1 = source file path
# $2 = destination folder path
function copy_dependencies_recursive()
{
  if [ $# -lt 2 ]
  then 
    echo "copy_dependencies_recursive requires at least 2 arg." 
    exit 1
  fi

  (
    local source_file_path="$1"
    local destination_folder_path="$2"

    DO_COPY_XBB_LIBS=${DO_COPY_XBB_LIBS:-'n'}
    DO_COPY_GCC_LIBS=${DO_COPY_GCC_LIBS:-'n'}

    local source_file_name="$(basename "${source_file_path}")"
    local source_folder_path="$(dirname "${source_file_path}")"

    local destination_file_path="${destination_folder_path}/${source_file_name}"

    if [ "${IS_DEVELOP}" == "y" ]
    then
      echo "copy_dependencies_recursive $@"
    fi

    # The first step is to copy the file to the destination,
    # if not already there.

    # Assume a regular file. Later changed if link.
    local actual_source_file_path="${source_file_path}"
    local actual_destination_file_path="$(realpath ${destination_folder_path})/${source_file_name}"

    # echo "I. Processing ${source_file_path} itself..."

    if [ ! -f "${destination_file_path}" ]
    then

      if [ -L "${source_file_path}" ]
      then

        # Compute the final absolute path of the link, regardless
        # how many links there are on the way.
        echo "process link ${source_file_path}"

        actual_source_file_path="$(readlink -f "${source_file_path}")"
        actual_source_file_name="$(basename "${actual_source_file_path}")"

        actual_destination_file_path="${destination_folder_path}/${actual_source_file_name}"
        if [ -f "${actual_destination_file_path}" ]
        then
          actual_destination_file_path="$(realpath "${actual_destination_file_path}")"
        fi

        if [ ! -f "${actual_destination_file_path}" ]
        then
          if [ ! -d "$(dirname "${actual_destination_file_path}")" ]
          then
            run_verbose install -d -m 755 "$(dirname "${actual_destination_file_path}")"
          fi
          run_verbose install -c -m 755 "${actual_source_file_path}" "${actual_destination_file_path}"
        fi

        (
          cd "${destination_folder_path}"
          run_verbose ln -s "${actual_source_file_name}" "${source_file_name}" 
        )

      elif is_elf "${source_file_path}" || is_pe "${source_file_path}"
      then

        if [ "${IS_DEVELOP}" == "y" ]
        then
          # The file is definitelly an elf, not a link.
          echo "is_elf ${source_file_name}"
        fi

        if [ ! -f "${destination_file_path}" ]
        then
          if [ ! -d "$(dirname "${destination_file_path}")" ]
          then
            run_verbose install -d -m 755 "$(dirname "${destination_file_path}")"
          fi
          run_verbose install -c -m 755 "${source_file_path}" "${destination_file_path}"
        fi

      else
        file "${source_file_path}"
        echo "Oops! ${source_file_path} not a symlink and not an elf"
        exit 1
      fi

    else
      echo_develop "already there ${destination_file_path}"
    fi

    if [ "${WITH_STRIP}" == "y" -a ! -L "${actual_destination_file_path}" ]
    then
      strip_binary "${actual_destination_file_path}"
    fi

    local actual_destination_folder_path="$(dirname "${actual_destination_file_path}")"

    if [ "${TARGET_PLATFORM}" == "linux" ]
    then

      echo
      echo "${actual_destination_file_path}:"
      readelf -d "${actual_destination_file_path}" | egrep '(SONAME)' || true
      readelf -d "${actual_destination_file_path}" | egrep '(RUNPATH|RPATH)' || true
      readelf -d "${actual_destination_file_path}" | egrep '(NEEDED)' || true

      # patch_linux_elf_origin "${actual_destination_file_path}"
      
      # echo "II. Processing ${source_file_path} dependencies..."

      # The file must be an elf. Get its shared libraries.
      local lib_names=$(readelf -d "${actual_destination_file_path}" \
            | grep -i 'Shared library' \
            | sed -e 's/.*Shared library: \[\(.*\)\]/\1/')
      local lib_name

      local linux_rpaths_line=$(get_linux_rpaths_line "${actual_destination_file_path}")

      # On Linux the references are library names.
      for lib_name in ${lib_names}
      do
        echo_develop "processing ${lib_name} of ${actual_destination_file_path}"

        if is_linux_allowed_sys_so "${lib_name}"
        then
          echo_develop "${lib_name} is allowed sys so"
          continue # System library, no need to copy it.
        fi

        local origin_prefix="\$ORIGIN"
        local must_add_origin=""
        local was_processed=""

        if [ -z "${linux_rpaths_line}" ]
        then
          echo ">>> \"${actual_destination_file_path}\" has no rpath"
          linux_rpaths_line="${LIBS_INSTALL_FOLDER_PATH}/lib"
        fi

        for rpath in $(echo "${linux_rpaths_line}" | tr ":" "\n")
        do
          echo_develop "rpath ${rpath}"

          if [ "${rpath:0:1}" == "/" ]
          then
            # Absolute path.
            if [ -f "${rpath}/${lib_name}" ]
            then
              echo_develop "${lib_name} found in ${rpath}"
              # Library present in the absolute path
              copy_dependencies_recursive \
                "${rpath}/${lib_name}" \
                "${APP_PREFIX}/libexec"

              must_add_origin="$(compute_origin_relative_to_libexec "${actual_destination_folder_path}")"
              was_processed="y"
              break
            fi

          elif [ "${rpath:0:${#origin_prefix}}" == "${origin_prefix}" ]
          then
            # Looks like "", "/../lib"
            local file_relative_path="${rpath:${#origin_prefix}}"
            if [ -f "${actual_destination_folder_path}/${file_relative_path}/${lib_name}" ]
            then
              # Library present in the $ORIGIN path
              echo_develop "${lib_name} found in ${rpath}"
              was_processed="y"
              break
            fi
          else
            echo ">>> \"${rpath}\" with unsupported syntax"
            exit 1
          fi
        done

        if [ "${was_processed}" != "y" ]
        then
          # Perhas a compiler dependency.
          local full_path=$(${CC} -print-file-name=${lib_name})
          # -print-file-name outputs back the requested name if not found.

          if [ -f "$(dirname "${actual_source_file_path}")/${lib_name}"  ]
          then
            must_add_origin="\$ORIGIN"
          elif [ "${full_path}" != "${lib_name}" ]
          then
            echo_develop "${lib_name} found as compiler file \"${full_path}\""
            copy_dependencies_recursive \
              "${full_path}" \
              "${APP_PREFIX}/libexec"

            must_add_origin="$(compute_origin_relative_to_libexec "${actual_destination_folder_path}")"
          else
            echo ">>> \"${lib_name}\" of \"${actual_destination_file_path}\" not yet processed"
            exit 1
          fi
        fi

        if [ ! -z "${must_add_origin}" ]
        then
          patch_linux_elf_add_rpath \
            "${actual_destination_file_path}" \
            "${must_add_origin}"
        fi
      done

      clean_rpaths "${actual_destination_file_path}"

      # echo "iterate ${destination_folder_path}/${source_file_name} done"
    elif [ "${TARGET_PLATFORM}" == "darwin" ]
    then

      # echo "II. Processing ${source_file_path} dependencies..."

      local lc_rpaths=$(get_darwin_lc_rpaths "${actual_destination_file_path}")
      local lc_rpaths_line=$(echo ${lc_rpaths} | tr '\n' ';' | sed -e 's|;$||')

      echo
      if [ ! -z "${lc_rpaths_line}" ]
      then
        otool -L "${actual_destination_file_path}" | sed -e "1s|:|: (LC_RPATH=${lc_rpaths_line})|"
      else
        otool -L "${actual_destination_file_path}"
      fi

      local lib_paths
      if is_darwin_dylib "${actual_destination_file_path}"
      then
        # Skip the extra line with the library name.
        lib_paths=$(otool -L "${actual_destination_file_path}" \
              | sed '1d' \
              | sed '1d' \
              | sed -e 's|[[:space:]]*\(.*\) (.*)|\1|' \
            )
      else
        lib_paths=$(otool -L "${actual_destination_file_path}" \
              | sed '1d' \
              | sed -e 's|[[:space:]]*\(.*\) (.*)|\1|' \
            )
      fi

      local executable_prefix="@executable_path/"
      local loader_prefix="@loader_path/"
      local rpath_prefix="@rpath/"

      # On macOS the references to dynamic libraries use full paths.
      for lib_path in ${lib_paths}
      do
        # The path may be regular (absolute or relative), but may also be
        # relative to a special prefix (executable, loader, rpath).
        # The name usually is a link to more strictly versioned file.
        
        echo_develop "processing ${lib_path} of ${actual_destination_file_path}"

        if [ "${lib_path:0:1}" == "@" ]
        then
          # If special prefix, someone else took care to place the
          # dependencies in the correct location.
          echo_develop "${lib_path} was already processed"
          continue
        fi

        local from_path="${lib_path}"

        if [ "${lib_path:0:1}" == "/" ]
        then
          # Regular absolute path, possibly a link.
          if is_darwin_sys_dylib "${lib_path}"
          then
            if is_darwin_allowed_sys_dylib "${lib_path}"
            then
              # Allowed system library, no need to copy it.
              echo_develop "${lib_path} is allowed sys dylib"
              continue 
            else
              echo ">>> absolute \"${lib_path}\" not one of the allowed libs"
              exit 1
            fi
          fi
        else
          ## Relative path.
          echo_develop "${lib_path} is a relative path"
          if [ -f "${LIBS_INSTALL_FOLDER_PATH}/lib/${lib_path}" ]
          then
            # Make the from path absolute.
            from_path="${LIBS_INSTALL_FOLDER_PATH}/lib/${lib_path}"
          else
            echo ">>> Relative path ${lib_path} not found in libs/lib"
            exit 1
          fi
        fi

        # Copy to libexec and use @loader_path.

        # Not optimised, since the library might already be in one
        # of the LC_RPATH folders.

        # TODO: add libexec to LC_RPATH and copy the libraries there.

        copy_dependencies_recursive \
          "${from_path}" \
          "${APP_PREFIX}/libexec" 

        local lib_name="$(basename "${from_path}")"
        local relative_folder_path="$(realpath --relative-to="${actual_destination_folder_path}" "${APP_PREFIX}/libexec")"

        # chmod +w "${file_path}"
        run_verbose install_name_tool \
          -change "${lib_path}" \
          "@loader_path/${relative_folder_path}/${lib_name}" \
          "${actual_destination_file_path}"

      done

      clean_rpaths "${actual_destination_file_path}"

    elif [ "${TARGET_PLATFORM}" == "win32" ]
    then

      echo
      echo "${actual_destination_file_path}:"
      ${CROSS_COMPILE_PREFIX}-objdump -x "${source_file_path}" \
            | grep -i 'DLL Name' || true

      local source_file_name="$(basename "${source_file_path}")"
      local source_folder_path="$(dirname "${source_file_path}")"

      # The first step is to copy the file to the destination.

      local actual_source_file_path=""
      local copied_file_path="${destination_folder_path}/${source_file_name}"

      # echo "I. Processing ${source_file_path} itself..."

      if [ ! -f "${destination_folder_path}/${source_file_name}" ]
      then

        # On Windows don't bother with sym links, simply copy the file
        # to the destination.

        actual_source_file_path="$(readlink -f "${source_file_path}")"
        copied_file_path="${destination_folder_path}/${source_file_name}"

      else
        if [ "${IS_DEVELOP}" == "y" ]
        then
          echo "${destination_folder_path}/${source_file_name} already there"
        fi
      fi

      if [ ! -z "${actual_source_file_path}" ]
      then
        if [ ! -f "${copied_file_path}" ]
        then
          if [ ! -d "$(dirname "${copied_file_path}")" ]
          then
            run_verbose install -d -m 755 "$(dirname "${copied_file_path}")"
          fi
          run_verbose install -c -m 755 "${actual_source_file_path}" "${copied_file_path}"
        fi
      else
        actual_source_file_path="${source_file_path}"
      fi

      if [ "${WITH_STRIP}" == "y" -a ! -L "${copied_file_path}" ]
      then
        strip_binary "${copied_file_path}"
      fi

      # If libexec is the destination, there is no need to link.
      if [ ! -f "${destination_folder_path}/${source_file_name}" ]
      then
        (
          cd "${destination_folder_path}"

          local link_relative_path="$(realpath --relative-to="${destination_folder_path}" "${copied_file_path}")"
          run_verbose ln -s "${link_relative_path}" "${source_file_name}" 
        )
      fi

      local actual_destination_file_path="$(realpath "${destination_folder_path}/${source_file_name}")"
      local actual_destination_folder_path="$(dirname "${actual_destination_file_path}")"

      # echo "II. Processing ${source_file_path} dependencies..."

      local libs=$(${CROSS_COMPILE_PREFIX}-objdump -x "${destination_folder_path}/${source_file_name}" \
            | grep -i 'DLL Name' \
            | sed -e 's/.*DLL Name: \(.*\)/\1/' \
          )
      local lib_name
      for lib_name in ${libs}
      do
        if [ -f "${destination_folder_path}/${lib_name}" ]
        then
          : # Already present in the same folder as the source.
        elif is_win_sys_dll "${lib_name}"
        then
          : # System DLL, no need to copy it.
        else
          local full_path=$(${CROSS_COMPILE_PREFIX}-gcc -print-file-name=${lib_name})

          if [ -f "${APP_PREFIX}/bin/${lib_name}" ]
          then
            # GCC leaves some .DLLs in bin.
            copy_dependencies_recursive \
              "${APP_PREFIX}/bin/${lib_name}" \
              "${destination_folder_path}"
          elif [ -f "${APP_PREFIX}/${CROSS_COMPILE_PREFIX}/bin/${lib_name}" ]
          then
            # ... or in x86_64-w64-mingw32/bin
            copy_dependencies_recursive \
              "${APP_PREFIX}/${CROSS_COMPILE_PREFIX}/bin/${lib_name}" \
              "${destination_folder_path}"
          elif [ -f "${LIBS_INSTALL_FOLDER_PATH}/bin/${lib_name}" ]
          then
            # These scripts leave libraries in install/libs/bin.
            copy_dependencies_recursive \
              "${LIBS_INSTALL_FOLDER_PATH}/bin/${lib_name}" \
              "${destination_folder_path}"
          elif [ "${DO_COPY_XBB_LIBS}" == "y" -a -f "${XBB_FOLDER_PATH}/${CROSS_COMPILE_PREFIX}/bin/${lib_name}" ]
          then
            copy_dependencies_recursive \
              "${XBB_FOLDER_PATH}/${CROSS_COMPILE_PREFIX}/bin/${lib_name}" \
              "${destination_folder_path}"
          elif [ "${DO_COPY_GCC_LIBS}" == "y" -a "${full_path}" != "${lib_name}" ]
          then
            # -print-file-name outputs back the requested name if not found.
            copy_dependencies_recursive \
              "${full_path}" \
              "${destination_folder_path}"
          elif [ "${DO_COPY_GCC_LIBS}" == "y" -a "${lib_name}" == "libwinpthread-1.dll" -a -f "${XBB_FOLDER_PATH}/usr/${CROSS_COMPILE_PREFIX}/bin/libwinpthread-1.dll" ]
          then
            copy_dependencies_recursive \
              "${XBB_FOLDER_PATH}/usr/${CROSS_COMPILE_PREFIX}/bin/libwinpthread-1.dll" \
              "${destination_folder_path}"
          else
            echo "${lib_name} required by ${source_file_name}, not found"
            exit 1
          fi
        fi
      done
    else
      echo "Oops! Unsupported TARGET_PLATFORM=${TARGET_PLATFORM}."
      exit 1
    fi

    if [ "${IS_DEVELOP}" == "y" ]
    then
      echo "done with ${source_file_path}"
    fi
  )
}

# Check all executables and shared libraries in the given folder.

# $1 = folder path (default ${APP_PREFIX})
function check_binaries()
{
  local folder_path="${APP_PREFIX}"
  if [ $# -ge 1 ]
  then
    folder_path="$1"
  fi

  (
    xbb_activate
    
    echo
    echo "Checking binaries for unwanted libraries..."

    # Otherwise `find` may fail.
    cd "${WORK_FOLDER_PATH}"

    local binaries
    if [ "${TARGET_PLATFORM}" == "win32" ]
    then

      binaries=$(find_binaries "${folder_path}")
      for bin in ${binaries} 
      do
        check_binary "${bin}"
      done

    elif [ "${TARGET_PLATFORM}" == "darwin" ]
    then

      binaries=$(find_binaries "${folder_path}")
      for bin in ${binaries} 
      do
        if is_elf "${bin}"
        then
          check_binary "${bin}"
        else
          if [ "${IS_DEVELOP}" == "y" ]
          then
            echo "$(file "${bin}") (not elf)"
          fi
        fi
      done

    elif [ "${TARGET_PLATFORM}" == "linux" ]
    then

      binaries=$(find_binaries "${folder_path}")
      for bin in ${binaries} 
      do
        if is_elf_dynamic "${bin}"
        then
          check_binary "${bin}"
        else
          if [ "${IS_DEVELOP}" == "y" ]
          then
            echo "$(file "${bin}") (not dynamic elf)"
          fi
        fi
      done

    else
      echo "Oops! Unsupported TARGET_PLATFORM=${TARGET_PLATFORM}."
      exit 1
    fi
  ) 2>&1 | tee "${LOGS_FOLDER_PATH}/check-binaries-output-$(date -u +%Y%m%d-%H%M).txt"
}

# -----------------------------------------------------------------------------

# $1 - absolute path to input folder
# $2 - name of output folder below INSTALL_FOLDER
function copy_license() 
{
  # Iterate all files in a folder and install some of them in the
  # destination folder
  (
    if [ -z "$2" ]
    then
      return
    fi

    echo
    echo "Copying license files for $2..."

    cd "$1"
    local f
    for f in *
    do
      if [ -f "$f" ]
      then
        if [[ "$f" =~ AUTHORS.*|NEWS.*|COPYING.*|README.*|LICENSE.*|Copyright.*|COPYRIGHT.*|FAQ.*|DEPENDENCIES.*|THANKS.*|CHANGES.* ]]
        then
          install -d -m 0755 \
            "${APP_PREFIX}/${DISTRO_INFO_NAME}/licenses/$2"
          install -v -c -m 644 "$f" \
            "${APP_PREFIX}/${DISTRO_INFO_NAME}/licenses/$2"
        fi
      elif [ -d "$f" ] && [[ "$f" =~ [Ll][Ii][Cc][Ee][Nn][Ss][Ee]* ]]
      then
        (
          cd "$f"
          local files=$(find . -type f)
          for file in ${files}
          do
            install -d -m 0755 \
              "${APP_PREFIX}/${DISTRO_INFO_NAME}/licenses/$2/$(dirname ${file})"
            install -v -c -m 644 "$file" \
              "${APP_PREFIX}/${DISTRO_INFO_NAME}/licenses/$2/$(dirname ${file})"
          done
        )
      fi
    done
  )
  (
    xbb_activate

    if [ "${TARGET_PLATFORM}" == "win32" ]
    then
      find "${APP_PREFIX}/${DISTRO_INFO_NAME}/licenses" \
        -type f \
        -exec unix2dos '{}' ';'
    fi
  )
}

function copy_build_files()
{
  echo
  echo "Copying build files..."

  (
    cd "${BUILD_GIT_PATH}"

    mkdir -pv patches

    # Ignore hidden folders/files (like .DS_Store).
    find scripts patches -type d ! -iname '.*' \
      -exec install -d -m 0755 \
        "${APP_PREFIX}/${DISTRO_INFO_NAME}"/'{}' ';'

    find scripts patches -type f ! -iname '.*' \
      -exec install -v -c -m 644 \
        '{}' "${APP_PREFIX}/${DISTRO_INFO_NAME}"/'{}' ';'

    if [ -f CHANGELOG.txt ]
    then
      install -v -c -m 644 \
          CHANGELOG.txt "${APP_PREFIX}/${DISTRO_INFO_NAME}"
    fi
    if [ -f CHANGELOG.md ]
    then
      install -v -c -m 644 \
          CHANGELOG.md "${APP_PREFIX}/${DISTRO_INFO_NAME}"
    fi
  )
}

# Must be called in the build folder, like 
# cd "${LIBS_BUILD_FOLDER_PATH}"
# cd "${BUILD_FOLDER_PATH}"

function copy_cmake_logs()
{
  local folder_name="$1"

  echo
  echo "Preserving CMake log files..."
  rm -rf "${LOGS_FOLDER_PATH}/${folder_name}"
  mkdir -pv "${LOGS_FOLDER_PATH}/${folder_name}/CMakeFiles"

  (
    cd "${folder_name}"
    cp -v "CMakeCache.txt" "${LOGS_FOLDER_PATH}/${folder_name}"

    cp -v "CMakeFiles"/*.log "${LOGS_FOLDER_PATH}/${folder_name}/CMakeFiles"
  )
}

# -----------------------------------------------------------------------------

# Copy one folder to another
function copy_dir() 
{
  local from_path="$1"
  local to_path="$2"

  set +u
  # rm -rf "${to_path}"
  mkdir -pv "${to_path}"

  (
    cd "${from_path}"
    if [ "${TARGET_PLATFORM}" == "darwin" ]
    then
      find . -xdev -print0 | cpio -oa0 | (cd "${to_path}" && cpio -im)
    else
      find . -xdev -print0 | cpio -oa0V | (cd "${to_path}" && cpio -imuV)
    fi
  )

  set -u
}

# -----------------------------------------------------------------------------

function create_archive()
{
  (
    xbb_activate

    local distribution_file_version="${RELEASE_VERSION}"

    local target_folder_name=${TARGET_FOLDER_NAME}

    if [ "${HAS_NAME_ARCH}" != "y" ]
    then
      # Pre xPack distributions use the old file name convention.
      distribution_file_version="${RELEASE_VERSION}-${DISTRIBUTION_FILE_DATE}"

      if [ "${TARGET_PLATFORM}" == "win32" ]
      then
        target_folder_name="win${TARGET_BITS}"
      elif [ "${TARGET_PLATFORM}" == "darwin" ]
      then
        target_folder_name="macos"
      elif [ "${TARGET_PLATFORM}" == "linux" ]
      then
        target_folder_name="${CONTAINER_DISTRO_LC_NAME}${TARGET_BITS}"
      else
        echo "Oops! Unsupported TARGET_PLATFORM=${TARGET_PLATFORM}."
        exit 1
      fi
    fi

    local distribution_file="${DEPLOY_FOLDER_PATH}/${DISTRO_LC_NAME}-${APP_LC_NAME}-${distribution_file_version}-${target_folder_name}"

    local archive_version_path
    archive_version_path="${INSTALL_FOLDER_PATH}/archive/${DISTRO_LC_NAME}-${APP_LC_NAME}-${distribution_file_version}"

    cd "${APP_PREFIX}"
    find . -name '.DS_Store' -exec rm '{}' ';'

    echo
    echo "Creating distribution..."

    mkdir -pv "${DEPLOY_FOLDER_PATH}"

    # The folder is temprarily moved into a versioned folder like
    # xpack-<app-name>-<version>, or, in previous versions, 
    # in a more elaborate hierarchy like
    # xPacks/<app-name>/<version>.
    # After the archive is created, the folders are moved back.
    # The atempt to transform the tar path fails, since symlinks were
    # also transformed, which is bad.
    if [ "${TARGET_PLATFORM}" == "win32" ]
    then

      local distribution_file="${distribution_file}.zip"

      echo
      echo "ZIP file: \"${distribution_file}\"."

      rm -rf "${INSTALL_FOLDER_PATH}/archive"
      mkdir -pv "${archive_version_path}"
      mv "${APP_PREFIX}"/* "${archive_version_path}"

      cd "${INSTALL_FOLDER_PATH}/archive"
      zip -r9 -q "${distribution_file}" *

      # Put everything back.
      mv "${archive_version_path}"/* "${APP_PREFIX}"

    else

      # Unfortunately on node.js, xz & bz2 require native modules, which
      # proved unsafe, some xz versions failed to compile on node.js v9.x,
      # so use the good old .tar.gz.
      local distribution_file
      if [ "${USE_TAR_GZ}" == "y" ]
      then
        # Some platforms (like Arduino) accept only this explicit path.
        distribution_file="${distribution_file}.tar.gz"
      else
        distribution_file="${distribution_file}.tgz"
      fi

      echo "Compressed tarball: \"${distribution_file}\"."

      rm -rf "${INSTALL_FOLDER_PATH}/archive"
      mkdir -pv "${archive_version_path}"
      mv -v "${APP_PREFIX}"/* "${archive_version_path}"

      # Without --hard-dereference the hard links may be turned into
      # broken soft links on macOS.
      cd "${INSTALL_FOLDER_PATH}"/archive
      # -J uses xz for compression; best compression ratio.
      # -j uses bz2 for compression; good compression ratio.
      # -z uses gzip for compression; fair compression ratio.
      tar -c -z -f "${distribution_file}" \
        --owner=0 \
        --group=0 \
        --format=posix \
        --hard-dereference \
        *

      # Put folders back.
      mv -v "${archive_version_path}"/* "${APP_PREFIX}"

    fi

    cd "${DEPLOY_FOLDER_PATH}"
    compute_sha shasum -a 256 "$(basename ${distribution_file})"
  )
}

# -----------------------------------------------------------------------------

# Deprecated, use check_binaries.
# $1 = application name
function _check_application()
{
  local app_name=$1

  if [ "${TARGET_PLATFORM}" == "linux" ]
  then

    echo
    echo "Checking binaries for unwanted shared libraries..."

    check_binary "${APP_PREFIX}/bin/${app_name}"

    local libs=$(find "${APP_PREFIX}/bin" -name \*.so.\* -type f)
    local lib
    for lib in ${libs} 
    do
      check_binary_for_libraries "${lib}"
    done

  elif [ "${TARGET_PLATFORM}" == "darwin" ]
  then

    echo
    echo "Checking binaries for unwanted dynamic libraries..."

    check_binary "${APP_PREFIX}/bin/${app_name}"

    local libs=$(find "${APP_PREFIX}/bin" -name \*.dylib -type f)
    local lib
    for lib in ${libs} 
    do
      check_binary_for_libraries "${lib}"
    done

  elif [ "${TARGET_PLATFORM}" == "win32" ]
  then

    echo
    echo "Checking binaries for unwanted DLLs..."

    check_binary "${APP_PREFIX}/bin/${app_name}.exe"

    local libs=$(find "${APP_PREFIX}/bin" -name \*.dll -type f)
    local lib
    for lib in ${libs} 
    do
      check_binary_for_libraries "${lib}"
    done

  else
    echo "Oops! Unsupported TARGET_PLATFORM=${TARGET_PLATFORM}."
    exit 1
  fi

  echo
}

# -----------------------------------------------------------------------------

function compute_sha() 
{
  # $1 shasum program
  # $2.. options
  # ${!#} file

  file=${!#}
  sha_file="${file}.sha"
  "$@" >"${sha_file}"
  echo "SHA: $(cat ${sha_file})"
}

# -----------------------------------------------------------------------------

function prime_wine()
{
  if [  "${TARGET_PLATFORM}" == "win32" ]
  then
    (
      xbb_activate

      echo
      winecfg &>/dev/null
      echo "wine primed, testing..."
    )
  fi
}

# -----------------------------------------------------------------------------

function copy_distro_files()
{
  (
    xbb_activate

    echo
    mkdir -pv "${APP_PREFIX}/${DISTRO_INFO_NAME}"

    copy_build_files

    echo
    echo "Copying xPack files..."

    cd "${BUILD_GIT_PATH}"
    README_OUT_FILE_NAME="${README_OUT_FILE_NAME:-README-OUT.md}"
    install -v -c -m 644 "scripts/${README_OUT_FILE_NAME}" \
      "${APP_PREFIX}/README.md"
  )
}

# -----------------------------------------------------------------------------

# Possibly set these before calling.
# FIX_LTO_PLUGIN
# LTO_PLUGIN_ORIGINAL_NAME
# LTO_PLUGIN_BFD_PATH

function fix_lto_plugin()
{
  # Redefine to "" to disable it.
  FIX_LTO_PLUGIN=${FIX_LTO_PLUGIN:-"y"}

  if [ "${TARGET_PLATFORM}" == "darwin" ]
  then
    LTO_PLUGIN_ORIGINAL_NAME=${LTO_PLUGIN_ORIGINAL_NAME:-"liblto_plugin.0.so"}
    LTO_PLUGIN_BFD_PATH=${LTO_PLUGIN_BFD_PATH:-"lib/bfd-plugins/liblto_plugin.so"}
  elif [ "${TARGET_PLATFORM}" == "linux" ]
  then
    if [ ${GCC_VERSION_MAJOR} -ge 11 ]
    then
      LTO_PLUGIN_ORIGINAL_NAME=${LTO_PLUGIN_ORIGINAL_NAME:-"liblto_plugin.so"}
    else # Up to GCC 10.
      LTO_PLUGIN_ORIGINAL_NAME=${LTO_PLUGIN_ORIGINAL_NAME:-"liblto_plugin.so.0.0.0"}
    fi
    LTO_PLUGIN_BFD_PATH=${LTO_PLUGIN_BFD_PATH:-"lib/bfd-plugins/liblto_plugin.so"}
  elif [ "${TARGET_PLATFORM}" == "win32" ]
  then
    if [ ${GCC_VERSION_MAJOR} -ge 11 ]
    then
      LTO_PLUGIN_ORIGINAL_NAME=${LTO_PLUGIN_ORIGINAL_NAME:-"liblto_plugin.dll"}
      LTO_PLUGIN_BFD_PATH=${LTO_PLUGIN_BFD_PATH:-"lib/bfd-plugins/liblto_plugin.dll"}
    else
      LTO_PLUGIN_ORIGINAL_NAME=${LTO_PLUGIN_ORIGINAL_NAME:-"liblto_plugin-0.dll"}
      LTO_PLUGIN_BFD_PATH=${LTO_PLUGIN_BFD_PATH:-"lib/bfd-plugins/liblto_plugin-0.dll"}
    fi
    LTO_PLUGIN_BFD_PATH=${LTO_PLUGIN_BFD_PATH:-"lib/bfd-plugins/liblto_plugin-0.dll"}
  fi

  # Create the missing LTO plugin links.
  # For `ar` to work with LTO objects, it needs the plugin in lib/bfd-plugins,
  # but the build leaves it where `ld` needs it. On POSIX, make a soft link.
  (
    cd "${APP_PREFIX}"

    echo
    if [ "${TARGET_PLATFORM}" == "win32" ]
    then
      echo
      echo "Copying ${LTO_PLUGIN_ORIGINAL_NAME}..."

      mkdir -pv "$(dirname ${LTO_PLUGIN_BFD_PATH})"

      if [ ! -f "${LTO_PLUGIN_BFD_PATH}" ]
      then
        local plugin_path="$(find * -type f -name ${LTO_PLUGIN_ORIGINAL_NAME})"
        if [ ! -z "${plugin_path}" ]
        then
          cp -v "${plugin_path}" "${LTO_PLUGIN_BFD_PATH}"
        else
          echo "${LTO_PLUGIN_ORIGINAL_NAME} not found."
          exit 1
        fi
      fi
    else
      echo
      echo "Creating ${LTO_PLUGIN_ORIGINAL_NAME} link..."

      mkdir -pv "$(dirname ${LTO_PLUGIN_BFD_PATH})"
      if [ ! -f "${LTO_PLUGIN_BFD_PATH}" ]
      then
        local plugin_path="$(find * -type f -name ${LTO_PLUGIN_ORIGINAL_NAME})"
        if [ ! -z "${plugin_path}" ]
        then
          ln -s -v "../../${plugin_path}" "${LTO_PLUGIN_BFD_PATH}"
        else
          echo "${LTO_PLUGIN_ORIGINAL_NAME} not found."
          exit 1
        fi
      fi
    fi
  )
}

# -----------------------------------------------------------------------------

# Return the value via MACOS_SDK_PATH.
function get_macos_sdk_path()
{
  local macos_sdk_path
  local print_path="$(xcode-select -print-path)"
  if [ -d "${print_path}/SDKs/MacOSX.sdk" ]
  then
    # Without Xcode, use the SDK that comes with the CLT.
    macos_sdk_path="${print_path}/SDKs/MacOSX.sdk"
  elif [ -d "${print_path}/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk" ]
  then
    # With Xcode, chose the SDK from the macOS platform.
    macos_sdk_path="${print_path}/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk"
  elif [ -d "${print_path}/Platforms/MacOSX.platform/Developer/SDKs/MacOSX${MACOSX_DEPLOYMENT_TARGET}.sdk" ]
  then
    # With Xcode, chose the SDK from the macOS platform.
    macos_sdk_path="${print_path}/Platforms/MacOSX.platform/Developer/SDKs/MacOSX${MACOSX_DEPLOYMENT_TARGET}.sdk"
  else
    echo "Cannot find SDK in ${print_path}."
    exit 1
  fi

  echo "${macos_sdk_path}"
}

function copy_macos_sdk()
{
  local sdk_path="$1"
  local sdk_dest_path="$2"

  # Copy the SDK in the distribution, to have a standalone package.
  run_verbose rm -rf "${sdk_dest_path}/"
  run_verbose cp -pRH "${sdk_path}" "${sdk_dest_path}"
  # Remove the manuals and save about 225 MB.
  run_verbose rm -rf "${sdk_dest_path}/usr/share/man/"
}

# -----------------------------------------------------------------------------

# Use a file, to allow sub-shells to add functions.

function tests_initialize()
{
  export TEST_FUNCTION_NAMES_FILE_PATH="${INSTALL_FOLDER_PATH}/test-function-names"
  rm -rf "${TEST_FUNCTION_NAMES_FILE_PATH}"
  touch "${TEST_FUNCTION_NAMES_FILE_PATH}"
}

function tests_add()
{
  echo "$1" >> "${TEST_FUNCTION_NAMES_FILE_PATH}"
}

function tests_run()
{
  (
    echo
    echo "Runnng final tests..."

    for test_function in $(cat ${TEST_FUNCTION_NAMES_FILE_PATH})
    do
      if [ "${test_function}" != "" ]
      then
        echo
        local func=$(echo ${test_function} | sed -e 's|-|_|g')
        echo "Running ${func}..."
        ${func}
      fi
    done
  ) 2>&1 | tee "${LOGS_FOLDER_PATH}/tests-output-$(date -u +%Y%m%d-%H%M).txt"
}

# -----------------------------------------------------------------------------
