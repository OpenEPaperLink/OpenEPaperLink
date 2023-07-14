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

scripts_folder_path="$(dirname $(dirname $(dirname "${script_folder_path}")))/scripts"
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

if [ $# -lt 1 ]
then
  echo "usage: $(basename $0) [--32] [--version X.Y.Z] --base-url URL"
  exit 1
fi

force_32_bit=""
image_name=""
RELEASE_VERSION="${RELEASE_VERSION:-$(get_current_version)}"
BASE_URL="${BASE_URL:-release}"

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

    --base-url)
      BASE_URL="$2"
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

echo "BASE_URL=${BASE_URL}"

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
fi

# -----------------------------------------------------------------------------

detect_architecture

prepare_env "$(dirname "${scripts_folder_path}")"

if [ "${BASE_URL}" == "release" ]
then
  BASE_URL=https://github.com/xpack-dev-tools/${APP_LC_NAME}-xpack/releases/download/${RELEASE_VERSION}/
fi

install_archive

run_tests

good_bye

# Completed successfully.
exit 0

# -----------------------------------------------------------------------------
