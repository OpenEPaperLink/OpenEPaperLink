#pragma once

#include <stdint.h>
#include <time.h>
#include <string>

/// Result from computeSchedule().
struct ScheduleResult {
    int32_t intervalSeconds;  // seconds until next update (>= 0)
    std::string error;        // non-empty means invalid input; safe fallback applied
};

/// Compute seconds until the next scheduled update.
///
/// Accepts two syntaxes:
///   Legacy numeric:  integer minutes (e.g. "60", "-60", "-1440")
///   Cron expression: 5-field standard cron "m h dom mon dow"
///                    (e.g. "*/15 * * * *", "0 6 * * 1-5")
///
/// @param intervalStr  raw value from the "interval" config field
/// @param now          current epoch time (time_t)
ScheduleResult computeSchedule(const char* intervalStr, time_t now);

#ifdef ARDUINO
#include <Arduino.h>
/// Arduino String convenience overload.
inline ScheduleResult computeSchedule(const String& intervalStr, time_t now) {
    return computeSchedule(intervalStr.c_str(), now);
}
#endif
