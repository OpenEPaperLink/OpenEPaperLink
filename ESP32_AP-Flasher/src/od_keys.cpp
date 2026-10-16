#ifdef HAS_BLE_WRITER
#include "od_keys.h"

#include <Preferences.h>

static const char* OD_KEYS_NAMESPACE = "odkeys";

// NVS keys are at most 15 characters: use the 6-byte BLE address, not the 8-byte tag MAC
static void odKeyName(const uint8_t mac[8], char name[13]) {
    snprintf(name, 13, "%02X%02X%02X%02X%02X%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
}

bool odKeyGet(const uint8_t mac[8], uint8_t key[16]) {
    char name[13];
    odKeyName(mac, name);
    Preferences prefs;
    if (!prefs.begin(OD_KEYS_NAMESPACE, true)) return false;
    const bool found = prefs.getBytes(name, key, 16) == 16;
    prefs.end();
    return found;
}

bool odKeyHas(const uint8_t mac[8]) {
    uint8_t key[16];
    return odKeyGet(mac, key);
}

bool odKeySet(const uint8_t mac[8], const uint8_t key[16]) {
    char name[13];
    odKeyName(mac, name);
    Preferences prefs;
    if (!prefs.begin(OD_KEYS_NAMESPACE, false)) return false;
    const bool ok = prefs.putBytes(name, key, 16) == 16;
    prefs.end();
    return ok;
}

// A tag added without a key has a 1-byte entry instead of the 16-byte key
bool odKeyAdded(const uint8_t mac[8]) {
    char name[13];
    odKeyName(mac, name);
    Preferences prefs;
    if (!prefs.begin(OD_KEYS_NAMESPACE, true)) return false;
    const bool added = prefs.isKey(name);
    prefs.end();
    return added;
}

bool odKeyAddWithoutKey(const uint8_t mac[8]) {
    char name[13];
    odKeyName(mac, name);
    const uint8_t marker = 0;
    Preferences prefs;
    if (!prefs.begin(OD_KEYS_NAMESPACE, false)) return false;
    const bool ok = prefs.putBytes(name, &marker, 1) == 1;
    prefs.end();
    return ok;
}

bool odKeyClear(const uint8_t mac[8]) {
    char name[13];
    odKeyName(mac, name);
    Preferences prefs;
    if (!prefs.begin(OD_KEYS_NAMESPACE, false)) return false;
    if (prefs.isKey(name)) prefs.remove(name);
    prefs.end();
    return true;
}
#endif
