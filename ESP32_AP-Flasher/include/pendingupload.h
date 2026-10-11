#pragma once

#include <Arduino.h>

#include <mutex>

// Images posted to /imgupload stay in PSRAM until the content runner converts them, so the web server
// (async_tcp) doesn't write them to flash: a LittleFS write takes about 1 s per 32 KB and stalls it.
// Per tag only the newest image is kept.

#define IMAGE_UPLOAD_MAX_SIZE (1024 * 1024)        // one upload (the whole request body)
#define IMAGE_UPLOAD_BUDGET (4 * 1024 * 1024)      // all uploads being received or waiting, at most half the PSRAM
#define IMAGE_UPLOAD_PSRAM_RESERVE (1024 * 1024)   // PSRAM that stays free, for the sprite of the conversion

/// @brief Allocate a buffer in PSRAM for an upload
/// @return nullptr when the uploads would exceed their budget or PSRAM runs low
uint8_t *allocUploadBuffer(size_t size);
void freeUploadBuffer(uint8_t *buffer, size_t size);

/// @brief Guards the stored uploads. Hold it to store an upload and set the tag config that names it at once.
std::recursive_mutex &pendingUploadMutex();

/// @brief Keep the image for a tag until it is converted, replacing (and freeing) an older one
/// @param size allocated size of data, which this takes over
void storePendingUpload(const uint8_t mac[8], const String &name, uint8_t *data, size_t len, size_t size);

enum class PendingUpload { None, Taken, Other };
/// @brief Take the stored image with this name for a tag. The caller frees it with freeUploadBuffer(data, size).
/// @return Taken, None (nothing stored for the tag), or Other (the tag has a different, newer image)
PendingUpload takePendingUpload(const uint8_t mac[8], const String &name, uint8_t *&data, size_t &len, size_t &size);

/// @brief Free stored images that won't be converted: the tag is gone or its config names another image
void dropStalePendingUploads();

#define PENDING_UPLOAD_SPILL_MS 60000
/// @brief Write one image that waited longer than PENDING_UPLOAD_SPILL_MS to the file its name gives, and free it.
/// The tag isn't due yet (a sleeping tag can take hours); the content runner converts the file when it is.
/// @note Call it from the loop task, never from async_tcp: it writes to flash.
void spillPendingUpload();
