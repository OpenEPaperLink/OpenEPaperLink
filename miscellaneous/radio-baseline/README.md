# Radio chain baseline

Repeatable measurement of how one tag experiences the radio protocol: on air with the [radio sniffer](../radio-sniffer/README.md) and in the tag's current with the [PPK2 capture](../ppk2-measure/README.md), aligned through PPK2 logic input D0. It is the baseline for phase 0 (M2 + M4) in [docs/design/radio-chain-tasks.md](../../docs/design/radio-chain-tasks.md); repeat it after every change to the AP's radio path and compare.

| Script | What it does |
| --- | --- |
| `run_baseline.py` | Generates a fixed set of noise test images, starts sniffer and PPK2 in parallel, waits for the tag to reboot and check in, uploads the images via `POST /imgupload` at a fixed interval, then runs the analysis. |
| `analyze_baseline.py` | Aligns the pcap with the PPK2 trace (k-th D0 edge = k-th frame to/from the tag) and reports boot, check-ins, XFC, every block and every transfer. Can be rerun on old captures. |

## Requirements

- Sniffer devboard flashed and connected (native USB), PPK2 powering the tag, sniffer GPIO 2 → PPK2 D0 with logic VCC and GND connected. Check with the `T` test in the PPK2 README.
- Python 3 with `pyserial`, `numpy`, `Pillow`, `ppk2-api==0.9.2`.
- The tag is registered on the AP, and the AP accepts `/imgupload` for it.

## Run

Nic's setup (Yellow AP, channel 25, 2.9" ZBS243 test tag on the PPK2):

```bash
python miscellaneous/radio-baseline/run_baseline.py --tag 0000021EDAAE3B17 --ap http://192.168.178.198 \
    --sniffer COM11 --ppk COM3 --reps 5 --interval 100 --out measurements/baseline-<date>/run1
```

| Option | Default | Meaning |
| --- | --- | --- |
| `--reps` | 5 | Image transfers. |
| `--interval` | 100 s | Time between uploads. The AP sends an upload at the tag's next check-in (≈ 40 s on this tag), so leave room for a check-in, the transfer and the display refresh. |
| `--settle` | 30 s | Wait after the PPK2 power-cycles the tag (boot ≈ 19 s on a 2.9" ZBS243). |
| `--tail` | 60 s | Keep measuring after the last upload. |
| `--size` / `--colors` | 296x128 / 3 | Test image size and palette (black/white/red). |
| `--seed` | 1000 | Images use seeds `seed … seed+reps-1`. Keep it the same to compare runs. |

Total time = settle + reps × interval + tail (≈ 10 minutes with the defaults). Output files (`<out>` = the `--out` base): `<out>.pcap`, `<out>-ppk.{npz,json,txt}`, `<out>-sniff.txt`, `<out>-uploads.txt`, `<out>-img<i>.jpg`, `<out>-analysis.{txt,json}`.

Re-analyse an existing run:

```bash
python miscellaneous/radio-baseline/analyze_baseline.py --pcap <out>.pcap --ppk <out>-ppk.npz --tag <MAC> --out <out>-analysis
```

## Reading the analysis

- **boot**: power-on to the end of the first check-in: duration, charge, pings sent and pongs received (AP scan).
- **check-ins**: split into answered without new data (single request / with retries), with new data, unanswered. Per group: requests, first reply (end of the first request → end of AVAIL_DATA_INFO, on air), awake time and charge of the tag's active episode around it. A retry roughly doubles the cost of a check-in.
- **XFC ack**: XFER_COMPLETE → ACK, on air.
- **blocks**, one line per block request:
  - `wait ms`: `pleaseWaitMs` from the AP's ACK.
  - `late ms`: first block part start − (ACK end + wait). Positive = the AP started late (finding E1).
  - `listen ms` / `listen µC`: tag radio on (current > 20 mA) before the first part arrived: wasted.
  - `rx ms`: radio on from waking until the tag switched it off (block complete).
  - `after off`: block parts the AP sent after the tag switched its radio off (finding E4).
  - `after all sent`: parts sent after every requested part had been sent once.
  - `charge µC`: from the start of the block request to radio off.
- **transfers**: data type (0x30 zlib, 0x31 G5, 0x20/0x21 raw), data size in bytes, blocks, time and charge from the AVAIL_DATA_INFO that announced the image to the XFC, refresh time, and charge including the display refresh.
- **per block (mean)**: the numbers to compare between runs.
- **AP side, all tags**: the sniffer's own summary over everything on the channel.

## Pitfalls

- Compare runs only with the same seed, size and colors, and check that `bytes` per transfer match (the JPEG bytes depend on the Pillow version; the AP's conversion on its firmware).
- If the analysis warns that frame and edge counts differ, the sniffer lost frames or D0 is not wired; the per-block numbers are then unreliable.
- The thresholds are for a ZBS243 (radio on > 20 mA, idle < 200 µA). Other tags need other values (`RADIO_ON_UA`, `IDLE_UA` at the top of `analyze_baseline.py`).
- Other tags on the AP keep checking in; they show up in the AP-side summary, not in the per-tag numbers.

## Longer runs with real traffic: `night_traffic.py`

Generates image-transfer traffic on the AP for an hour or a night, for the serial-integrity measurement (M3, together with [ap-console-log](../ap-console-log/README.md)) and to watch concurrent transfers on air.

- The test tag (on the PPK2, which the script keeps powered) gets an image every `--test-interval` s (default 180).
- The other tags (`--tags`, comma-separated) get one in rounds of `--tag-interval` s (default 600), alternating "burst" (all at the same moment, so their transfers overlap) and "staggered" (spread over the round). No tag gets one more often than its interval; the last upload time per tag is kept in `--state` (default `measurements/traffic-last-upload.json`) so this also holds across runs.
- No uploads in the AP's sleep window (`--quiet 1-7`).
- The sniffer runs the whole time (`<out>/night.pcap`, hourly summaries in `<out>/sniff.txt`); analyse the pcap later with `oepl_sniff.py --replay`.
- Images: header text, time, sequence number, and a noise band so each needs a few blocks (big displays capped at about 48 000 noise pixels). Sizes and colours come from `resources/tagtypes/<hwType>.json`.
- **`/imgupload` switches a tag to content mode 24 and replaces its content config.** The script saves every tag's config to `<out>/tag-configs.json` first and restores it at the end; if it was stopped early, restore with `--restore <out>/tag-configs.json`. Only use tags whose owner agreed.

```bash
python miscellaneous/radio-baseline/night_traffic.py --ap http://192.168.178.198 --test-tag 0000021EDAAE3B17 \
    --tags 000002BD0A923B1C,0000018938B83B11 --sniffer COM11 --ppk COM3 --hours 1 --out measurements/<date>/run
```
