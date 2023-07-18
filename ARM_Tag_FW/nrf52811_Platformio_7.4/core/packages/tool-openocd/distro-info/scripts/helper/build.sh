#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# This file is part of the xPack distribution.
#   (https://xpack.github.io)
# Copyright (c) 2019 Liviu Ionescu.
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

build_script_path="$0"
if [[ "${build_script_path}" != /* ]]
then
  # Make relative path absolute.
  build_script_path="$(pwd)/$0"
fi

script_folder_path="$(dirname "${build_script_path}")"
script_folder_name="$(basename "${script_folder_path}")"

# =============================================================================

scripts_folder_path="$(dirname $(dirname "${script_folder_path}"))/scripts"
helper_folder_path="${scripts_folder_path}/helper"

# -----------------------------------------------------------------------------

# Script to build the xPack distribution packages.
#
# Developed on macOS 10.13 High Sierra, but also intended to run on
# Intel & Arm Linux XBB.

# -----------------------------------------------------------------------------

source "${scripts_folder_path}/defs-source.sh"

echo
echo "${APP_DESCRIPTION} distribution build script."

# Helper functions.
source "${helper_folder_path}/common-functions-source.sh"
source "${helper_folder_path}/host-functions-source.sh"

# The order is important, it may override helper defs.
if [ -f "${scripts_folder_path}/common-functions-source.sh" ]
then
  source "${scripts_folder_path}/common-functions-source.sh"
fi

host_detect

# For clarity, explicitly define the docker images here.
docker_linux64_image=${docker_linux64_image:-"ilegeul/ubuntu:amd64-12.04-xbb-v3.3"}
docker_linux32_image=${docker_linux32_image:-"ilegeul/ubuntu:i386-12.04-xbb-v3.3"}
docker_linux_arm64_image=${docker_linux_arm64_image:-"ilegeul/ubuntu:arm64v8-16.04-xbb-v3.3"}
docker_linux_arm32_image=${docker_linux_arm32_image:-"ilegeul/ubuntu:arm32v7-16.04-xbb-v3.3"}

# -----------------------------------------------------------------------------

# Array where the remaining args will be stored.
declare -a rest

help_message="    bash $0 [--win32] [--win64] [--linux32] [--linux64] [--arm32] [--arm64] [--osx] [--all] [clean|cleanlibs|cleanall|preload-images] [--env-file file] [--disable-strip] [--without-pdf] [--with-html] [--disable-multilib] [--develop] [--debug] [--use-gits] [--jobs N] [--help]"
host_options "${help_message}" "$@"

host_common

# -----------------------------------------------------------------------------

host_build_all

host_show_sha

# -----------------------------------------------------------------------------

host_stop_timer

host_notify_completed

# Completed successfully.
exit 0

# -----------------------------------------------------------------------------
