#include "pendingupload.h"

#include <esp_heap_caps.h>

#include <algorithm>
#include <vector>

#include "tag_db.h"

struct StoredUpload {
    uint8_t mac[8];
    String name;
    uint8_t *data;
    size_t len;
    size_t size;
};

static std::vector<StoredUpload> storedUploads;
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
            return;
        }
    }
    StoredUpload stored = {{0}, name, data, len, size};
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

void dropStalePendingUploads() {
    std::lock_guard<std::recursive_mutex> lock(pendingUploadMutex());
    for (auto it = storedUploads.begin(); it != storedUploads.end();) {
        const tagRecord *taginfo = tagRecord::findByMAC(it->mac);
        if (taginfo == nullptr || taginfo->modeConfigJson.indexOf(it->name) < 0) {
            freeUploadBuffer(it->data, it->size);
            it = storedUploads.erase(it);
        } else {
            ++it;
        }
    }
}
