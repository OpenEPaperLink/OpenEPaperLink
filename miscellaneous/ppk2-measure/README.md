# PPK2 tag current measurement

Measures the current of one OEPL tag with a Nordic Power Profiler Kit II (PPK2), at 100 kS/s, together with the PPK2's logic inputs. It is measurement **M4** of the radio chain work; context and the timing budget are in [docs/design/radio-chain-analysis.md](../../docs/design/radio-chain-analysis.md), the plan in [docs/design/radio-chain-tasks.md](../../docs/design/radio-chain-tasks.md).

`ppk2_capture.py` powers the tag from the PPK2 (source-meter mode), captures for a given time, splits the trace into active episodes, and saves the raw trace. Combined with the [radio sniffer](../radio-sniffer/README.md), every radio frame to or from the tag shows up as an edge on logic input D0.

## Setup

| What | How |
| --- | --- |
| Software | Python 3, `pip install ppk2-api==0.9.2 numpy pyserial` |
| PPK2 port | The PPK2 has two serial interfaces. Use the one described as **"nRF Connect USB CDC ACM"** (USB interface MI_01; COM3 on Nic's PC). The other one (COM4) does not answer. The script finds the right one itself if `--port` is left out. |
| Tag | Battery removed. PPK2 **VOUT** to battery +, **GND** to battery −. Never power the tag from both. ZBS243 Solum tags: 3.0 V. |
| Sync with the sniffer | Sniffer devboard GPIO 2 → PPK2 logic **D0**, devboard GND → logic **GND**, devboard 3V3 → logic **VCC**. Without VCC the logic inputs read only zeros. |

Check the D0 wiring: start a capture, then send `T` to the sniffer's serial port (toggles GPIO 2 at 20 Hz for 2 s); the capture must show 40 D0 edges about 50 ms apart.

## Run

```bash
python miscellaneous/ppk2-measure/ppk2_capture.py --duration 300 --voltage 3000 --out measurements/run1
```

| Option | Meaning |
| --- | --- |
| `--port` | PPK2 measurement port; default auto-detect. |
| `--voltage` | Source voltage in mV (default 3000). |
| `--duration` | Seconds to capture (default 60). |
| `--out BASE` | Write `BASE.npz` (raw trace) and `BASE.json` (summary and episodes). |
| `--threshold-ua` | A sample is "active" above this current (default 200 µA; sleep is ≈ 1.6 µA on a ZBS243). |
| `--merge-ms` | Active stretches closer than this are one episode (default 5 ms). |
| `--power-off` | Switch the tag off after the capture. |

A full measurement with the sniffer: start the sniffer with `--sync-mac <tag MAC>` and `--pcap`, start this script, then trigger what you want to measure (for example a new image via `POST /imgupload` with fields `mac`, `dither`, `file`; the tag fetches it at its next check-in).

## Output

```
captured 90.1 s at 3000 mV (9012224 samples)
mean 958.0 µA, charge 86333 µC (23.98 µAh), idle median 1.62 µA, active 13571 ms, D0 edges 15
5 active episodes (> 200 µA, gaps < 5 ms merged):
   start s   duration ms   charge µC   mean mA   peak mA   D0 edges
     6.560        7548.4     67004.7      8.88     37.92         11
    19.299          17.1       244.0     14.30     43.68          2
```

- **episode**: a stretch where the tag is awake (current above the threshold, gaps shorter than `--merge-ms` merged). `start s` is seconds since the capture started.
- **charge**: integral of the current; µC at the source voltage. 1 µAh = 3600 µC.
- **D0 edges**: radio frames to or from the tag that the sniffer saw during the episode (the sniffer toggles D0 once per frame). 2 edges in a short episode = one check-in (request + reply); more edges = retries, pings or a block transfer.
- **idle median**: median of all samples below the threshold, i.e. the sleep current.

`BASE.npz` holds `current_uA` (float32, one value per 10 µs), `logic` (uint8, bit n = input Dn), `sample_rate` and `voltage_mV`. `BASE.json` holds the summary and the episode list.

Aligning with the sniffer: the k-th D0 edge is the k-th frame to/from the sync MAC in the sniffer's pcap. Frames the sniffer missed produce no edge in either.

## Pitfalls

- **Every capture starts with a tag reboot.** The PPK2 switches its output off when the serial port closes, and the script switches it on again. A ZBS243 2.9" then spends ≈ 6 s on a display refresh and ≈ 7.5 s scanning for the AP (≈ 24 µAh in total) before its first check-in. Measure long enough, or ignore the first ≈ 14 s.
- Decoding is done by `ppk2-api` per sample in Python (≈ 1.2 M samples/s on Nic's PC), in a background thread during the capture. Memory is ≈ 5 bytes per sample (≈ 300 MB for 10 minutes).
- Near-zero samples can be slightly negative (calibration offset); the idle median is still meaningful.
- The PPK2 measures what the tag draws from VOUT; the display's own boost converter is included.
