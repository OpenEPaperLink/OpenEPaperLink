# S3 ↔ radio wiring self-test – design

*Design 2026-10-12, implemented 2026-10-13 in `ESP32_AP-Flasher/src/radioselftest.cpp`. Task #32 in [radio-chain-tasks.md](radio-chain-tasks.md). See* Implementation *at the end for what differs from this design.*

When the radio does not come online, the AP now says "I wasn't able to connect to the AP radio" and lists the pins it expects. Users with self-built APs (an S3 and a C6 board connected by wires) then have to find out themselves which wire is wrong. The self-test lets the S3 check every line between the two chips and report what fails, for example *"TX and RX of the protocol UART are swapped"* or *"no signal on GPIO 14 (radio TX → S3 RX): wire not connected"*. It runs on the S3 only and uses what the radio already does; the radio firmware does not change.

## Scope

| Boards | Lines tested |
| --- | --- |
| 6-line ESP32-C6/H2 boards (`C6_OTA_FLASHING`, separate debug UART): Yellow, Nano C6, Big C6, Mini v4, Simple, Elecrow 2.8, PoE-ISO | all 6: protocol TX/RX, debug TX/RX, RESET, PROG |
| 4-line boards with one shared UART (`FLASHER_DEBUG_SHARED`): LILYGO AP, 4inch AP | TX/RX, RESET, PROG |
| TLSR (SWS), ZBS243 and other radios flashed over SPI | not covered (the existing TX–RX short check for "no radio" stays) |

Pin names below are from the S3's side. The S3 reads the protocol pins from `FLASHER_AP_TXD/RXD` and the debug, RESET and PROG pins from the runtime config (`config.flashPinDbgTxd`, `flashPinDbgRxd`, `flashPinReset`, `flashPinProg`; named from the radio's side, so `flashPinDbgTxd` is the radio's debug TX = the S3's debug RX).

## What the radio does that the test can use

| Radio behaviour | Line it appears on | When |
| --- | --- | --- |
| ROM bootloader prints `ESP-ROM:esp32c6…` (or `esp32h2`) and the boot mode at 115200 baud | radio UART0 TX → S3 debug RX | ≈ 0–50 ms after RESET is released |
| With GPIO9 (PROG) low during reset, the ROM prints `waiting for download` and answers the serial bootloader's SYNC | radio UART0 TX, answers SYNC sent on its UART0 RX | download mode |
| The OEPL firmware prints `RES>` `RDY>` | radio UART1 TX (protocol) → S3 protocol RX | ≈ 0.5–1 s after boot (radio init) |
| The OEPL firmware answers `RDY?` with `ACK>` | needs S3 protocol TX → radio RX and the reverse | any time after boot |
| An idle UART TX pin is driven high | every radio TX line | while the firmware runs |

On shared-UART boards the ROM text and the protocol share one line pair.

## Checks

The test stops the normal serial tasks (as flashing does), runs these steps and puts every S3 pin back to its normal function at the end. A step that cannot be judged because an earlier one failed is reported as "not tested".

1. **Is each radio TX line driven?** With the radio running, set each S3 receive pin (protocol RX, debug RX) to input with pull-down, then with pull-up. A line that follows the pull is not driven: the wire is missing, or the radio is not running. A line that stays high is driven by a radio TX (good). This does not depend on how the radio's pins behave during reset.
2. **Where does each output appear?** Set all four S3 UART pins (both TX and both RX) to input and count signal edges on each while pulsing RESET (step 3 times the edges). Edges in the first 100 ms after reset are the ROM text (radio UART0 TX); edges after ≈ 300 ms are `RES>`/`RDY>` (radio protocol TX). Listening on the S3's own TX pins as well catches TX/RX swaps without ever driving two outputs against each other.
3. **Read what arrives.** Attach a UART receiver to each pin that showed edges and reset once more, to confirm the text (`ESP-ROM:`, `RES>`). Only text confirms a line; edges alone could be noise.
4. **RESET.** If no pin shows ROM text after a reset pulse, but `RDY?` still gets `ACK>` (the firmware kept running), the RESET wire does not reach the radio.
5. **PROG and debug TX.** Use the serial bootloader connection the C6 flasher already has (`loader_port_esp32_init` + `esp_loader_connect`): it pulls PROG low, resets, and sends SYNC on the S3's debug TX. Success proves RESET, PROG, debug TX and debug RX at once. If the ROM text says normal boot instead of `waiting for download`, PROG does not reach GPIO9. If the ROM is in download mode but SYNC gets no answer, the S3 debug TX → radio UART0 RX wire is the problem.
6. **Protocol TX.** Send `RDY?` on the S3 protocol TX; `ACK>` on the protocol RX proves both directions. If nothing comes back, send `RDY?` on the other S3 output (debug TX); an `ACK>` then means the two S3 TX wires are swapped between the radio's UARTs.
7. Reset the radio normally and continue with `bringAPOnline()`.

## Conclusions and messages

| Observation | Message (web UI and log) |
| --- | --- |
| everything as expected | "Radio wiring OK" |
| ROM text on the protocol RX pin, `RES>` on the debug RX pin | "Protocol and debug RX wires are swapped (GPIO a ↔ GPIO b)" |
| radio output on an S3 TX pin, nothing on the matching RX pin | "TX and RX of the protocol UART are swapped (GPIO a ↔ GPIO b)" (same for debug) |
| an RX line follows the pulls while the radio runs, and no output is found on any pin | "No signal on GPIO n (radio TX → S3 RX): wire not connected or radio not powered" |
| no ROM text after reset, firmware keeps answering | "The radio does not restart: RESET wire (GPIO n) not connected" |
| normal boot instead of download mode | "PROG wire (GPIO n → radio GPIO9) not connected" |
| download mode, no SYNC answer | "Debug TX wire (GPIO n → radio RX) not connected" |
| `RES>` seen, `RDY?` unanswered on both TX pins | "S3 TX → radio RX wire (GPIO n) not connected" |
| nothing on any line, RESET has no effect | "No response at all: is the radio powered and flashed?" |

## Automatic correction (optional)

If the only problem is a swapped pair, the S3 can use its pins the other way round: the ESP32 GPIO matrix lets any pin be TX or RX. The test would then offer *"Use the pins swapped?"*; on yes it stores the swapped numbers in the config. For the debug, RESET and PROG pins the runtime config exists already. For the protocol UART, `bringAPOnline()` has to read the pins from the config instead of `FLASHER_AP_TXD/RXD` (a small change). Input-only pins (GPIO 34–39 on the classic ESP32, e.g. `FLASHER_AP_RXD=36` on PoE-ISO) cannot become TX, so no correction there.

## When it runs

- Automatically when `bringAPOnline()` fails, before the AP is marked FAILED or offers to flash the radio; the result goes to the log and the web UI.
- On request: a "Test radio connection" button in the web UI (radio settings), with a warning that the radio is offline for a few seconds.
- It takes about 3–5 s (three resets plus the radio's start-up).

## Testing without touching wires

From the test's point of view, swapping two pin numbers in its input is the same as swapping the two wires, and giving it a GPIO that is not connected is the same as a wire without contact. The implementation takes the pin set as a parameter, so it can be tested on a correctly wired AP (Nic's Yellow AP) by:

| Simulated fault | How |
| --- | --- |
| protocol TX/RX swapped | run the test with `FLASHER_AP_TXD` and `FLASHER_AP_RXD` exchanged |
| protocol and debug RX swapped | exchange protocol RX and debug RX |
| wire not connected | replace one pin by a free GPIO |
| RESET or PROG not connected | replace that pin by a free GPIO |

Every simulated fault must produce the matching message, and the real wiring must produce "Radio wiring OK" with no false alarms. A test endpoint that takes a pin set does this from the web UI or a script; it is for development only and not shown to users. The real check on a hand-wired AP comes from users who have one.

## Open points

- The pull test (step 1) assumes no external pull resistors on the RX lines; a board with strong pull-ups would read "driven". To be checked against the board schematics, or the step is used only as a hint.
- The ESP32-H2 ROM banner and timing to be checked (the test looks for `ESP-ROM:` only).
- If a user's eFuses disable ROM messages, steps 3–5 lose their evidence; the test then reports "ROM output disabled, partial result".

## Implementation (2026-10-13)

`radioselftest.cpp` / `radioselftest.h`, only in `C6_OTA_FLASHING` builds.

- **Runs** automatically when the radio does not come online at boot (before the AP is marked FAILED; if the radio comes online after the test, the AP carries on), and from the button *Test radio wiring* under *Other actions* on the update page. `POST /radio_selftest` starts it, `GET /radio_selftest` returns the last result as JSON (`ok`, `messages`, `observations` with the pull results, what was heard on each pin, boot text, bootloader and `ACK>` results). Progress and findings go to the web console, the serial console and `log.txt`. It takes ≈ 8 s, after which the radio is brought online again.
- **Steps as designed**, with two UART receivers (Serial1 and Serial2) listening on two pins per reset: first the S3's RX pins, then its TX pins. PROG and debug TX are checked with an own SLIP-framed SYNC command to the ROM, listening on the pin where the ROM text appeared; `esp_loader` is not used because the boot text must be read in the same session. The protocol TX check sends `RDY?` on the S3 output that is not the pin the firmware output was found on, so a TX/RX swap is also confirmed in the other direction.
- **Development parameters**: `protoTx`, `protoRx`, `dbgTx`, `dbgRx`, `reset`, `prog` (S3-side GPIOs) replace the configured pins; the value **-2 stands for a wire that is not connected**: the test never drives it and hears nothing on it. This simulates missing wires without driving a spare GPIO that may be wired to something on the board.
- **Not yet covered**: PROG on shared-UART boards (LILYGO, 4inch; the bootloader step needs separate debug pins); the automatic correction of swapped pins; the ESP32-H2 banner (the test looks for `ESP-ROM:` only, which the H2 ROM also prints, unverified).

### Tested on the Yellow AP (C6 0x0021)

| Pins given to the test | Message |
| --- | --- |
| real wiring (twice, before and after the series) | Radio wiring OK |
| `protoTx=18 protoRx=17` | TX and RX of the protocol UART are swapped (GPIO 18 <-> GPIO 17) |
| `protoRx=19 dbgRx=18` | Protocol and debug RX wires are swapped (GPIO 19 <-> GPIO 18) |
| `dbgTx=19 dbgRx=20` | TX and RX of the debug UART are swapped (GPIO 19 <-> GPIO 20) |
| `protoRx=-2` | No signal on … (radio protocol TX -> S3 RX): wire not connected |
| `dbgRx=-2` | No signal on … (radio debug TX -> S3 RX): wire not connected |
| `reset=-2` | The radio does not restart: RESET wire … not connected |
| `prog=-2` | PROG wire … not connected: the radio boots normally with PROG low |
| `dbgTx=-2` | Debug TX wire … not connected: the bootloader does not answer |
| `protoTx=-2` | S3 TX -> radio RX wire … not connected: RDY? gets no answer |
| `protoTx=20 dbgTx=17` | The S3's two TX wires are swapped between the radio's UARTs (GPIO 20 <-> GPIO 17) |

Every simulated fault gave exactly its message, the real wiring no false alarm, and the radio came online after every run. With the real wiring the pull test read both RX lines as driven high, so the Yellow AP has no external pulls that fool it.

Seen on the way: at boot the C6 firmware prints `CC1101_QuickCheck … CC1101 not detected` on the **protocol** UART (`LOGE`/`LOGA` in `cc1101_radio.h` write to UART1 on purpose, "log to all"). The S3's parser skips it; it is noted as an open item.
