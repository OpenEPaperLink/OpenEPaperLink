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

- [ ] **M1 – response times in the C6** (#1). `esp_timer` timestamps in a RAM ring buffer, read out afterwards via a command. Yields: RX → TX per packet type, queue wait time, burst start vs `pleaseWaitMs`, duration of serial handling. Behaviour-neutral C6 build.
- [~] **M2 – timing and loss on air** (#2). Sniffer built and working: `miscellaneous/radio-sniffer/` — `0594eb1b`; baseline with block transfers still to do. Any ESP32-C6 devboard on USB as sniffer: small ESP-IDF (v5.5.1) firmware in promiscuous mode on the AP's channel, sends every frame with its SFD timestamp (µs), RSSI and LQI over serial; a Python script on the PC decodes the OEPL packet types, computes response times and gaps, and writes a pcap for Wireshark. Tags: ZBS243 and nRF52811 (available); no TLSR8258 tag available, so TLSR windows stay code-derived. Yields: missed windows, repeated requests, packet loss per burst.
- [ ] **M3 – UART integrity.** Counters for checksum errors and timeouts on both sides. Yields: how often P1/P5 occur in practice.
- [ ] **M4 – baseline tag consumption.** Nordic PPK2 in source-meter mode powers one ZBS243 tag (battery removed, ≈ 3.0 V), 100 kS/s, controlled from Python (`ppk2-api` 0.9.2). Sync with M2: the sniffer devboard toggles a GPIO on every frame to or from the tag, wired to PPK2 logic input D0, so current and radio frames share one timeline. Yields: sleep current, charge per check-in, per block and per complete image transfer, radio-on time per step (check-in, `pleaseWaitMs`, block data), display refresh shown separately. ZBS243 chosen because its `pleaseWaitMs` handling (busy-wait with radio on below 35 ms, sleep above) makes the effect of R3 impossible to predict from code; also the most common tag. Reference for R3/R5.

Done when: baseline numbers for M1–M4 are recorded under Results.

## Phase 1 – S3 fixes without radio protocol change

Fixes in the existing serial protocol. They stay necessary for TLSR, ZBS AP and old C6/H2 (migration step 1). Plus the UDP version number, so multi-AP structs can change safely later. Independent of the measurements.

- [ ] #5 SCP size from `apInfo.type` instead of `HAS_SUBGHZ` (P3)
- [ ] #6 Limit TRD length and add a timeout to the S3 parser (S1, S3)
- [ ] #8 `VER>` buffer with terminating NUL (S2)
- [ ] #23 Do not retry `NOQ>` in `sendDataAvail` (S5)
- [ ] #25 Enlarge the `Serial1` RX buffer and read event-driven (T2)
- [ ] #31 Version number for the UDP protocol between APs. Must stay compatible with S3s that don't know it: in `udp.cpp` they ignore unknown packet types and cut longer payloads to the struct size. So: announce the version (new packet type, or an extra field in the AP list), keep sending the current packets unchanged, and only use changed structs towards peers that announced a newer version

Done when: all S3 envs build; tested on the Yellow AP (S3 + C6), on a board without `HAS_SUBGHZ` for #5, and #31 in a multi-AP setup with one AP on the old firmware.

## Phase 2 – C6/H2 rework on the current protocol (interim release)

The C6/H2 firmware is restructured and fixed while it still speaks the old serial protocol, and released as an interim release. Works with every S3 version. The rework gets field-tested before the new protocol is added in phase 4, and the biggest timing gain (E1) reaches users early. Measure with M1/M2 before and after.

- [ ] #13 Event-driven structure of the C6 firmware (R1)
- [ ] #3 Move the `memcpy` in `radioTx` after the wait loop (C5)
- [ ] #7 Logging to WARN and out of the ISRs (E2, R7)
- [ ] #10 Use the PAN of the incoming frame for cancel and XFC ack (C4)
- [ ] #11 Actually call `esp_ieee802154_set_txpower` (C6)
- [ ] #12 Drop stale packets from the RX queue (E5, R6)
- [ ] #19 `radioSetChannel` without full re-initialisation and LED delay (E6)
- [ ] Build in DIO flash mode for ISSI flash (upstream issue #596; also in the local `TASKS.md`): `CONFIG_ESPTOOLPY_FLASHMODE_DIO=y` in `sdkconfig.defaults`

Done when: C6 firmware version bumped and released; M1/M2 meet the response-time requirements of the timing budget and show no regression against the baseline.

## Phase 3 – Foundations for the new protocol

S3 structure and protocol design, without changing behaviour (migration step 2). Can run in parallel with phase 2.

- [ ] #28 Transport abstraction on the S3, with only the old implementation
- [ ] #30 Design the message set for the new UART protocol: check-in with AP-side RSSI/LQI/radio, data in chunks with offset, cancel by MAC, fixed SCP fields. Written down in `docs/design/` before implementation

Done when: S3 behaviour unchanged (same test as phase 1); message set documented.

## Phase 4 – New protocol (C6/H2 + S3)

Migration step 3: S3 and C6/H2 firmware are released together. Old S3 + new C6/H2 is not supported.

- [ ] #14 Framing in chunks of ≤ 256 B with CRC16, sequence numbers and version handshake (R2, T1)
- [ ] #29 Negotiation at startup (`CAP>`, `FRM1`, HELLO, heartbeat); C6/H2 only ASCII recognition plus the new protocol
- [ ] #26 C6: one RX buffer, possibly UHCI DMA (T3, E7)
- [ ] #15 Block cache on the C6 (R3)
- [ ] #20 Less repetition in `sendBlockData`, based on M2 packet loss (E4, R5)
- [ ] T4 Baud rate to 4–5 Mbaud, only if M3 allows it and the block cache needs it

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
- [ ] #27 Flash the C6 at a higher baud rate (T5)
- [ ] #24 Archive or synchronise the separate repo `AP_Radio_FW_ESP32` (C9)

## Dropped

- [-] #4 Validity flag and checksum check for the block buffer (C1) — replaced by the block cache with confirmation after the checksum check (#15, #30)
- [-] #9 Do not pass channel 0 to the 802.15.4 radio (C3) — replaced by fixed SCP fields in the new protocol (#30)

## Unplanned

New findings that are not yet in a phase.

*(none)*

## Results

- Final results (baselines and before/after comparisons) go into [radio-chain-results.md](radio-chain-results.md), committed.
- Intermediate results stay local in `measurements/` at the repo root (excluded via `.git/info/exclude`, not committed). Latest: `2026-10-12-m2-sniffer-check.md` (check-in reply median 2.4 ms, max 4.5 ms; no block transfers yet).
- Measurement tools are committed under `miscellaneous/`; each has a README with setup, usage, how to read the output and pitfalls.

## Tools

| Tool | Used for | Docs |
| --- | --- | --- |
| `miscellaneous/radio-sniffer/` | M2: on-air reply times, check-in attempts, block bursts, pcap; GPIO sync output for M4 | [README](../../miscellaneous/radio-sniffer/README.md) |
