---
title:  xPack OpenOCD {{ RELEASE_VERSION }} released

TODO: select one summary

summary: "Version {{ RELEASE_VERSION }} is a maintenance release; it updates to
the latest upstream master."

summary: "Version {{ RELEASE_VERSION }} is a new release; it follows the upstream release."

version: {{ RELEASE_VERSION }}
npm_subversion: 1
download_url: https://github.com/xpack-dev-tools/openocd-xpack/releases/tag/v{{ RELEASE_VERSION }}/

date:   {{ RELEASE_DATE }}

categories:
  - releases
  - openocd

tags:
  - releases
  - openocd

---

[The xPack OpenOCD](https://xpack.github.io/openocd/)
is a standalone cross-platform binary distribution of
[OpenOCD](http://openocd.org).

There are separate binaries for **Windows** (Intel 32/64-bit),
**macOS** (Intel 64-bit) and **GNU/Linux** (Intel 32/64-bit, Arm 32/64-bit).

{% raw %}{% include note.html content="The main targets for the Arm binaries
are the **Raspberry Pi** class devices." %}{% endraw %}

## Download

The binary files are available from GitHub [releases]({% raw %}{{ page.download_url }}{% endraw %}).

## Install

The full details of installing the **xPack OpenOCD** on various platforms
are presented in the separate
[Install]({% raw %}{{ site.baseurl }}{% endraw %}/openocd/install/) page.

### Easy install

The easiest way to install OpenOCD is with
[`xpm`]({% raw %}{{ site.baseurl }}{% endraw %}/xpm/)
by using the **binary xPack**, available as
[`@xpack-dev-tools/openocd`](https://www.npmjs.com/package/@xpack-dev-tools/openocd)
from the [`npmjs.com`](https://www.npmjs.com) registry.

With the `xpm` tool available, installing
the latest version of the package and adding it as
a dependency for a project is quite easy:

```sh
cd my-project
xpm init # Only at first use.

xpm install @xpack-dev-tools/openocd@latest

ls -l xpacks/.bin
```

To install this specific version, use:

```sh
xpm install @xpack-dev-tools/openocd@{% raw %}{{ page.version }}.{{ page.npm_subversion }}{% endraw %}
```

For xPacks aware tools, like the **Eclipse Embedded C/C++ plug-ins**,
it is also possible to install OpenOCD globally, in the user home folder.

```sh
xpm install --global @xpack-dev-tools/openocd@latest
```

Eclipse will automatically
identify binaries installed with
`xpm` and provide a convenient method to manage paths.

### Uninstall

To remove the links from the current project:

```sh
cd my-project

xpm uninstall @xpack-dev-tools/openocd
```

To completely remove the package from the global store:

```sh
xpm uninstall --global @xpack-dev-tools/openocd
```

## Compliance

The xPack OpenOCD generally follows the official
[OpenOCD](http://openocd.org) releases.

The current version is based on:

TODO: update commit id and date.

- OpenOCD version 0.11.0, the development commit
[<xxxxxxx>](https://github.com/xpack-dev-tools/openocd/commit/<xxxxxxxxxxxxx>)
from <ddddddddd>.

## Changes

There are no functional changes.

Compared to the upstream, the following changes were applied:

- a configure option was added to configure branding (`--enable-branding`)
- the `src/openocd.c` file was edited to display the branding string
- the `contrib/60-openocd.rules` file was simplified to avoid protection
  related issues.

## Bug fixes

- none

## Enhancements

- none

## Known problems

- none

## Shared libraries

On all platforms the packages are standalone, and expect only the standard
runtime to be present on the host.

All dependencies that are build as shared libraries are copied locally
in the `libexec` folder (or in the same folder as the executable for Windows).

### `DT_RPATH` and `LD_LIBRARY_PATH`

On GNU/Linux the binaries are adjusted to use a relative path:

```console
$ readelf -d library.so | grep runpath
 0x000000000000001d (RPATH)            Library rpath: [$ORIGIN]
```

In the GNU ld.so search strategy, the `DT_RPATH` has
the highest priority, higher than `LD_LIBRARY_PATH`, so if this later one
is set in the environment, it should not interfere with the xPack binaries.

Please note that previous versions, up to mid-2020, used `DT_RUNPATH`, which
has a priority lower than `LD_LIBRARY_PATH`, and does not tolerate setting
it in the environment.

### `@executable_path`

Similarly, on macOS, the dynamic libraries are adjusted with `otool` to use a
relative path.

## Documentation

The original documentation is available in the `share/doc` folder.

## Supported platforms

Binaries for **Windows**, **macOS** and **Intel/Arm GNU/Linux** are provided.

The binaries were built using the
[xPack Build Box (XBB)](https://github.com/xpack/xpack-build-box), a set
of build environments based on slightly older distributions, that should be
compatible with most recent systems.

- Intel GNU/Linux: all binaries were built with GCC 11.1, running in an
  Ubuntu 12 Docker container
- Arm GNU/Linux: all binaries were built with GCC 11.1, running in an
  Ubuntu 16 Docker container (added in mid-2020)
- Windows: all binaries were built with mingw-w64 GCC 11.1, running in an
  Ubuntu 12 Docker container
- macOS: all binaries were built with GCC 11.1, running in a separate
  folder on macOS 10.13.6, but were tested and also run on 10.10.

## Build

The scripts used to build this distribution are in:

- `distro-info/scripts`

For the prerequisites and more details on the build procedure, please see the
[How to build](https://github.com/xpack-dev-tools/openocd-xpack/blob/xpack/README-BUILD.md) page.

## CI tests

Before publishing, a set of simple tests were performed on an exhaustive
set of platforms. The results are available from:

- [GitHub Actions](https://github.com/xpack-dev-tools/openocd-xpack/actions/)
- [travis-ci.com](https://app.travis-ci.com/github/xpack-dev-tools/openocd-xpack/builds/)

## Tests

The binaries were testes on Windows 10 Pro 32/64-bit, Intel Ubuntu 18
LTS 64-bit, Intel Xubuntu 18 LTS 32-bit and macOS 10.15.

Install the package with xpm.

The simple test, consists in starting the binaries
only to identify the STM32F4DISCOVERY board.

```sh
.../xpack-openocd-{{ RELEASE_VERSION }}/bin/openocd -f board/stm32f4discovery.cfg
```

A more complex test consist in programming and debugging a simple blinky
application on the STM32F4DISCOVERY board. The binaries were
those generated by
[simple Eclipse projects](https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack/tree/xpack/tests/eclipse)
available in the **xPack GNU Arm Embedded GCC** project.

## Checksums

The SHA-256 hashes for the files are:
