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

## M3 stage A and concurrent transfers, 2026-10-12 (before any change)

In one hour with 55 image uploads (8 tags), 6 % of the block hand-overs from the S3 to the C6 failed, every time because the C6 was busy sending a burst and answered `>D>` too late; after such a failure the C6 is left waiting for block data and swallows the next commands. The C6 handles serial data only when its 100 ms radio-queue wait runs out, which is also why every burst starts ≈ 70 ms late.

**Setup**

| Item | Value |
| --- | --- |
| AP | Yellow AP as in the baseline; console on its CH340 USB port (COM12), logged with [ap-console-log](../../miscellaneous/ap-console-log/) |
| Traffic | `night_traffic.py` (in [radio-baseline](../../miscellaneous/radio-baseline/README.md)): test tag every 3 min (20 uploads), 7 more tags (2.9" M2, 1.3"/1.6"/2.7"/11.6" M3) every 10 min in alternating burst (all at once) and staggered rounds (5 each); configs saved and restored |
| Time | 18:44–19:50, 1.1 h; sniffer on channel 25 the whole hour |

**Serial errors (console, 1.1 h)**

| Message | Count | Meaning |
| --- | --- | --- |
| block send failed / Failed sending block | 11 | no `ACK>` to `>D>` within 200 ms: block data not sent (of 177 blocks: 6 %) |
| SDA send failed / given up | 7 / 1 | `SDA>` retries; once all 5 tries failed (19:14:25, 5 s after a failed block hand-over at 19:14:20) |
| UART Timeout (C6) | 4 | the C6 waited for bytes that did not come |
| wait… tx busy | 24 | S3 transmit lock contention |
| checksum errors, receive buffer full, radio pings failed | 0 | |

**Timing inside the C6 (its own log lines, 177 blocks)**

| Metric | Value |
| --- | --- |
| block request to the S3 → `>D>` seen by the C6 | median 96 ms, p90 99 ms, max 476 ms |
| 4100 bytes of block data received (21 ms on the wire at 2 Mbaud) | 117 ms in 154 of 157 cases |

**On air (sniffer, 1 h)**

| Metric | Value |
| --- | --- |
| Block parts / airtime | 12 448 frames, 1.7 % of the hour |
| Full blocks | always 41 of 42 parts on air (part 0 missing), on every tag type (ZBS243 and nRF52811 M3 tags), followed by a partial request for exactly that one part and 40–41 redundant frames |
| Burst start after `pleaseWaitMs` | +54 to +86 ms |
| Check-ins | 821 exchanges, 808 answered within 5 ms, 1.13 requests on average (max 14); first reply median 2.42 ms, p95 5.7 ms, max 38 ms; 66 extra replies |
| Check-ins needing retries | 40 of 847; only 2 of them overlapped a block burst; a retry typically took ≈ 11 ms (AP reply ≈ 9 ms after the first request, too late for the 5 ms window) |
| CANCEL_XFER ("BUSY": second tag during a transfer) | 6 |

**What this shows**

- **The C6's main loop is woken only by radio packets** (`xQueueReceive` on the radio queue, 100 ms timeout); serial bytes wait until it runs out. That gives the ≈ 96 ms before `>D>` is seen and the 117 ms for 21 ms of data, so a block is in the C6 ≈ 210 ms after the request, while the tag was told 140 ms (E1; the +71 ms late start of the baseline).
- **P2/P4 in practice:** during a burst the C6 reads no serial data; the S3 gives up on `>D>` after 200 ms, the C6's late `ACK>` then puts it in block-data mode waiting for 4100 bytes that never come, and it takes later commands as data until its 1 s timeout. The failed SDA series at 19:14 fits this; not proven byte by byte.
- **C5 hits all tag types**, not only the ZBS243.
- **Concurrency is not the main cost** at this load: 1.7 % airtime, 6 cancels, 2 retried check-ins during bursts. Most check-in retries come from the C6 answering ≈ 9 ms late after an idle period (`vTaskDelay(10)` after an empty queue wait: E1).
- All 8 tag configs were restored exactly (checked against the saved copies).
