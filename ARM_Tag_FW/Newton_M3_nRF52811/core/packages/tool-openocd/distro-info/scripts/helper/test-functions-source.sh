# -----------------------------------------------------------------------------
# This file is part of the xPack distribution.
#   (https://xpack.github.io)
# Copyright (c) 2020 Liviu Ionescu.
#
# Permission to use, copy, modify, and/or distribute this software 
# for any purpose is hereby granted, under the terms of the MIT license.
# -----------------------------------------------------------------------------

# Helper script used in the tests.

# -----------------------------------------------------------------------------

# Still in use, as the only available with old macOS versions.
# https://docs.travis-ci.com/user/reference/osx/
function create_macos_data_file()
{
  local message="$1"
  local branch="$2"
  local base_url="$3"
  local data_file_path="$4"

rm -rf "${data_file_path}"

# Versions before 10.13 may work for general packages, but fail on toolchains
# since they do not have the expected system headers in:
# /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk

# Note: __EOF__ is NOT quoted to allow substitutions.
cat <<__EOF__ > "${data_file_path}"
{
  "request": {
    "message": "${message}",
    "branch": "${branch}",
    "config": {
      "merge_mode": "replace",
      "jobs": [
        {
          "name": "x64 macOS 11.3",
          "os": "osx",
          "arch": "amd64",
          "osx_image": " xcode12.5",
          "language": "minimal",
          "script": [
            "env | sort",
            "pwd",
            "DEBUG=${DEBUG} bash scripts/helper/tests/native-test.sh --base-url ${base_url}" 
          ]
        },
        {
          "name": "x64 macOS 11.2",
          "os": "osx",
          "arch": "amd64",
          "osx_image": " xcode12.4",
          "language": "minimal",
          "script": [
            "env | sort",
            "pwd",
            "DEBUG=${DEBUG} bash scripts/helper/tests/native-test.sh --base-url ${base_url}" 
          ]
        },
        {
          "name": "x64 macOS 11.1",
          "os": "osx",
          "arch": "amd64",
          "osx_image": " xcode12.3",
          "language": "minimal",
          "script": [
            "env | sort",
            "pwd",
            "DEBUG=${DEBUG} bash scripts/helper/tests/native-test.sh --base-url ${base_url}" 
          ]
        },
        {
          "name": "x64 macOS 10.15",
          "os": "osx",
          "arch": "amd64",
          "osx_image": "xcode11.5",
          "language": "minimal",
          "script": [
            "env | sort",
            "pwd",
            "DEBUG=${DEBUG} bash scripts/helper/tests/native-test.sh --base-url ${base_url}" 
          ]
        },
        {
          "name": "x64 macOS 10.14",
          "os": "osx",
          "arch": "amd64",
          "osx_image": "xcode11.3",
          "language": "minimal",
          "script": [
            "env | sort",
            "pwd",
            "DEBUG=${DEBUG} bash scripts/helper/tests/native-test.sh --base-url ${base_url}" 
          ]
        },
        {
          "name": "x64 macOS 10.13",
          "os": "osx",
          "arch": "amd64",
          "osx_image": "xcode10.1",
          "language": "minimal",
          "script": [
            "env | sort",
            "pwd",
            "DEBUG=${DEBUG} bash scripts/helper/tests/native-test.sh --base-url ${base_url}" 
          ]
        }
      ],
      "notifications": {
        "email": {
          "on_success": "always",
          "on_failure": "always"
        }
      }
    }
  }
}
__EOF__

cat "${data_file_path}"
}


# data_file_path
# github_org
# github_repo
function trigger_travis()
{
  local github_org="$1"
  local github_repo="$2"
  local data_file_path="$3"
  local token="$4"

  curl \
    --request POST \
    --include \
    --header "Content-Type: application/json" \
    --header "Accept: application/json" \
    --header "Travis-API-Version: 3" \
    --header "Authorization: token ${token}" \
    --data-binary @"${data_file_path}" \
    https://api.travis-ci.com/repo/${github_org}%2F${github_repo}/requests
    # Warning: Do not add a trailing slash, it'll fail!

  rm -rf "${data_file_path}"
}

# -----------------------------------------------------------------------------

function trigger_github_workflow()
{
  local github_org="$1"
  local github_repo="$2"
  local workflow_id="$3"
  local data_file_path="$4"
  local token="$5"

  echo
  echo "Request body:"
  cat "${data_file_path}"

  # This script requires an authentication token in the environment.
  # https://docs.github.com/en/rest/reference/actions#create-a-workflow-dispatch-event

  echo
  echo "Response:"
  
  curl \
    --request POST \
    --include \
    --header "Authorization: token ${token}" \
    --header "Content-Type: application/json" \
    --header "Accept: application/vnd.github.v3+json" \
    --data-binary @"${data_file_path}" \
    https://api.github.com/repos/${github_org}/${github_repo}/actions/workflows/${workflow_id}/dispatches

  rm -rf "${data_file_path}"
}

# -----------------------------------------------------------------------------

# Runs natively or inside a container.
#
# Sets the following variables:
#
# - TARGET_PLATFORM=node_platform={win32,linux,darwin}
# - TARGET_ARCH=node_architecture={x64,ia32,arm64,arm}
# - bits={32,64}

function detect_architecture()
{
  uname -a

  uname_platform=$(uname -s | tr '[:upper:]' '[:lower:]')
  uname_machine=$(uname -m | tr '[:upper:]' '[:lower:]')

  node_platform="${uname_platform}"
  # Travis uses Msys2; git for Windows uses mingw-w64.
  if [[ "${uname_platform}" == msys_nt* ]] \
  || [[ "${uname_platform}" == mingw64_nt* ]] \
  || [[ "${uname_platform}" == mingw32_nt* ]]
  then
    node_platform="win32"
  fi

  node_architecture=""
  bits=""
  force_32_bit="${force_32_bit:-""}"

  if [ "${uname_machine}" == "x86_64" ]
  then
    if [ "${force_32_bit}" == "y" ]
    then
      node_architecture="ia32"
      bits="32"
    else
      node_architecture="x64"
      bits="64"
    fi
  elif [ "${uname_machine}" == "i386" -o "${uname_machine}" == "i586" -o "${uname_machine}" == "i686" ]
  then
    node_architecture="ia32"
    bits="32"
  elif [ "${uname_machine}" == "aarch64" ]
  then
    if [ "${force_32_bit}" == "y" ]
    then
      node_architecture="arm"
      bits="32"
    else
      node_architecture="arm64"
      bits="64"
    fi
  elif [ "${uname_machine}" == "armv7l" -o "${uname_machine}" == "armv8l" ]
  then
    node_architecture="arm"
    bits="32"
  else
    echo "${uname_machine} not supported"
    exit 1
  fi

  export TARGET_PLATFORM="${node_platform}"
  export TARGET_ARCH="${node_architecture}"
}

# Runs natively or inside a container.
#
# Sets the following variables:
# - WORK_FOLDER_PATH
# - repo_folder_path
# - RELEASE_VERSION
# - LD_LIBRARY_PATH
# - XBB_FOLDER_PATH
# - TARGET_FOLDER_NAME
# - DOT_EXE
# - SHLIB_EXT
# - CROSS_COMPILE_PREFIX
# - HOST
# - TARGET
# - IS_DEVELOP

function prepare_env() 
{
  container_work_folder_path="/Host/Work"
  container_repo_folder_path="/Host/repo"

  export CI=${CI:-"false"}

  if [ -f "/.dockerenv" -a -d "${container_work_folder_path}" ]
  then
    WORK_FOLDER_PATH="${container_work_folder_path}"
    repo_folder_path="${container_repo_folder_path}"
  else
    WORK_FOLDER_PATH="${HOME}/Work"
    # On the host, it must be called using the script folder path.
    repo_folder_path="$1"
  fi

  # Extract only the first line
  RELEASE_VERSION="${RELEASE_VERSION:-$(get_current_version ${repo_folder_path}/scripts/VERSION)}"

  if [ -z "${RELEASE_VERSION}" ]
  then
    echo "Check the version, it cannot be empty."
    exit 1
  fi

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

  TARGET_FOLDER_NAME="${TARGET_PLATFORM}-${TARGET_ARCH}"

  SOURCES_FOLDER_PATH=${SOURCES_FOLDER_PATH:-"${WORK_FOLDER_PATH}/${TARGET_FOLDER_NAME}/sources"}

  # Always in the user home, even when inside a container.
  test_xpacks_folder_path="${WORK_FOLDER_PATH}/${TARGET_FOLDER_NAME}/tests"
  cache_folder_path="${WORK_FOLDER_PATH}/cache"

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

    # do_config_guess

    DOT_EXE=".exe"
    SHLIB_EXT="dll"

    HOST="${CROSS_COMPILE_PREFIX}"
    TARGET="${HOST}"

  elif [ "${TARGET_PLATFORM}" == "darwin" ]
  then

    SHLIB_EXT="dylib"

    do_config_guess

    HOST="${BUILD}"
    TARGET="${HOST}"

  elif [ "${TARGET_PLATFORM}" == "linux" ]
  then

    SHLIB_EXT="so"

    do_config_guess

    HOST="${BUILD}"
    TARGET="${HOST}"

  else
    echo "Oops! Unsupported TARGET_PLATFORM=${TARGET_PLATFORM}."
    exit 1
  fi

  IS_DEVELOP=${IS_DEVELOP:-""}
}

# -----------------------------------------------------------------------------

# Requires BASE_URL and lots of other variables.
function install_archive()
{
  local archive_extension
  local archive_architecture="${node_architecture}"
  if [ "${node_platform}" == "win32" ]
  then
    archive_extension="zip"
    if [ "${force_32_bit}" == "y" ]
    then
      archive_architecture="ia32"
    fi
  else
    archive_extension="tar.gz"
  fi
  archive_name="${DISTRO_LC_NAME}-${APP_LC_NAME}-${RELEASE_VERSION}-${node_platform}-${archive_architecture}.${archive_extension}"
  archive_folder_name="${DISTRO_LC_NAME}-${APP_LC_NAME}-${RELEASE_VERSION}"

  mkdir -pv "${cache_folder_path}"

  if [ -f "${cache_folder_path}/${archive_name}" -a "${CI:-""}" != "true" ]
  then
    echo
    echo "Using cached "${cache_folder_path}/${archive_name}"..."
  else
    echo
    echo "Downloading ${archive_name}..."
    curl -L --fail -o "${cache_folder_path}/${archive_name}" \
      "${BASE_URL}/${archive_name}"
    echo
  fi

  app_folder_path="${test_xpacks_folder_path}/${archive_folder_name}"

  rm -rf "${app_folder_path}"

  mkdir -pv "${test_xpacks_folder_path}"
  cd "${test_xpacks_folder_path}"

  echo
  echo "Extracting ${archive_name}..."
  if [[ "${archive_name}" == *.zip ]]
  then
    unzip -q "${cache_folder_path}/${archive_name}"
  else 
    tar xf "${cache_folder_path}/${archive_name}"
  fi

  echo ls -lL "${app_folder_path}"
  ls -lL "${app_folder_path}"

  export APP_PREFIX="${app_folder_path}"
}

# -----------------------------------------------------------------------------

function install_xpm()
{
  curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/master/install.sh | bash
  
  export NVM_DIR="${HOME}/.nvm"
  [ -s "${NVM_DIR}/nvm.sh" ] && \. "${NVM_DIR}/nvm.sh"  # This loads nvm
  [ -s "${NVM_DIR}/bash_completion" ] && \. "${NVM_DIR}/bash_completion"  # This loads nvm bash_completion

  nvm install --lts node
  nvm use --lts node

  nvm install-latest-npm

  npm install --global xpm@latest
}

# -----------------------------------------------------------------------------

function docker_run_test() {

  local script_name="none.sh"
  local prefix32=""
  local image_name="none"
  local version=${RELEASE_VERSION:-"$(cat "${script_folder_path}"/VERSION | sed -e '2,$d')"}
  local base_url="release"

  while [ $# -gt 0 ]
  do
    case "$1" in

      --script)
        script_name="$2"
        shift 2
        ;;

      --32)
        prefix32="linux32"
        shift
        ;;

      --image)
        image_name="$2"
        shift 2
        ;;

      --version)
        version="$2"
        shift 2
        ;;

      --base-url)
        base_url="$2"
        shift 2
        ;;

      --*)
        echo "Unsupported option $1."
        exit 1
        ;;

    esac
  done

  (
    prefix32="${prefix32:-""}"

    # For --security-opt see:
    # https://github.com/tianon/docker-brew-ubuntu-core/issues/183

    run_verbose docker run \
      --tty \
      --security-opt seccomp:unconfined \
      --hostname "docker" \
      --workdir="/root" \
      --env DEBUG=${DEBUG} \
      --volume "${WORK_FOLDER_PATH}:${container_work_folder_path}" \
      --volume "${repo_folder_path}:${container_repo_folder_path}" \
      "${image_name}" \
      ${prefix32} /bin/bash "${container_repo_folder_path}/${script_name}" \
        --image "${image_name}" \
        --version "${version}" \
        --base-url "${base_url}"
  )
}

# -----------------------------------------------------------------------------

function show_libs()
{
  # Does not include the .exe extension.
  local app_path=$1
  shift
  if [ "${node_platform}" == "win32" ] && [[ ${app_path} != *.dll ]]
  then
    app_path+='.exe'
  fi
  
  if [ -f "${app_path}" ]
  then
    run_verbose ls -l "${app_path}"
  fi

  if [ "${node_platform}" == "linux" ]
  then
    echo
    echo "[readelf -d ${app_path} | egrep -i ...]"
    readelf -d "${app_path}" | egrep -i '(SONAME)' || true
    readelf -d "${app_path}" | egrep -i '(RUNPATH|RPATH)' || true
    readelf -d "${app_path}" | egrep -i '(NEEDED)' || true
    echo
    echo "[ldd -v ${app_path}]"
    ldd -v "${app_path}" 2>&1 || true
  elif [ "${node_platform}" == "darwin" ]
  then
    echo
    echo "[otool -L ${app_path}]"
    otool -L "${app_path}"
  elif [ "${node_platform}" == "win32" ]
  then
    : # Unfortunately objdump is not available on regular Windows.
    # echo
    # echo "[${CROSS_COMPILE_PREFIX}-objdump -x ${app_path}]"
    # ${CROSS_COMPILE_PREFIX}-objdump -x ${app_path} | grep -i 'DLL Name' || true
  fi
}

function good_bye()
{
  echo
  echo "All ${APP_LC_NAME} ${RELEASE_VERSION} tests completed successfully."

  run_verbose uname -a
  if [ "${node_platform}" == "linux" ]
  then
    # On opensuse/tumbleweed:latest it fails:
    # /usr/bin/lsb_release: line 122: getopt: command not found
    run_verbose lsb_release -a || true
    run_verbose ldd --version
  elif [ "${node_platform}" == "darwin" ]
  then
    run_verbose sw_vers
  fi

if [ ! -f "/.dockerenv" -a ! -d ".content/bin" -a "${CI:-""}" != "true" ]
  then
    echo
    echo "To remove the temporary folders, use: ' rm -rf ${test_xpacks_folder_path} '."
    echo "This test also leaves a folder in ~/Downloads and an archive in ${cache_folder_path}."
  fi
}

# -----------------------------------------------------------------------------
