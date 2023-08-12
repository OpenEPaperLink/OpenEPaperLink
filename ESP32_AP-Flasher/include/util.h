#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

#include "web.h"

namespace util {

/// @brief Prints free heap, allocatbale heap and free stack
static void printHeap() {
    const uint32_t freeStack = uxTaskGetStackHighWaterMark(NULL);
    Serial.printf("Free heap: %d allocatable: %d stack: %d\n", ESP.getFreeHeap(), ESP.getMaxAllocHeap(), freeStack);
}

/// @brief Prints the maximum continuous heap space
static void printLargestFreeBlock() {
    Serial.println("Maximum Continuous Heap Space: " + String(heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT)));
}

/// @brief Do a GET request to the given url and fill the given json with the response
/// @param url Request URL
/// @param json Json document to fill
/// @param timeout Request timeout
/// @return True on success, false on error (httpCode != 200 || deserialization error)
static bool httpGetJson(String &url, JsonDocument &json, uint16_t timeout) {
    HTTPClient http;
    http.begin(url);
    http.setTimeout(timeout);
    const int httpCode = http.GET();
    if (httpCode != 200) {
        http.end();
        wsErr("http " + httpCode);
        return false;
    }

    DeserializationError error = deserializeJson(json, http.getString());
    http.end();
    if (error) {
        Serial.println(error.c_str());
        return false;
    }
    return true;
}

}  // namespace util
