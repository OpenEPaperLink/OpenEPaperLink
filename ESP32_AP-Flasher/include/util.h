#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

#include "system.h"
#include "web.h"

/// @brief Different utility functions
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
/// @return True on success, false on error (httpCode != 200 || deserialization error)
static bool httpGetJson(String &url, JsonDocument &json, const uint16_t timeout, JsonDocument *filter = nullptr) {
    HTTPClient http;
    // logLine("http httpGetJson " + url);
    http.begin(url);
    http.setTimeout(timeout);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    const int httpCode = http.GET();
    if (httpCode != 200) {
        http.end();
        wsErr(String("[httpGetJson] http ") + url + " code " + httpCode);
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

/// @brief Get the time_t for midnight
/// @return time_t for midnight
inline time_t getMidnightTime() {
    struct tm time_info;
    getLocalTime(&time_info);
    time_info.tm_hour = time_info.tm_min = time_info.tm_sec = 0;
    time_info.tm_mday++;
    return mktime(&time_info);
}

/// @brief Timer for kind of scheduling things
class Timer {
   public:
    /// @brief Construct a timer
    /// @param interval Interval in ms at which @ref doRun() returns true
    /// @param delay Delay in ms until first execution to defer start
    Timer(const unsigned long interval, const unsigned long delay = 0) : m_interval(interval), m_nextMillis(millis() + delay) {}

    /// @brief Change the interval
    /// @param interval New interval in ms
    void setInterval(const unsigned long interval) {
        m_interval = interval;
    }

    /// @brief Check if interval is met
    /// @param currentMillis Optionally provide the current time in millis
    /// @return True if interval is met, false if not
    bool doRun(const unsigned long currentMillis = millis()) {
        if (currentMillis >= m_nextMillis) {
            m_nextMillis = currentMillis + m_interval;
            return true;
        }
        return false;
    }

   private:
    /// @brief Timer interval in ms
    unsigned long m_interval;
    /// @brief Next timeer interval in ms
    unsigned long m_nextMillis;
};

/// @brief Create a String from format
/// @param buffer Buffer to use for sprintf
/// @param format String format
/// @return String
template <size_t bufSize>
inline String formatString(char buffer[bufSize], const char *format, ...) {
    va_list args;

    va_start(args, format);
    const size_t size = vsnprintf(buffer, bufSize, format, args);
    va_end(args);

    return String(buffer, size);
}

}  // namespace util

/// @brief Converts seconds to milliseconds
#define seconds(s) s * 1000
/// @brief Converts minutes to milliseconds
#define minutes(m) seconds(m * 60)
/// @brief Converts hours to milliseconds
#define hours(m) minutes(m * 60)
