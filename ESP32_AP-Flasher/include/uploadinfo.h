#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>

#include "storage.h"
#include "system.h"

#define UPLOAD_BUFFER_SIZE 32768
// free space that has to remain after an upload, for the images made from it and other uploads at the same time
#define UPLOAD_FREE_SPACE_MARGIN 262144

/// @brief State of a buffered file upload, stored in AsyncWebServerRequest::_tempObject
/// @note Allocated with malloc(), because the web server releases _tempObject with free() when a request is aborted
struct UploadInfo {
    char filename[96];
    bool error;
    size_t bufferSize;
    uint8_t buffer[UPLOAD_BUFFER_SIZE];
};

/// @brief Create an upload buffer for the given file
/// @return nullptr if the filename is too long or there is not enough memory
inline UploadInfo *newUploadInfo(const String &filename) {
    if (filename.length() >= sizeof(UploadInfo::filename)) return nullptr;
    UploadInfo *uploadInfo = static_cast<UploadInfo *>(malloc(sizeof(UploadInfo)));
    if (uploadInfo == nullptr) return nullptr;
    strlcpy(uploadInfo->filename, filename.c_str(), sizeof(uploadInfo->filename));
    uploadInfo->error = false;
    uploadInfo->bufferSize = 0;
    return uploadInfo;
}

/// @brief Start a buffered upload to the given file: checks the free space, creates the file and stores the
/// upload state in request->_tempObject. The handler sets _tempObject to nullptr when the upload is complete.
/// @note LittleFS must never run full: littlefs 2.9.0 divides by zero in its "No more free space" error.
/// A client that disconnects before the upload is complete leaves a partial file: it is removed then.
/// @return nullptr after sending an error response
inline UploadInfo *beginUpload(AsyncWebServerRequest *request, const String &filename) {
    size_t existingSize = 0;  // overwriting a file frees its space
    if (contentFS->exists(filename)) {
        File file = contentFS->open(filename, "r");
        existingSize = file.size();
        file.close();
    }
    // Storage.freeSpace() walks the file system (seconds): reserve against its last result instead
    const size_t needed = request->contentLength() > existingSize ? request->contentLength() - existingSize : 0;
    if (!Storage.reserveSpace(needed, UPLOAD_FREE_SPACE_MARGIN)) {
        logLineRateLimited("uploadspace", "Upload refused, not enough free space for " + filename, 600);
        request->send(507, "text/plain", "Not enough free space");
        return nullptr;
    }

    UploadInfo *uploadInfo = newUploadInfo(filename);
    if (uploadInfo == nullptr) {
        request->send(500, "text/plain", "Out of memory or path too long");
        return nullptr;
    }
    xSemaphoreTake(fsMutex, portMAX_DELAY);
    File file = contentFS->open(filename, "w");
    file.close();
    xSemaphoreGive(fsMutex);
    request->_tempObject = (void *)uploadInfo;

    request->onDisconnect([request]() {
        const UploadInfo *uploadInfo = static_cast<const UploadInfo *>(request->_tempObject);
        if (uploadInfo == nullptr) return;  // complete
        xSemaphoreTake(fsMutex, portMAX_DELAY);
        contentFS->remove(uploadInfo->filename);
        xSemaphoreGive(fsMutex);
    });
    return uploadInfo;
}

/// @brief Append data to the upload file. Sets error on a failed write.
inline void writeUploadInfo(UploadInfo *uploadInfo, const uint8_t *data, size_t len) {
    xSemaphoreTake(fsMutex, portMAX_DELAY);
    File file = contentFS->open(uploadInfo->filename, "a");
    if (file) {
        // a 1-byte stdio buffer makes fwrite() hand all data to the file system in one write, instead of
        // keeping a tail in the buffer that only gets written (and seeked for) during close()
        file.setBufferSize(1);
        if (file.write(data, len) != len) uploadInfo->error = true;
        file.close();
    } else {
        uploadInfo->error = true;
    }
    xSemaphoreGive(fsMutex);
}

/// @brief Append the buffered data to the file and empty the buffer. Sets error on a failed write.
inline void flushUploadInfo(UploadInfo *uploadInfo) {
    if (uploadInfo->bufferSize == 0) return;
    writeUploadInfo(uploadInfo, uploadInfo->buffer, uploadInfo->bufferSize);
    uploadInfo->bufferSize = 0;
}

/// @brief Add received data to the upload buffer, writing it to the file when the buffer is full
inline void appendUploadInfo(UploadInfo *uploadInfo, const uint8_t *data, size_t len) {
    if (uploadInfo->bufferSize + len > UPLOAD_BUFFER_SIZE) flushUploadInfo(uploadInfo);
    if (len > UPLOAD_BUFFER_SIZE) {
        // chunk larger than the buffer: write it directly
        writeUploadInfo(uploadInfo, data, len);
        return;
    }
    memcpy(&uploadInfo->buffer[uploadInfo->bufferSize], data, len);
    uploadInfo->bufferSize += len;
}
