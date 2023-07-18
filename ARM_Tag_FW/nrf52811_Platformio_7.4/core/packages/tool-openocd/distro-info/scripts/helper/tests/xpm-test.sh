#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# This file is part of the xPack distribution.
#   (https://xpack.github.io)
# Copyright (c) 2020 Liviu Ionescu.
#
# Permission to use, copy, modify, and/or distribute this software 
# for any purpose is hereby granted, under the terms of the MIT license.
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Safety settings (see https://gist.github.com/ilg-ul/383869cbb01f61a51c4d).

if [[ ! -z ${DEBUG} ]]
then
  set ${DEBUG} # Activate the expand mode if DEBUG is anything but empty.
else
  DEBUG=""
fi

set -o errexit # Exit if command failed.
set -o pipefail # Exit if pipe failed.
set -o nounset # Exit if variable not set.

# Remove the initial space and instead use '\n'.
IFS=$'\n\t'

# -----------------------------------------------------------------------------
# Identify the script location, to reach, for example, the helper scripts.

script_path="$0"
if [[ "${script_path}" != /* ]]
then
  # Make relative path absolute.
  script_path="$(pwd)/$0"
fi

script_name="$(basename "${script_path}")"

script_folder_path="$(dirname "${script_path}")"
script_folder_name="$(basename "${script_folder_path}")"

# =============================================================================

scripts_folder_path="$(dirname $(dirname "${script_folder_path}"))"
helper_folder_path="${scripts_folder_path}/helper"

# -----------------------------------------------------------------------------

source "${scripts_folder_path}/defs-source.sh"

# Helper functions
source "${helper_folder_path}/common-functions-source.sh"
source "${helper_folder_path}/common-apps-functions-source.sh"
source "${helper_folder_path}/test-functions-source.sh"

# Reuse the test functions defined in the build scripts.
source "${scripts_folder_path}/common-apps-functions-source.sh"

# Common native & docker functions (like run_tests()).
source "${scripts_folder_path}/tests/common-functions-source.sh"

# -----------------------------------------------------------------------------

force_32_bit=""
image_name=""
RELEASE_VERSION="${RELEASE_VERSION:-latest}"

while [ $# -gt 0 ]
do
  case "$1" in

    --32)
      force_32_bit="y"
      shift
      ;;

    --image)
      image_name="$2"
      shift 2
      ;;

    --version)
      RELEASE_VERSION="$2"
      shift 2
      ;;

    --*)
      echo "Unsupported option $1."
      exit 1
      ;;

    *)
      echo "Unsupported arg $1."
      exit 1
      ;;

  esac
done

# -----------------------------------------------------------------------------

if [ -f "/.dockerenv" ]
then
  if [ -n "${image_name}" ]
  then
    # When running in a Docker container, update it.
    update_image "${image_name}"
  else
    echo "No image defined, quit."
    exit 1
  fi

  # The Debian npm docker images have nvm installed in the /root folder.
  if [ -d "/root/.nvm" ]
  then
    export NVM_DIR="/root/.nvm"
    [ -s "$NVM_DIR/nvm.sh" ] && \. "$NVM_DIR/nvm.sh"
    [ -s "$NVM_DIR/bash_completion" ] && \. "$NVM_DIR/bash_completion"
    
    hash -r
  fi
fi

# -----------------------------------------------------------------------------

detect_architecture

prepare_env "$(dirname ${scripts_folder_path})"

# -----------------------------------------------------------------------------

test_xpm_folder_path="${WORK_FOLDER_PATH}/${TARGET_FOLDER_NAME}/tests/${APP_LC_NAME}"

rm -rf "${test_xpm_folder_path}"
mkdir -p "${test_xpm_folder_path}"
cd "${test_xpm_folder_path}"
run_verbose pwd

run_verbose npm install --global xpm

run_verbose xpm init
if [ "${force_32_bit}" == "y" ]
then
  run_verbose xpm install ${NPM_PACKAGE} --force-32bit
else
  run_verbose xpm install ${NPM_PACKAGE}
fi

# -----------------------------------------------------------------------------

run_tests

good_bye

# Completed successfully.
exit 0

# -----------------------------------------------------------------------------
