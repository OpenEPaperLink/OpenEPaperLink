# Radio chain – task list

Phased plan for the findings in [radio-chain-analysis.md](radio-chain-analysis.md). The analysis explains *why*; this file tracks *what* and *how far*. Nothing here is decided until it is started; phases can be re-ordered.

Status: `[ ]` open · `[~]` in progress · `[x]` done (with commit) · `[-]` dropped (with reason)

**Keeping this file current**

- Update this file in the same commit as the work it describes: tick the item and add the commit hash.
- `#n` is a stable task number; IDs like `P3`, `C1`, `E2` are findings in the analysis.
- New findings go under [Unplanned](#unplanned) first, and into the analysis when they are confirmed.
- Measurement results: see [Results](#results) for where final and intermediate results go; always with date, firmware versions and setup.

## Phase 0 – Baseline measurements

Measure how often the tag windows are missed today, before changing behaviour. The instrumentation must not affect timing: no live logging. Gate for phases 2 and 4.

- [x] **M2 – timing and loss on air** (#2). Sniffer `miscellaneous/radio-sniffer/` — `0594eb1b`; baseline 2026-10-12 in [radio-chain-results.md](radio-chain-results.md). Any ESP32-C6 devboard on USB as sniffer: small ESP-IDF (v5.5.1) firmware in promiscuous mode on the AP's channel, sends every frame with its SFD timestamp (µs), RSSI and LQI over serial; a Python script on the PC decodes the OEPL packet types, computes response times and gaps, and writes a pcap for Wireshark. Tags: ZBS243 and nRF52811 (available); no TLSR8258 tag available, so TLSR windows stay code-derived. Yields: missed windows, repeated requests, packet loss per burst.
- [~] **M3 – UART integrity.** Stage A done 2026-10-12 (1.1 h with generated traffic, console on COM12): 6 % of block hand-overs failed, all through the C6 being busy or slow (P2/P4, E1); no checksum errors. Results in [radio-chain-results.md](radio-chain-results.md). Stage B (counters in S3 and C6 firmware) still waits;
- [x] **M4 – baseline tag consumption.** Capture script `miscellaneous/ppk2-measure/` — `506a79a2`, baseline runner `miscellaneous/radio-baseline/`; baseline 2026-10-12 in [radio-chain-results.md](radio-chain-results.md). Nordic PPK2 in source-meter mode powers one ZBS243 tag (battery removed, ≈ 3.0 V), 100 kS/s, controlled from Python (`ppk2-api` 0.9.2). Sync with M2: the sniffer devboard toggles a GPIO on every frame to or from the tag, wired to PPK2 logic input D0, so current and radio frames share one timeline. Yields: sleep current, charge per check-in, per block and per complete image transfer, radio-on time per step (check-in, `pleaseWaitMs`, block data), display refresh shown separately. ZBS243 chosen because its `pleaseWaitMs` handling (busy-wait with radio on below 35 ms, sleep above) makes the effect of R3 impossible to predict from code; also the most common tag. Reference for R3/R5.

Done when: baseline numbers for M2–M4 are recorded under Results. M2 and M4 done; M3 (UART) stage B still open. M1 moved to phase 4 (2026-10-12): M2 showed on air everything phase 2 needed.

## Phase 1 – S3 fixes without radio protocol change

Fixes in the existing serial protocol. They stay necessary for TLSR, ZBS AP and old C6/H2 (migration step 1). Plus the UDP version number, so multi-AP structs can change safely later. Independent of the measurements.

- [x] #5 SCP size from `apInfo.type` instead of `HAS_SUBGHZ` (P3) — `81b90b4e`, `a5a495bd`: 4 bytes for C6/H2 ≥ 0x0019, 3 for all others; radio info is asked before the first SCP, also in the RES> handler. Tested on the Yellow AP (C6); not on a board without `HAS_SUBGHZ` or with a TLSR (none here)
- [x] #6 Limit TRD length and add a timeout to the S3 parser (S1, S3) — `52150c6a`; runs on the Yellow AP, the error paths were not triggered
- [x] #8 `VER>` buffer with terminating NUL (S2) — `d168ce0f`; version 0x001F read correctly on the Yellow AP
- [x] #23 Do not retry `NOQ>` in `sendDataAvail` (S5) — `405bc9b5`; NOQ> not triggered in the test
- [x] #25 Enlarge the `Serial1` RX buffer (T2) — `14d22365`: 4 KB. Event-driven reading dropped: 1 ms polling with ≈ 20 ms of buffer is enough
- [x] #31 Version number for the UDP protocol between APs (`adf89611`: `UDP_PROTO_VERSION` 1 as an extra byte in the AP list, peers recorded, `minPeerVersion()`). Must stay compatible with S3s that don't know it: in `udp.cpp` they ignore unknown packet types and cut longer payloads to the struct size. So: announce the version (new packet type, or an extra field in the AP list), keep sending the current packets unchanged, and only use changed structs towards peers that announced a newer version

Status 2026-10-12: flashed as `a5a495bd` on the Yellow AP; boots cleanly, radio online at 2 Mbaud, baseline rerun shows no regression ([results](radio-chain-results.md)). #31 still to be tested with a second AP. Done when: all S3 envs build; tested on the Yellow AP (S3 + C6), on a board without `HAS_SUBGHZ` for #5, and #31 in a multi-AP setup with one AP on the old firmware. Phase 1 completed 2026-10-19 (Nic: all tests done).

## Phase 2 – C6/H2 rework on the current protocol (interim release)

The C6/H2 firmware is restructured and fixed while it still speaks the old serial protocol, and released as an interim release. Works with every S3 version. The rework gets field-tested before the new protocol is added in phase 4, and the biggest timing gain (E1) reaches users early. Measure with M1/M2 before and after.

- [x] #13 Event-driven structure of the C6 firmware (R1) — `3688c43d` (main loop woken by radio, serial and timer; 1 ms ticks), `e652c295` (burst starts 5 ms after `pleaseWaitMs`, only with a complete, checksum-checked block). Measured: 3 block requests per 3-block image (was 5–6), burst +7 ms (was +71), tag charge per transfer −35 % ([results](radio-chain-results.md))
- [x] #3 Move the `memcpy` in `radioTx` after the wait loop (C5) — `52a65b66`; on air all 42 parts of a full block now arrive (was 41 + a duplicate). Confirmed by the baseline: part 0 of every full block is never sent, so each block needs a partial re-request; biggest single gain in this phase
- [x] #7 Logging to WARN and out of the ISRs (E2, R7) — `45cd3173`; traffic hour without serial errors
- [x] #10 Use the PAN of the incoming frame for cancel and XFC ack (C4) — `ea6e6fe0`; sub-GHz not testable here, to be tested by users after the release
- [x] Sub-GHz in the event loop (found after `2.94-nic`, C6/H2 0021): #12 judged sub-GHz requests by the age of the last 802.15.4 frame (`lastRxAgeUs` was only set on that path), so after a late 802.15.4 frame they were skipped; now 0 for sub-GHz. The CC1101 interrupt (GDO0) now wakes the main loop like an 802.15.4 frame (was: polled, up to 100 ms). Build-tested; 2.4 GHz checked with 12 min of traffic; bins in `binaries/ESP32-C6`, `binaries/ESP32-H2` for separate testing (the 4inch AP's `OpenEPaperLink_esp32_C6uart0.bin`, profile 4inchAP, rebuilt as 0021 too; was 001f in QIO mode; not tested, no 4inch AP here), in the next release by default
- [x] #12 Drop stale packets from the RX queue (E5, R6) — `85f56e13`: requests older than 5 ms (7 ms for block requests) are skipped and counted
- [x] #19 `radioSetChannel` without full re-initialisation and LED delay (E6) — `35d2f98f`; tested 2026-10-12: AP switched 25 → 26 → 25 through the web config, the C6 changed channel at once each time, the test tag (rebooted) found the AP on 26, the other tags checked in again right after the switch back
- [x] Build in DIO flash mode for ISSI flash (upstream issue #596; also in the local `TASKS.md`) — `00e9228f`; runs on the Yellow AP; the ISSI case waits for user feedback after the release
- [x] No debug text on the protocol UART (P7) — `1d3c0f5d`
- [x] S3: wait 500 ms for the radio's `ACK>` to `>D>` (P2/P4) — `acca3f99`; 0 failed block hand-overs in the traffic hour (was 11 of 177)
- [x] S3 C6 updater: don't call a complete download dropped (unplanned, found while flashing) — `74761447`

Status 2026-10-12: all of the above on the Yellow AP (C6 0x0020 and S3 `74761447`); one hour of traffic like M3 stage A: no serial errors, half the block frames on air, bursts +6 ms instead of +72 ms, check-ins answered on the first try ([results](radio-chain-results.md#phase-2-traffic-hour-2026-10-12-c6-74761447-s3-74761447)). Released as `2.94-nic` (C6/H2 0020, built with ESP-IDF 5.5.5 and checked with 25 min of traffic; H2 build-tested only, its `sdkconfig.defaults` got the same timing settings). Still open: #10 (sub-GHz) and the ISSI case, by users. Phase 2 completed 2026-10-19 (Nic: all tests done).

Done when: C6 firmware version bumped and released; M2 meets the response-time requirements of the timing budget and shows no regression against the baseline.

## Phase 3 – Foundations for the new protocol

S3 structure and protocol design, without changing behaviour (migration step 2). Can run in parallel with phase 2.

- [x] #28 Transport abstraction on the S3, with only the old implementation (`radiotransport.h`, `legacytransport.cpp`; AP-state checks stay in `serialap.cpp`) — `be5bf3ae`; all S3 envs build; on the Yellow AP the radio came online as before and 25 min of traffic showed no serial errors and the same timing as the 5.5.5 check
- [x] #30 Design the message set for the new UART protocol: check-in with AP-side RSSI/LQI/radio, data in chunks with offset, cancel by MAC, fixed SCP fields. Described in [radio-protocol-v2.md](radio-protocol-v2.md), structs with size checks in `oepl-radio-v2.h`

Done when: S3 behaviour unchanged (same test as phase 1); message set documented.

## Phase 4 – New protocol (C6/H2 + S3)

Migration step 3: S3 and C6/H2 firmware are released together. Old S3 + new C6/H2 is not supported.

On hold from 2026-10-13, picked up again on 2026-10-19 (Nic). Order: step 1 framing and messages without a cache (#14, #29, #26), step 2 block cache (#15), step 3 LED (#33). How it works: [radio-protocol-v2.md](radio-protocol-v2.md).

- [x] #14 Framing in chunks of ≤ 256 B with CRC16, sequence numbers and version handshake (R2, T1) — step 1, `c852665f`: `oepl-radio-v2.h` (structs, COBS, CRC16), C6 `v2link.c`, S3 `v2transport.cpp`. C6/H2 0023 (0022 is the last version with the old protocol). Tested 2026-10-19 on the Yellow AP: new S3 + C6 0021 stays on the old protocol (2026-10-20 also with 0022); new S3 + C6 v2 runs v2, check-ins of all tags, three concurrent image transfers each block requested once, XFC from all three in 45 s
- [x] #29 Negotiation at startup (`CAP>`, `FRM1`, HELLO, heartbeat); C6/H2 only ASCII recognition plus the new protocol — step 1, as #14. Heartbeat every second from APTask, 3 misses bring the link up again; the radio falls back to ASCII after 5 s without frames
- [x] #26 C6: one RX buffer, possibly UHCI DMA (T3, E7) — step 1: the main loop reads the UART driver's 4 KB buffer directly, the 8 KB ring and its copy task are gone. No DMA
- [x] #15 Block cache on the C6 (R3) — step 2, `c5efe2a4`: C6 `blockcache.c` (4 KB blocks, LRU, as much heap as is left minus 48 KB, at most 256 KB), S3 sends a pending item to the cache right after SET_PENDING. Tested 2026-10-19 on the Yellow AP: three concurrent image transfers, all blocks from the cache (no block request reached the S3), XFC from all three; with preloading switched off in a test build every block was asked from the S3 once, XFC from all three. Cache size on the radio not checked (only in its serial log)
- [~] #20 Less repetition in `sendBlockData`, based on M2 packet loss (E4, R5) — `939cd948` (moved forward into the phase 2 build): each requested part at most twice, never more than 42 frames; frames after the tag had its data 5 308 → 735 in the traffic hour. Tune with M2 packet loss
- [x] #33 Activity LED honours "LED off" on the radio (PR #587, jakobwesthoff) — step 3: SET_CONFIG `led_brightness` carries `config.led`; the C6/H2 drives its two activity LEDs with LEDC PWM at that brightness (roughly gamma-corrected), not at all at 0. A changed LED setting in the web UI goes to the radio at once (`setAPchannel()`). Tested 2026-10-19 on the Yellow AP: radio online, no errors; the LED itself not checked by eye. Close #587 with credit
- [ ] M1 – response times in the C6 (#1), moved here from phase 0: build it when the new protocol and the block cache need a look inside the C6. `esp_timer` timestamps in a RAM ring buffer, read out afterwards via a command. Yields: RX → TX per packet type, queue wait time, burst start vs `pleaseWaitMs`, duration of serial handling. Behaviour-neutral C6 build.

Done when: tested with new S3 + old C6, new C6, TLSR and ZBS AP, and old S3 + old radio; multi-AP with mixed radios; timing budget met in M1/M2; M4 compared to the baseline.

## Phase 5 – TLSR AP

Migration step 4. Blocked until the source is available or new firmware is written.

- [ ] #17 Ask for the TLSR AP source (atc1441, OEPL maintainers)
- [ ] #18 TLSR AP: higher baud rate and prefetch of the next block, then the new protocol (R8)

## Independent

Can be done at any time; not tied to a phase.

- [ ] #16 nRF tag: fix the ping loop (`HAL_msDelay` out of the window) (G1) — Shared_Tag_CPP_Libraries
- [ ] #21 ZBS and TLSR tags: catch `pleaseWaitMs` underflow (G3, G4) — Tag_FW_ZBS243, OpenEPaperLink_TLSR
- [ ] #22 TLSR tag: stop receiving when the block is complete (G5)
- [x] #27 Flash the C6 at a higher baud rate (T5) — 921600 baud (`FLASHER_BAUD`), one retry at 115200 if that fails. Yellow AP, C6 0022: connect to flash end 41 s → 12 s (app 326 KB: 34 s → 7 s). The fallback itself was not tested
- [x] #34 S3 part of PR #587 (jakobwesthoff): `quickBlink()` used full brightness under `HAS_TFT` even with the LED off; now off when `maxledbrightness` is 0 — together with #33
- [x] #35 Radio ignores unknown tags when the tag inventory is locked (issue #177): "Lock tag inventory" only worked on the S3; the C6 still answered every check-in, so two installs on the same channel interfere. Now protocol 2.1 (C6/H2 0024): the S3 keeps an allow-list with its tag database in the radio (ALLOW_CLEAR/ADD/REMOVE) and sends the lock mode in SET_CONFIG; locked, the radio does not answer other tags at all; learning, only their full check-in at first boot; no PONG for unknown tags. See the rules in [radio-protocol-v2.md](radio-protocol-v2.md). Tested on the Yellow AP: S3 with C6 0023 and 0024, lock on for 3.5 min with all 17 known radio tags answered; a tag deleted while locked (6 min) came back 5 min 18 s after unlocking, against its normal 1 min interval, so it got no answers and went looking for an AP (indirect: no sniffer attached). Not tested, and not needed (Nic, 2026-10-21): the missing replies on air (sniffer), learning mode with a booting tag, the H2 and Uart0 builds
- [x] #36 One radio reader at a time (issue #586, analysis by Ei-Qt): `bringAPOnline()` reopened the UART and started a second `rxSerialTask` while one ran (ping recovery, modem-stuck reset), which corrupted the heap and produced ghost tags. Now `startApReader()`/`stopApReader()` with a task handle, used by every place that takes the port; the modem-stuck reset is done by `APTask` on a flag from the debug monitor. Tested on the Yellow AP: boot, self-test 3×, C6 update; the recovery paths themselves were not triggered
- [x] #37 Self-test gives a wrong cause when the S3's native USB (GPIO19/20) carries the debug UART and a USB cable is plugged into that port (issue #586): a USB charger connects D+ and D−, so debug TX and RX are shorted. The test now checks each TX/RX pair for a connection first (radio in reset) and says so, with a hint about the charger; a PC port does not disturb the lines. See [radio-selftest.md](radio-selftest.md)
- [ ] #24 Archive or synchronise the separate repo `AP_Radio_FW_ESP32` (C9)
- [x] #32 S3 ↔ radio wiring self-test (nice to have): when the radio does not come online (or from a button in the web UI), the S3 checks each line and reports what fails, e.g. "TX and RX swapped" or "no signal on GPIO n". S3-side only; uses what the C6 already does (ROM boot text on UART0 after a reset, `RES>`/`RDY>` on the protocol UART, `ACK>` to `RDY?`). Design: [radio-selftest.md](radio-selftest.md); testable on a correctly wired AP by simulating faults with swapped or unused pin numbers — implemented: `radioselftest.cpp`, button *Test radio wiring* on the update page, `POST`/`GET /radio_selftest`, runs automatically when the radio does not come online at boot; tested on the Yellow AP with the real wiring and 10 simulated faults (all correct, see the design doc); the button tested by Nic. Not covered yet: PROG on shared-UART boards, automatic correction of swapped pins

## Dropped

- [-] #11 Actually call `esp_ieee802154_set_txpower` (C6) — there is no power setting: the S3 always sends 10, the C6 transmits at the driver default of 20 dBm (maximum); applying the 10 would only reduce range
- [-] T4 Baud rate to 4–5 Mbaud — 2 Mbaud only: some APs are hand-wired with wires of unknown length (Nic, 2026-10-12)
- [-] #4 Validity flag and checksum check for the block buffer (C1) — replaced by the block cache with confirmation after the checksum check (#15, #30)
- [-] #9 Do not pass channel 0 to the 802.15.4 radio (C3) — replaced by fixed SCP fields in the new protocol (#30)

## Unplanned

New findings that are not yet in a phase.

- [x] C6/H2 firmware prints `CC1101_QuickCheck … not detected` on the protocol UART at every boot without a CC1101 (`LOGE`/`LOGA` in `cc1101_radio.h` use `uart_printf` to UART1 on purpose). The S3 parser skips it; move these logs to UART0 (ESP_LOG) like the other debug output (P7). Now `printf` to the debug console (UART0); in the code, ships with the next radio version. Runs on the Yellow AP (radio online, tags answered); the UART0 output itself was not checked (no serial connection)

- [-] One block request answered only after 43 ms (0021, 2026-10-13 00:20:46, tag 00007E1CFE21B296): the C6 did not receive the tag's first five requests (no requests skipped as too old, the sniffer heard all six). Cause: the S3's WiFi transmissions desensitise the C6 receiver next to it; the web UI was open, so each check-in triggered a websocket push just when the tag asked for its block. Measured: check-ins needed 1.11 requests on average under WiFi load against 1.02 quiet ([results](radio-chain-results.md#wifi-desensitisation-2026-10-13)). Not fixed (Nic, 2026-10-13)

- [ ] 11.6" M3 tags (hwType 0x37, 2× rechargeable AA) check in only every 10–20 min instead of every minute, also with the old C6 firmware; a button press makes them check in every 40 s again, but the one at RSSI −68 fell silent again after 5 check-ins, the one moved close to the AP stayed regular. Next: sniffer close to the tag to see whether it sends and whether the AP hears it; check what the tag firmware does after a missed reply or at 2.4–2.5 V ([results](radio-chain-results.md#phase-2-traffic-hour-2026-10-12-c6-74761447-s3-74761447))

## Results

- Final results (baselines and before/after comparisons) go into [radio-chain-results.md](radio-chain-results.md), committed.
- Intermediate results stay local in `measurements/` at the repo root (excluded via `.git/info/exclude`, not committed). Latest: `run-2026-10-12/` (M3 stage A + concurrent transfers, raw data).
- Measurement tools are committed under `miscellaneous/`; each has a README with setup, usage, how to read the output and pitfalls.

## Tools

| Tool | Used for | Docs |
| --- | --- | --- |
| `miscellaneous/radio-sniffer/` | M2: on-air reply times, requests per exchange, block bursts, pcap; GPIO sync output for M4 | [README](../../miscellaneous/radio-sniffer/README.md) |
| `miscellaneous/ppk2-measure/` | M4: tag current with a PPK2 (100 kS/s), active episodes with charge and radio frames (D0) | [README](../../miscellaneous/ppk2-measure/README.md) |
| `miscellaneous/radio-baseline/` | Repeatable baseline (M2 + M4): scripted image transfers to one tag, analysis of boot, check-ins, XFC, blocks and transfers. Rerun after each change and compare. `night_traffic.py`: longer runs with traffic on several tags | [README](../../miscellaneous/radio-baseline/README.md) |
| `miscellaneous/ap-console-log/` | M3 stage A: long log of the AP's USB console, counting serial-protocol errors | [README](../../miscellaneous/ap-console-log/README.md) |
| `miscellaneous/ap-tool/` | Testing on an AP from the command line, mainly for agents: firmware and radio version, tags, tag backup/restore, S3 OTA, C6/H2 update, `/ws` messages | [README](../../miscellaneous/ap-tool/README.md) |
| `ARM_Tag_FW/build_radio.ps1` | Builds the C6, C6 Uart0 and H2 firmware with ESP-IDF 5.5.5; `-Publish` copies them to `binaries/` and sets the version from `main.c` in the firmware json files | Header of the script |
