/// @file tagdata.h
/// @author Moritz Wirger (contact@wirmo.de)
/// @brief Custom tag data parser and helpers
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

    Field(const String &name, const Type type, const uint8_t length, uint8_t decimals = 0, std::optional<double> mult = std::nullopt)
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
extern void loadParsers(const String &filename);

/// @brief Parse the incoming custom message
/// @param src Source mac address
/// @param id Message identifier
/// @param data Payload
/// @param len Payload length
extern void parse(const uint8_t src[8], const size_t id, const uint8_t *data, const uint8_t len);

/// @brief Convert the given byte array @ref data with given @ref length to an unsigned integer
///
/// Will also convert non standard integer sizes (e.g. 3, 5, 6, and 7 bytes)
/// @tparam T Unsigned integer type
/// @param data Byte array
/// @param length Length of byte array
/// @return Unsigned integer
template <typename T, std::enable_if_t<std::is_unsigned_v<T> && std::is_integral_v<T>, bool> = true>
inline T bytesTo(const uint8_t *data, const uint8_t length) {
    T value = 0;
    for (int i = 0; i < length; i++) {
        value |= (data[i] & 0xFF) << (8 * i);
    }
    return value;
}

/// @brief Convert the given byte array @ref data with given @ref length to a signed integer
///
/// Will also convert non standard integer sizes (e.g. 3, 5, 6, and 7 bytes)
/// @tparam T Signed integer type
/// @param data Byte array
/// @param length Length of byte array
/// @return Signed integer
template <typename T, std::enable_if_t<std::is_signed_v<T> && std::is_integral_v<T>, bool> = true>
inline T bytesTo(const uint8_t *data, const uint8_t length) {
    T value = 0;
    for (int i = 0; i < length; ++i) {
        value |= (data[i] & 0xFF) << (8 * i);
    }

    // If data is smaller than T and last byte is negative set all upper bytes negative
    if (length < sizeof(T) && (data[length - 1] & 0x80) != 0) {
        value |= ~((1 << (length * 8)) - 1);
    }
    return value;
}

/// @brief Convert the given byte array to a float/double
/// @param data Byte array, should be at least 4/8 bytes long
/// @param length Length of byte array
/// @return float/double
template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
inline T bytesTo(const uint8_t *data, const uint8_t length) {
    const size_t len = sizeof(T) < length ? sizeof(T) : length;
    T value;
    memcpy(&value, data, len);
    return value;
}

/// @brief Convert the given byte array to a string
/// @param data Byte array representing a string
/// @param length Length of byte array
/// @return String
template <typename T, std::enable_if_t<std::is_same_v<T, String>, bool> = true>
inline T bytesTo(const uint8_t *data, int length) {
    return T(data, length);
}

/// @brief Convert the given byte array to a string
/// @param data Byte array representing a string
/// @param length Length of byte array
/// @return std::string
template <typename T, std::enable_if_t<std::is_same_v<T, std::string>, bool> = true>
inline T bytesTo(const uint8_t *data, int length) {
    return T(data, data + length);
}
}  // namespace TagData