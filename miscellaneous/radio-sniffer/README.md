# OEPL radio sniffer

Passive IEEE 802.15.4 sniffer for timing measurements on the OpenEPaperLink radio protocol. It never transmits, so it can run next to a live AP without disturbing it.

It is measurement **M2** of the radio chain work. Context, finding IDs and the timing budget are in [docs/design/radio-chain-analysis.md](../../docs/design/radio-chain-analysis.md); what to measure and where results go is in [docs/design/radio-chain-tasks.md](../../docs/design/radio-chain-tasks.md).

| Part | What it does |
| --- | --- |
| `firmware/` | ESP-IDF project for any ESP32-C6 or ESP32-H2 devboard. Radio in promiscuous mode; every frame goes to the PC over the native USB port with its SFD timestamp (µs), RSSI and LQI. GPIO 2 toggles on every frame to or from one tag (for a power profiler). |
| `host/oepl_sniff.py` | Reads the sniffer, decodes OEPL packets, reports AP reply times, check-in attempts and block bursts, writes a pcap for Wireshark. Python 3 + `pyserial`. |

## Quick start

1. Connect the devboard's **native USB port** (marked "USB", not "UART"). Windows shows it as "USB JTAG/serial debug unit", VID:PID `303A:1001`. Find the port with `python -m serial.tools.list_ports -v`.
2. Build and flash (once per devboard):

   ```powershell
   $env:IDF_PATH = "C:\Users\nic\esp\v5.5.1\esp-idf"   # ESP-IDF v5.5.1 on Nic's PC
   . "$env:IDF_PATH\export.ps1"
   cd miscellaneous\radio-sniffer\firmware
   idf.py set-target esp32c6        # or esp32h2
   idf.py -p COM11 flash
   ```

3. Find the AP's channel: `curl http://<ap-ip>/sysinfo` → field `channel` (Nic's Yellow AP: 192.168.178.198, channel 25).
4. Run:

   ```bash
   python miscellaneous/radio-sniffer/host/oepl_sniff.py --port COM11 --channel 25 --duration 600 --pcap run.pcap
   ```

| Option | Meaning |
| --- | --- |
| `--port` | Sniffer's serial port (required). |
| `--channel` | 802.15.4 channel, 11–26. Default 25. |
| `--duration` | Seconds to run; 0 = until Ctrl-C. The summary prints at the end. |
| `--pcap FILE` | Also write all frames (OEPL and others) to a pcap, LINKTYPE 230 (802.15.4 without FCS). |
| `--sync-mac MAC` | Toggle GPIO 2 on frames to/from this tag. MAC as the web UI shows it (e.g. `0000021EE3963B16`). |
| `--summary-every N` | Print an intermediate summary every N seconds. |
| `-v` | Print every OEPL packet with its timestamp. |

A run without block transfers only shows check-ins (reply median ≈ 2.4 ms on the Yellow AP). A useful measurement needs block transfers: send a new image to a tag (web UI or `/imgupload`) while the sniffer runs.

## Reading the summary

```
sniffer during this run: received N, dropped in sniffer queue N, dropped on USB N, channel N
packets: <OEPL packet type> <count>, ...
frames from other 802.15.4 networks (ignored): N
AP sequence gaps (AP did not send, or sniffer missed): N
reply time = end of tag request -> end of AP reply (ms)
  checkin    n=… unanswered=…  median …  p95 …  max …  over 5 ms: …
  check-ins: N, answered N, attempts per answered check-in: mean …, max …
block bursts:
  tag <MAC>: wait <pleaseWaitMs> ms, start vs wait <±ms>, <frames> frames (<distinct> distinct, <requested> requested, <missing> requested not seen), <ms> ms, <n> frames after all requested parts were sent
tags seen (hwType): <MAC>=<hwType hex>, ...
```

- **Reply time** is measured on air: end of the tag's request → end of the AP's reply. That is what has to fit in the tag's receive window. Windows (from the tag firmware, see the timing budget in the analysis): ping 2 ms on ZBS243 (5 ms on others), check-in 5 ms, block request 6 ms on nRF52811 (7–9 ms on ZBS243), XFC ack 6 ms. The `over X ms` counts are against those windows.
- **unanswered**: a tag request without a matching AP reply before that tag's next request. Either the AP did not answer in time, or the sniffer missed the reply.
- **attempts per answered check-in**: requests a tag needed before it got an `AVAIL_DATA_INFO`. More than 1 means missed windows (or the AP did not hear the tag).
- **block bursts**: per `BLOCK_REQUEST_ACK`: the `pleaseWaitMs` the AP gave, when the first `BLOCK_PART` actually came relative to that (`+` = late), how many frames were sent, how many distinct parts, how many requested parts were never seen, burst duration, and frames sent after every requested part had already gone out once (redundancy; ZBS243 and nRF52811 have stopped listening by then).
- **AP sequence gaps**: jumps in the AP's MAC sequence number. 0 means the sniffer saw every AP frame.
- **dropped in sniffer queue / on USB**: should be 0. If not, the sniffer lost frames itself and the numbers are incomplete.
- **hwType**: from `AVAIL_DATA_REQ`; identifies the tag model (see `tag_types.h` and `resources/tagtypes/`).

## Pitfalls

- Only one channel at a time, and no sub-GHz (CC1101) traffic.
- Other 802.15.4 networks (Zigbee, Thread) may share the channel. Only PAN 0x4447 (2.4 GHz) and 0x1337 (sub-GHz) are decoded; the rest is counted as "other networks".
- The sniffer can miss frames. Put it close to the AP *and* the tags; tags far from the sniffer show up as AP replies without a request.
- MAC byte order: over the air the MAC is little-endian; the web UI and this script show it reversed (last byte first). `--sync-mac` takes the web UI form.
- Timestamps are µs since the sniffer booted, taken in the SFD interrupt (jitter of a few µs). The pcap uses PC time at the first frame plus the sniffer's offsets.
- Opening the port does not reset the board (the script clears DTR/RTS before opening). Flashing does.
- While nobody reads the port, the sniffer keeps up to 16 KB of old records buffered and counts the rest as "dropped on USB". The script therefore drops everything that arrives in the first 0.5 s, and reports the sniffer counters relative to the start of the run.
- The C6 console (ESP-IDF logs) is on UART0, not on the native USB port; the USB port carries only sniffer records.

## Serial protocol (for extending the tools)

Device → host records, little-endian: `0xA5 0x5A`, type `u8`, payload length `u16`, payload, checksum `u8` (sum of payload bytes).

| Type | Payload |
| --- | --- |
| 1 FRAME | `ts_us u64, rssi i8, lqi u8, channel u8, len u8, frame[len]` (FCS included) |
| 2 STATUS (every 1 s) | `ts_us u64, received u32, dropped_queue u32, dropped_usb u32, channel u8, sync_on u8` |
| 3 HELLO (at boot) | `version u8, channel u8` |

Host → device commands, ASCII lines: `C<11..26>` set channel, `M<16 hex>` sync on frames with this MAC (over-the-air byte order), `M` sync off, `S` status now. Every command is answered with a STATUS record.

Script structure: `Reader` splits the byte stream into records (resyncs on the magic bytes), `decode()` parses the 802.15.4 header and the OEPL packet type, `Analyzer.feed()` pairs tag requests with AP replies and collects bursts, `Analyzer.summary()` prints the report, `Pcap` writes the capture. Packet types and reply pairing are in the `PKT` and `REPLIES` tables at the top; OEPL structs are in `oepl-proto.h`.

## Where results go

- Intermediate results: local `measurements/` at the repo root (not committed), one dated Markdown note per run with setup (AP, firmware versions, channel, sniffer position, duration) and the summary.
- Final results (baselines, before/after comparisons): `docs/design/radio-chain-results.md`.
