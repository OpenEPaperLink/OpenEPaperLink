# WiFiClientSecure (patched copy)

Copy of the WiFiClientSecure library from framework-arduinoespressif32 3.20017 (Arduino core 2.0.17).
PlatformIO uses this copy instead of the framework's, also for HTTPClient.

The only change is in `stop_ssl_socket()` (src/ssl_client.cpp): after the `memset` of the context,
`socket` is set back to -1. Without that, a second `stop()` (for example `http.end()` followed by the
HTTPClient destructor) calls `close(0)` and closes whatever file holds file descriptor 0, typically a
LittleFS file another task is reading or writing. That caused LittleFS asserts (`lfs_mlist_isopen`)
in async_tcp while it served a file. Arduino core 3.x no longer closes the socket through `close()`
in `stop()`, so this copy can go when the AP moves to core 3.x.
