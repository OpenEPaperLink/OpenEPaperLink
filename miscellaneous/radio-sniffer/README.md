# OEPL radio sniffer

Passive IEEE 802.15.4 sniffer for timing measurements on the OpenEPaperLink radio protocol (measurement M2 in [docs/design/radio-chain-tasks.md](../../docs/design/radio-chain-tasks.md)). It never transmits.

- `firmware/` – ESP-IDF project for any ESP32-C6 (or H2) devboard. Frames go out over the native USB port (USB-Serial-JTAG) with their SFD timestamp (µs), RSSI and LQI. GPIO 2 toggles on every frame to or from one tag, for syncing with a power profiler (PPK2 logic input D0).
- `host/oepl_sniff.py` – reads the sniffer, decodes the OEPL packets, measures AP reply times and block bursts, writes a pcap (LINKTYPE_IEEE802_15_4_NOFCS) for Wireshark. Needs `pyserial`.

## Build and flash

Connect the devboard's native USB port (the one marked "USB", not the UART bridge).

```powershell
. $env:IDF_PATH\export.ps1
cd miscellaneous\radio-sniffer\firmware
idf.py set-target esp32c6
idf.py -p COM11 flash
```

Tested with ESP-IDF v5.5.1 on an ESP32-C6 rev 0.2.

## Run

```bash
python host/oepl_sniff.py --port COM11 --channel 25 --duration 600 --pcap run.pcap
```

Options: `-v` prints every OEPL packet, `--summary-every 60` prints intermediate summaries, `--sync-mac <MAC as in the web UI>` enables the sync GPIO for that tag.

## What the summary means

- **reply time**: end of the tag's request → end of the AP's reply, on air. Compare with the tag windows in the timing budget of [radio-chain-analysis.md](../../docs/design/radio-chain-analysis.md): ping 2 ms (ZBS243), check-in 5 ms, block request 6 ms (nRF52811), XFC ack 6 ms.
- **unanswered**: a tag request with no matching AP reply before the tag's next request. Can also be a reply the sniffer missed.
- **attempts per check-in**: how many requests a tag needed before it got an answer.
- **block bursts**: `pleaseWaitMs` from the ack, burst start relative to it, frames sent, distinct parts, and frames sent after every requested part had been sent once.
- Frames from other 802.15.4 networks on the same channel are ignored (only PAN 0x4447 and 0x1337 are decoded).
- The sniffer can miss frames itself; put it close to the AP and the tags.
