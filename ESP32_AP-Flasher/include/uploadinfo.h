#pragma once

#include <Arduino.h>
#include <FS.h>

#include "storage.h"

#define UPLOAD_BUFFER_SIZE 32768

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

/// @brief Append the buffered data to the file and empty the buffer. Sets error on a failed write.
inline void flushUploadInfo(UploadInfo *uploadInfo) {
    if (uploadInfo->bufferSize == 0) return;
    xSemaphoreTake(fsMutex, portMAX_DELAY);
    File file = contentFS->open(uploadInfo->filename, "a");
    if (file) {
        if (file.write(uploadInfo->buffer, uploadInfo->bufferSize) != uploadInfo->bufferSize) uploadInfo->error = true;
        file.close();
    } else {
        uploadInfo->error = true;
    }
    xSemaphoreGive(fsMutex);
    uploadInfo->bufferSize = 0;
}

/// @brief Add received data to the upload buffer, writing it to the file when the buffer is full
inline void appendUploadInfo(UploadInfo *uploadInfo, const uint8_t *data, size_t len) {
    if (uploadInfo->bufferSize + len > UPLOAD_BUFFER_SIZE) flushUploadInfo(uploadInfo);
    if (len > UPLOAD_BUFFER_SIZE) {
        // chunk larger than the buffer: write it directly
        xSemaphoreTake(fsMutex, portMAX_DELAY);
        File file = contentFS->open(uploadInfo->filename, "a");
        if (!file || file.write(data, len) != len) uploadInfo->error = true;
        if (file) file.close();
        xSemaphoreGive(fsMutex);
        return;
    }
    memcpy(&uploadInfo->buffer[uploadInfo->bufferSize], data, len);
    uploadInfo->bufferSize += len;
}
