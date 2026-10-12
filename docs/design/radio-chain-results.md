# Radio chain – measurement results

Final results of the radio chain measurements; the plan is in [radio-chain-tasks.md](radio-chain-tasks.md), the findings they refer to (C5, E1, E4, …) in [radio-chain-analysis.md](radio-chain-analysis.md). Intermediate runs stay local and are not listed here.

## Baseline 2026-10-12 (before any change)

The C6 sends block data about 70 ms after the moment it promised the tag, and every full block lacks part 0 because of a TX buffer overwrite (C5). Together they make a 3-block image cost 5–6 block requests and about 50 mC on a ZBS243 tag, of which roughly half is waiting and re-requesting.

**Setup**

| Item | Value |
| --- | --- |
| AP | Yellow AP (ESP32-S3 + ESP32-C6), S3 firmware `2.91-dev` (fork `main`), C6 firmware 0x001F, channel 25, ≈ 14 tags in range |
| Tag | 2.9" Solum ZBS243 `0000021EDAAE3B17` (hwType 01), powered by a PPK2 at 3.0 V |
| Tools | [radio-baseline](../../miscellaneous/radio-baseline/README.md) (`run_baseline.py`, `analyze_baseline.py`), sniffer v1 on an ESP32-C6 devboard, PPK2 at 100 kS/s, D0 sync (1068 edges for 1068 frames) |
| Run | 5 transfers of seeded noise images (seeds 1000–1004, 296×128, black/white/red), 100 s apart, 590 s capture |
| Reproduce | `python miscellaneous/radio-baseline/run_baseline.py --tag 0000021EDAAE3B17 --ap http://192.168.178.198 --sniffer COM11 --ppk COM3 --reps 5 --interval 100 --out measurements/<date>/run1` |

The AP sent every image as raw 2bpp (type 0x21), 9472 bytes = 3 blocks (42 + 42 + 13 parts).

**Image transfer (per transfer, n = 5)**

| Metric | Value |
| --- | --- |
| Block requests per transfer | 5–6 (3 needed) |
| Check-in announcing the image → XFC | 4.03–4.26 s, mean 4.1 s |
| Tag charge to XFC | 47.8–54.2 mC, mean 50.7 mC |
| Display refresh | 6.0 s, ≈ 19.7 mC |
| Tag charge including refresh | 67.5–73.9 mC, mean 70.4 mC |

**Per block request (n = 25)**

| Metric | Value |
| --- | --- |
| `pleaseWaitMs` given by the AP | 140 (new block), 30 (partial re-request) |
| Burst start after promised time | mean +71 ms, max +78 ms |
| Tag radio on before the first part (wasted) | mean 86 ms, 2.35 mC |
| Tag radio on per request | mean 219 ms; 317 ms (the full window) for every full block |
| Frames per burst | 35–42 (42 sent; sniffer misses a few) |
| Frames sent after the tag switched its radio off | mean 15.6; 27–38 for partial re-requests |
| Tag charge per request | mean 6.3 mC; full block ≈ 9.0 mC, partial 3.2–7.5 mC |

**Other exchanges**

| Exchange | Value |
| --- | --- |
| Boot (power-on to end of first check-in) | 19.3 s, 86 mC; 7 pings, 5 pongs |
| Check-in without new data (n = 8, all single request) | reply 2.42 ms (median), tag awake 13.8 ms, 177 µC (median) |
| Check-in that needed a retry (earlier run, n = 1) | 20.9 ms awake, 390 µC |
| XFC → ACK (n = 6) | 1.29 ms median, 10.5 ms max |
| Tag sleep current | 1.62 µA |
| AP side, all tags, 590 s | 1075 block parts, 232 AVAIL_DATA_INFO, 102 AP sequence gaps |

## What the baseline shows

- **C5 confirmed: every full block lacks part 0.** In a burst where the sniffer saw all 42 frames with consecutive sequence numbers, the content was parts 1, 2, …, 41, 41: each frame on air carries the next frame's content, because `radioTx` copies the next frame into the TX buffer before the radio has sent the previous one. Part 0 is never sent, the last part twice. The tag listens its full 317 ms window, then has to send a partial request; partial requests work because the AP repeats the requested parts. Cost per full block: one extra request round (≈ 3–7 mC and ≈ 0.5 s).
- **E1 confirmed: the burst starts ≈ 70 ms late.** The C6 main loop waits up to 100 ms in `xQueueReceive` while the tag sleeps and only then checks `blockStartTimer`, so a 140 ms wait becomes ≈ 210 ms, and a 30 ms wait ≈ 95 ms. The ZBS243 wakes 25 ms early with its radio on, so it listens ≈ 86 ms for nothing on every request: ≈ 2.35 mC, about 13 check-ins' worth.
- **E4: frames nobody listens to.** After the tag has switched its radio off, the AP keeps sending to fill 42 frames: ≈ 16 frames per request on average, up to 38 after a partial request (≈ 190 ms airtime that blocks other tags).
- Check-ins without new data are fast (2.4 ms) when the C6 is idle; slow replies with retries (up to 18–23 ms) showed up in earlier runs right after idle periods (E1 again).

Rough estimate, not measured: fixing C5 removes the partial re-requests (5–6 → 3 requests), and fixing E1 cuts the wasted listening from ≈ 86 to ≈ 25 ms; together that would bring a 3-block transfer from ≈ 51 mC to ≈ 30 mC on this tag. The next run after phase 2 will show the real number.
