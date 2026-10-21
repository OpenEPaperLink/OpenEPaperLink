# OEPL AP test tool

`aptool.py` drives an OpenEPaperLink AP over its web interface from the command line: show what runs on it, list tags, back up and restore a tag's settings, flash S3 or C6/H2 firmware and follow the AP's websocket.

It is meant mainly for agents (Claude Code and the like) that test firmware changes on a real AP, and for developers who want the same steps as one command. The web UI does everything here too; this tool only makes it repeatable and scriptable.

Python 3, standard library only (no `pip install`).

## AP address

The tool has no built-in address. Give it with `--ap`, or set the environment variable `OEPL_AP` once per shell:

```bash
python miscellaneous/ap-tool/aptool.py --ap 192.168.1.50 info
```

```bat
set OEPL_AP=192.168.1.50
python miscellaneous\ap-tool\aptool.py info
```

`host:port` works as well.

## Commands

| Command | What it does |
| --- | --- |
| `info` | Alias, build environment, firmware version and SHA, radio type and version, channel, reset reason, whether the last boot was a crash and whether a rollback is possible (`/sysinfo`). |
| `tags` | All tags (every page of `/get_db`) with seconds since last seen, hardware type and alias. |
| `backup MAC FILE` | Saves the tag's record from `/get_db` as json. |
| `restore FILE` | Puts back alias, content mode, content settings, rotation, LUT and invert from a backup (`/save_cfg`) and checks every field afterwards, then has the AP save its database to flash (`/backup_db`). The AP renders the tag again. |
| `ota FIRMWARE.bin` | Flashes S3 firmware: serves the file from this PC, starts `/update_ota` with its md5 and size, waits for "Firmware update successful" on `/ws`, then reboots the AP and waits until it answers. `--no-reboot` leaves the reboot to you. |
| `radio DIR` | Flashes the C6/H2: serves `DIR`, which must hold the `firmware_*.json` and the files it names (for example `binaries/ESP32-C6`), starts `/update_c6` and waits until the AP reports the new radio version. The AP reboots by itself afterwards. |
| `upload FILE PATH` | Puts a file on the AP's filesystem through `/littlefs_put`, for example a web file: `upload ESP32_AP-Flasher/data/www/main.js.gz www/main.js.gz`. Leave out the leading `/` in Git Bash: it turns `/www/...` into a Windows path (the tool refuses that). Reload the web UI afterwards (the browser may cache it). |
| `watch` | Prints `/ws` messages with a timestamp. `--seconds N` (default 60), `--until TEXT` (repeatable; stops at the first message containing it, exit code 1 if it never comes), `--out FILE` (append), `--console` (console messages only). |
| `reboot` | `/reboot`; `--wait` waits until the AP answers again. |

`ota` and `radio` start a temporary HTTP server on a free port of this PC, on the interface that faces the AP. The firewall must let the AP reach it; Windows asks the first time Python listens on a network.

## Pitfalls

- The S3 OTA write takes about 90 s and the AP does not reboot by itself. A reboot before "Firmware update successful" aborts the update silently; `ota` waits for that message.
- Flashing and rebooting interrupt the AP for its users. Agents: only do this on an AP whose owner allowed it.
- `restore` makes the AP render and send the tag's image again, so a battery tag wakes up for it.
- `/get_db` is paged; `tags` reads all pages, a single `curl` does not.
- Don't upload firmware through `/edit` (the file editor): large uploads there can crash the AP. Use `ota`.
