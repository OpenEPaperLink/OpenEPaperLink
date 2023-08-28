# Nordic nRF52: development platform for [PlatformIO](https://platformio.org)

[![Build Status](https://github.com/platformio/platform-nordicnrf52/workflows/Examples/badge.svg)](https://github.com/platformio/platform-nordicnrf52/actions)

The nRF52 Series are built for speed to carry out increasingly complex tasks in the shortest possible time and return to sleep, conserving precious battery power. They have a Cortex-M4F processor which makes them quite capable Bluetooth Smart SoCs.

* [Home](https://registry.platformio.org/platforms/platformio/nordicnrf52) (home page in the PlatformIO Registry)
* [Documentation](https://docs.platformio.org/page/platforms/nordicnrf52.html) (advanced usage, packages, boards, frameworks, etc.)

# Usage

1. [Install PlatformIO](https://platformio.org)
2. Create PlatformIO project and configure a platform option in [platformio.ini](https://docs.platformio.org/page/projectconf.html) file:

## Stable version

```ini
[env:stable]
platform = nordicnrf52
board = ...
...
```

## Development version

```ini
[env:development]
platform = https://github.com/platformio/platform-nordicnrf52.git
board = ...
...
```

# Configuration

Please navigate to [documentation](https://docs.platformio.org/page/platforms/nordicnrf52.html).
