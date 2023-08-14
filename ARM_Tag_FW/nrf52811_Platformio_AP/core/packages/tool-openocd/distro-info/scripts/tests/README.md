# Scripts to test the xPack OpenOCD

The binaries can be available from one of the pre-releases:

- <https://github.com/xpack-dev-tools/pre-releases/releases>

## Download the repo

The test script is part of the xPack OpenOCD:

```sh
rm -rf ~/Downloads/openocd-xpack.git; \
git clone \
  --branch xpack-develop \
  https://github.com/xpack-dev-tools/openocd-xpack.git  \
  ~/Downloads/openocd-xpack.git; \
git -C ~/Downloads/openocd-xpack.git submodule update --init --recursive
```

## Start a local test

To check if OpenOCD starts on the current platform, run a native test:

```sh
bash ~/Downloads/openocd-xpack.git/scripts/helper/tests/native-test.sh \
  --base-url "https://github.com/xpack-dev-tools/pre-releases/releases/download/test/"
```

The script stores the downloaded archive in a local cache, and
does not download it again if available locally.

To force a new download, remove the local archive:

```sh
rm -rf ~/Work/cache/xpack-openocd-*
```

## Start the GitHub Actions tests

The multi-platform tests run on GitHub Actions; they do not fire on
git commits, but only via a manual POST to the GitHub API.

```sh
bash ~/Downloads/openocd-xpack.git/scripts/helper/tests/trigger-workflow-test-prime.sh \
  --branch xpack-develop \
  --base-url "https://github.com/xpack-dev-tools/pre-releases/releases/download/test/"

bash ~/Downloads/openocd-xpack.git/scripts/helper/tests/trigger-workflow-test-docker-linux-intel.sh \
  --branch xpack-develop \
  --base-url "https://github.com/xpack-dev-tools/pre-releases/releases/download/test/"

bash ~/Downloads/openocd-xpack.git/scripts/helper/tests/trigger-workflow-test-docker-linux-arm.sh \
  --branch xpack-develop \
  --base-url "https://github.com/xpack-dev-tools/pre-releases/releases/download/test/"

```

The results are available at the project
[Actions](https://github.com/xpack-dev-tools/openocd-xpack/actions/) page.
