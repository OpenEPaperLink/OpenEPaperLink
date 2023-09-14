#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

#include "web.h"

namespace util {

/// @brief Can be used to wrap a stream and see what's going on
class DebugStream : public Stream {
   public:
    DebugStream(Stream &stream) : _stream(stream) {}

    int available() override {
        return _stream.available();
    }

    int read() override {
        int data = _stream.read();
        Serial.write(data);
        return data;
    }

    int peek() override {
        int data = _stream.peek();
        Serial.print("Peek: ");
        Serial.println(data);
        return data;
    }

    void flush() override {
        _stream.flush();
        Serial.println("Flush");
    }

    size_t write(uint8_t data) override {
        Serial.write(data);
        return _stream.write(data);
    }

    size_t write(const uint8_t *buffer, size_t size) override {
        for (size_t i = 0; i < size; i++) {
            Serial.print("Write: ");
            Serial.println(buffer[i]);
        }
        return _stream.write(buffer, size);
    }

   private:
    Stream &_stream;
};

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
/// @param redirects Redirects handling
/// @return True on success, false on error (httpCode != 200 || deserialization error)
static bool httpGetJson(String &url, JsonDocument &json, const uint16_t timeout, JsonDocument *filter = nullptr)  //, const followRedirects_t redirects = followRedirects_t::HTTPC_DISABLE_FOLLOW_REDIRECTS)
{
    HTTPClient http;
    http.begin(url);
    http.setTimeout(timeout);
    // http.setFollowRedirects(redirects);
    const int httpCode = http.GET();
    if (httpCode != 200) {
        http.end();
        wsErr(String("[httpGetJson] http code") + httpCode);
        return false;
    }

    DeserializationError error;
    if (filter) {
        error = deserializeJson(json, http.getString(), DeserializationOption::Filter(*filter));
    } else {
        error = deserializeJson(json, http.getString());
    }
    http.end();
    if (error) {
        Serial.printf("[httpGetJson] JSON: %s\n", error.c_str());
        return false;
    }
    return true;
}

/// @brief Check if the given string is empty or contains "null"
///
/// @param str String to check
/// @return True if empty or null, false if not
inline bool isEmptyOrNull(const String &str) {
    return str.isEmpty() || str == "null";
}

/// @brief checks if the current time is between sleeptime1 and sleeptime2
///
/// @param sleeptime1 Start of time block
/// @param sleeptime2 End of time block
/// @return True if within time block, false is outside time block
static bool isSleeping(int sleeptime1, int sleeptime2) {
    if (sleeptime1 == sleeptime2) return false;

    struct tm timeinfo;
    getLocalTime(&timeinfo);
    int currentHour = timeinfo.tm_hour;

    if (sleeptime1 < sleeptime2) {
        return currentHour >= sleeptime1 && currentHour < sleeptime2;
    } else {
        return currentHour >= sleeptime1 || currentHour < sleeptime2;
    }
}

class Timer {
   public:
    Timer(unsigned long interval) : interval_(interval), previousMillis_(0) {}

    void setInterval(unsigned long interval) {
        interval_ = interval;
    }

    bool doRun() {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis_ >= interval_) {
            previousMillis_ = currentMillis;
            return true;
        }
        return false;
    }

   private:
    unsigned long interval_;
    unsigned long previousMillis_;
};

/// @brief Create a String from format
/// @param format String format
/// @return String
inline String formatString(const char *format, ...) {
    va_list args;

    va_start(args, format);
    const size_t size = vsnprintf(NULL, 0, format, args) + 1;  // +1 for the null terminator
    va_end(args);

    char buffer[size];

    va_start(args, format);
    vsnprintf(buffer, size, format, args);
    va_end(args);

    return String(buffer);
}

/// @brief Create a String from format
/// @param buffer Buffer to use for sprintf
/// @param format String format
/// @return String
template <size_t bufSize>
inline String formatString(char buffer[bufSize], const char *format, ...) {
    va_list args;

    va_start(args, format);
    vsnprintf(buffer, bufSize, format, args);
    va_end(args);

    return String(buffer);
}

}  // namespace util
