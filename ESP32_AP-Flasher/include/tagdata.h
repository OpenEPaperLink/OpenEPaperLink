#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include <optional>
#include <unordered_map>
#include <vector>

#include "storage.h"
#include "system.h"
#include "web.h"

/// @brief Functions for custom tag data parser
namespace TagData {

/// @brief All available data types
enum class Type {
    /// @brief Signed integer type
    INT,
    /// @brief Unsigned integer type
    UINT,
    /// @brief Float type
    FLOAT,
    /// @brief String type
    STRING,

    /// @brief Not a type, just a helper to determine max type
    MAX,
};

/// @brief Field that can be parsed
struct Field {
    /// @brief Field name
    String name;
    /// @brief Field type
    Type type;
    /// @brief Field byte length
    uint8_t length;
    /// @brief Number of decimals numeric types
    uint8_t decimals;
    /// @brief Optional multiplication
    std::optional<double> mult;

    Field(const String& name, const Type type, const uint8_t length, uint8_t decimals = 0, std::optional<double> mult = std::nullopt)
        : name(name), type(type), length(length), decimals(decimals), mult(mult) {}
};

/// @brief Parser for parsing custom tag data
struct Parser {
    /// @brief Parser name
    String name;
    /// @brief Parsed fields
    std::vector<Field> fields = {};
};

/// @brief Maps parser id to parser
extern std::unordered_map<size_t, Parser> parsers;

/// @brief Load all parsers from the given json file
/// @param filename File name
extern void loadParsers(const String& filename);

/// @brief Parse the incoming custom message
/// @param src Source mac address
/// @param id Message identifier
/// @param data Payload
/// @param len Payload length
extern void parse(const uint8_t src[8], const size_t id, const uint8_t* data, const uint8_t len);

/// @brief Convert the given byte array to a integer of the given length in bytes
/// @param data Byte array
/// @param length Length of byte array
/// @return uint32_t
inline uint32_t bytesToInt(const uint8_t* data, const uint8_t length) {
    uint32_t value = 0;
    memcpy(&value, data, length > sizeof(uint32_t) ? sizeof(uint32_t) : length);
    return value;
}

/// @brief Convert the given byte array to a float
/// @param bytes Must be at least 4 bytes (sizeof(float)) long
/// @return float
inline float bytesToFloat(const uint8_t* bytes) {
    float value;
    memcpy(&value, bytes, sizeof(float));
    return value;
}

/// @brief Convert the given byte array to a double
/// @param bytes Must be at least 8 bytes (sizeof(double)) long
/// @return double
inline double bytesToDouble(const uint8_t* bytes) {
    double value;
    memcpy(&value, bytes, sizeof(double));
    return value;
}

/// @brief Convert the given byte array to a string
/// @param bytes Byte array representing a string
/// @param length Length of byte array
/// @return String
inline String bytesToString(const uint8_t* bytes, int length) {
    return String(bytes, length);
}
}  // namespace TagData