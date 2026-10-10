#pragma once

#include <Arduino.h>
#include <FS.h>

/// @brief Read a gzip file and decompress it into memory
/// @param size Set to the decompressed size
/// @return Buffer allocated with malloc (free() it), or nullptr on an error
uint8_t *readGzipFile(fs::FS &fs, const String &path, size_t &size);
