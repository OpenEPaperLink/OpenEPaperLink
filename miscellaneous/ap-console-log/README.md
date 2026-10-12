# AP console log

Logs the AP's USB console (the ESP32-S3's serial port) for hours and counts the error messages of the S3 ↔ radio serial protocol. It is measurement **M3, stage A** in [docs/design/radio-chain-tasks.md](../../docs/design/radio-chain-tasks.md): no firmware change needed, because the S3 already prints those errors to its console (not to `log.txt`) and echoes the radio's debug UART there too.

```bash
python miscellaneous/ap-console-log/ap_console_log.py --port COM12 --hours 1.1 --summary-every 30 --out measurements/<date>/console
```

- Output: `<out>.log` (every line with a time stamp) and `<out>-summary.txt` (counts with first and last occurrence); intermediate summaries every `--summary-every` minutes on stdout.
- Opening an ESP32 port normally resets it through DTR/RTS. The script clears both before opening, so the AP keeps running (checked on the Yellow AP's CH340 port). Plugging the USB cable in can still power-cycle the AP.
- If the port disappears (the AP's nightly reboot), the script reopens it and counts that.
- Counted messages and what they mean are in the `PATTERNS` table at the top of the script: S3 retries and failures for `>D>`, `SDA>`, `CXD>`, `SCP>`, checksum errors on `RQB>`, transmit lock contention, radio pings failed; from the radio: `UART Timeout`, `receive buffer full`.
- The radio's debug lines (`I (…) RADIO: RX …`, `MAIN: Starting BlkData, N ms after request`, `Blockdata fully received in N ms`) are in the log too; they contain the C6's own timing. Strip the ANSI colour codes before grepping (`sed 's/\x1b\[[0-9;]*m//g'`).
- The S3 stops echoing the radio's debug UART above 6000 characters/s ("Serial monitor stopped because of flooding"); the summary counts that.
