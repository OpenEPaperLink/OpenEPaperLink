#include "pendingupload.h"

#include <esp_heap_caps.h>

#include <algorithm>
#include <vector>

#include "storage.h"
#include "tag_db.h"
#include "uploadinfo.h"

struct StoredUpload {
    uint8_t mac[8];
    String name;
    uint8_t *data;
    size_t len;
    size_t size;
    uint32_t storedAt;  // millis()
};

static std::vector<StoredUpload> storedUploads;
// images written to flash by spillPendingUpload(), until converted (the content runner deletes the file) or replaced
static std::vector<StoredUpload> spilledUploads;
static size_t allocatedBytes = 0;  // buffers of uploads being received and stored ones

std::recursive_mutex &pendingUploadMutex() {
    static std::recursive_mutex mutex;
    return mutex;
}

uint8_t *allocUploadBuffer(size_t size) {
    std::lock_guard<std::recursive_mutex> lock(pendingUploadMutex());
    const size_t budget = std::min((size_t)IMAGE_UPLOAD_BUDGET, (size_t)ESP.getPsramSize() / 2);
    if (allocatedBytes + size > budget) return nullptr;
    if (heap_caps_get_free_size(MALLOC_CAP_SPIRAM) < size + IMAGE_UPLOAD_PSRAM_RESERVE) return nullptr;
    uint8_t *buffer = static_cast<uint8_t *>(heap_caps_malloc(size, MALLOC_CAP_SPIRAM));
    if (buffer != nullptr) allocatedBytes += size;
    return buffer;
}

void freeUploadBuffer(uint8_t *buffer, size_t size) {
    if (buffer == nullptr) return;
    std::lock_guard<std::recursive_mutex> lock(pendingUploadMutex());
    heap_caps_free(buffer);
    allocatedBytes -= size;
}

void storePendingUpload(const uint8_t mac[8], const String &name, uint8_t *data, size_t len, size_t size) {
    std::lock_guard<std::recursive_mutex> lock(pendingUploadMutex());
    for (StoredUpload &stored : storedUploads) {
        if (memcmp(stored.mac, mac, 8) == 0) {
            freeUploadBuffer(stored.data, stored.size);
            stored.name = name;
            stored.data = data;
            stored.len = len;
            stored.size = size;
            stored.storedAt = millis();
            return;
        }
    }
    StoredUpload stored = {{0}, name, data, len, size, millis()};
    memcpy(stored.mac, mac, 8);
    storedUploads.push_back(stored);
}

PendingUpload takePendingUpload(const uint8_t mac[8], const String &name, uint8_t *&data, size_t &len, size_t &size) {
    std::lock_guard<std::recursive_mutex> lock(pendingUploadMutex());
    for (auto it = storedUploads.begin(); it != storedUploads.end(); ++it) {
        if (memcmp(it->mac, mac, 8) != 0) continue;
        if (it->name != name) return PendingUpload::Other;
        data = it->data;
        len = it->len;
        size = it->size;
        storedUploads.erase(it);
        return PendingUpload::Taken;
    }
    return PendingUpload::None;
}

/// @brief A stored or spilled image that is still named by the config of its tag
static bool isUsed(const StoredUpload &upload) {
    const tagRecord *taginfo = tagRecord::findByMAC(upload.mac);
    return taginfo != nullptr && taginfo->modeConfigJson.indexOf(upload.name) >= 0;
}

static std::vector<String> spilledNames() {
    std::lock_guard<std::recursive_mutex> lock(pendingUploadMutex());
    std::vector<String> names;
    for (const StoredUpload &spilled : spilledUploads) names.push_back(spilled.name);
    return names;
}

void dropStalePendingUploads() {
    std::vector<String> obsoleteFiles;
    {
        std::lock_guard<std::recursive_mutex> lock(pendingUploadMutex());
        for (auto it = storedUploads.begin(); it != storedUploads.end();) {
            if (!isUsed(*it)) {
                freeUploadBuffer(it->data, it->size);
                it = storedUploads.erase(it);
            } else {
                ++it;
            }
        }
        for (auto it = spilledUploads.begin(); it != spilledUploads.end();) {
            if (!isUsed(*it)) {
                obsoleteFiles.push_back(it->name);
                it = spilledUploads.erase(it);
            } else {
                ++it;
            }
        }
    }
    // file system work outside the lock (it can take a while, and async_tcp waits for the lock)
    for (const String &filename : obsoleteFiles) {
        xSemaphoreTake(fsMutex, portMAX_DELAY);
        contentFS->remove(filename);
        xSemaphoreGive(fsMutex);
    }
    std::vector<String> converted;  // the content runner deleted the file after converting it
    for (const String &filename : spilledNames()) {
        if (!contentFS->exists(filename)) converted.push_back(filename);
    }
    std::lock_guard<std::recursive_mutex> lock(pendingUploadMutex());
    spilledUploads.erase(std::remove_if(spilledUploads.begin(), spilledUploads.end(), [&converted](const StoredUpload &spilled) {
                             return std::find(converted.begin(), converted.end(), spilled.name) != converted.end();
                         }),
                         spilledUploads.end());
}

void spillPendingUpload() {
    StoredUpload spill;
    {
        std::lock_guard<std::recursive_mutex> lock(pendingUploadMutex());
        auto it = std::find_if(storedUploads.begin(), storedUploads.end(), [](const StoredUpload &stored) {
            return millis() - stored.storedAt > PENDING_UPLOAD_SPILL_MS;
        });
        if (it == storedUploads.end() || !Storage.reserveSpace(it->len, UPLOAD_FREE_SPACE_MARGIN)) return;
        // out of the table while it is written: a newer upload for the tag can't free it meanwhile
        spill = *it;
        storedUploads.erase(it);
    }

    // LockedFile: fsMutex only per block, writing a few hundred KB takes seconds
    bool ok;
    {
        LockedFile file(spill.name, "w");
        ok = (bool)file;
        for (size_t pos = 0; ok && pos < spill.len; pos += 4096) {
            const size_t chunk = std::min((size_t)4096, spill.len - pos);
            ok = file.write(spill.data + pos, chunk) == chunk;
        }
    }
    freeUploadBuffer(spill.data, spill.size);
    spill.data = nullptr;

    if (ok) {
        // dropStalePendingUploads() removes the file when a newer upload replaces it
        std::lock_guard<std::recursive_mutex> lock(pendingUploadMutex());
        spilledUploads.push_back(spill);
    } else {
        logLine("Failed to write " + spill.name);
        xSemaphoreTake(fsMutex, portMAX_DELAY);
        contentFS->remove(spill.name);
        xSemaphoreGive(fsMutex);
    }
}
