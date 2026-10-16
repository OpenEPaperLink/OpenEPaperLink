#pragma once
#ifdef HAS_BLE_WRITER
#include <Arduino.h>

// Encryption keys of OpenDisplay BLE tags (16 bytes each), kept in NVS so they survive
// a reboot and a filesystem wipe. A tag without encryption needs no key.
bool odKeyGet(const uint8_t mac[8], uint8_t key[16]);
bool odKeyHas(const uint8_t mac[8]);
bool odKeySet(const uint8_t mac[8], const uint8_t key[16]);
// The user added the tag (with or without a key); the AP only connects to added tags
bool odKeyAdded(const uint8_t mac[8]);
bool odKeyAddWithoutKey(const uint8_t mac[8]);
// Removes the key, or the "added without key" mark
bool odKeyClear(const uint8_t mac[8]);
#endif
