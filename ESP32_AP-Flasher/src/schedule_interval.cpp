#include "schedule_interval.h"

#include <cctype>
#include <cstdlib>
#include <cstring>

// ─── Cron field representation ────────────────────────────────────────────────

struct CronField {
    bool bits[64];  // index space covers all field ranges (max 0-59)
    bool is_star;   // true only when the field was bare "*" (no step/range)
                    // used for the classic dom/dow OR interaction
};

struct CronExpr {
    CronField minute;   // 0-59
    CronField hour;     // 0-23
    CronField dom;      // 1-31  (day-of-month)
    CronField month;    // 1-12
    CronField dow;      // 0-6   (day-of-week, 0=Sunday)
};

// ─── Parse helpers ────────────────────────────────────────────────────────────

// Parse digits in [s, end) into *out. Returns false if any non-digit or empty.
static bool parseUint(const char* s, const char* end, int& out) {
    if (s >= end) return false;
    int acc = 0;
    while (s < end) {
        if (*s < '0' || *s > '9') return false;
        acc = acc * 10 + (*s - '0');
        s++;
    }
    out = acc;
    return true;
}

// Parse one comma-separated segment (e.g. "*/15", "1-5", "3", "1-5/2")
// into field bits within [minVal, maxVal].
static bool parseSegment(const char* seg, const char* segEnd, int minVal, int maxVal, CronField& cf) {
    // Find slash (step)
    const char* slash = nullptr;
    for (const char* c = seg; c < segEnd; c++) {
        if (*c == '/') { slash = c; break; }
    }

    int step = 1;
    const char* rangeEnd = slash ? slash : segEnd;

    if (slash) {
        int s;
        if (!parseUint(slash + 1, segEnd, s) || s <= 0) return false;
        step = s;
    }

    int from, to;

    if (rangeEnd == seg + 1 && *seg == '*') {
        // "*" or "*/n"
        from = minVal;
        to = maxVal;
    } else {
        // Find dash (range separator) — must not be the first char
        const char* dash = nullptr;
        for (const char* c = seg + 1; c < rangeEnd; c++) {
            if (*c == '-') { dash = c; break; }
        }
        if (dash) {
            if (!parseUint(seg, dash, from)) return false;
            if (!parseUint(dash + 1, rangeEnd, to)) return false;
        } else {
            if (!parseUint(seg, rangeEnd, from)) return false;
            // Bare number with a step (e.g. "5/2") is not in our supported grammar
            if (slash) return false;
            to = from;
        }
    }

    if (from < minVal || to > maxVal || from > to) return false;

    for (int v = from; v <= to; v += step) {
        cf.bits[v] = true;
    }
    return true;
}

// Parse one full cron field string (may contain commas) within [minVal, maxVal].
static bool parseCronField(const char* field, int minVal, int maxVal, CronField& cf) {
    memset(&cf, 0, sizeof(cf));

    // Track whether the whole field was bare "*"
    cf.is_star = (field[0] == '*' && field[1] == '\0');

    const char* p = field;
    while (true) {
        const char* comma = strchr(p, ',');
        const char* segEnd = comma ? comma : (p + strlen(p));

        if (!parseSegment(p, segEnd, minVal, maxVal, cf)) return false;

        if (!comma) break;
        p = comma + 1;
        if (*p == '\0') return false;  // trailing comma
    }
    return true;
}

// Split expression on whitespace and parse all 5 fields.
static bool parseCronExpr(const char* expr, CronExpr& ce) {
    // Copy fields into fixed buffers (max 32 chars each)
    char fields[5][32];
    int count = 0;

    const char* p = expr;
    while (*p && count < 5) {
        while (*p == ' ' || *p == '\t') p++;
        if (!*p) break;
        int i = 0;
        while (*p && *p != ' ' && *p != '\t') {
            if (i >= 31) return false;  // field too long
            fields[count][i++] = *p++;
        }
        fields[count][i] = '\0';
        count++;
    }
    // Skip trailing whitespace and verify no extra tokens
    while (*p == ' ' || *p == '\t') p++;
    if (count != 5 || *p != '\0') return false;

    //                           min  max
    if (!parseCronField(fields[0],  0,  59, ce.minute)) return false;
    if (!parseCronField(fields[1],  0,  23, ce.hour))   return false;
    if (!parseCronField(fields[2],  1,  31, ce.dom))    return false;
    if (!parseCronField(fields[3],  1,  12, ce.month))  return false;
    if (!parseCronField(fields[4],  0,   6, ce.dow))    return false;

    return true;
}

// ─── Cron matching ────────────────────────────────────────────────────────────

static bool matchesCron(const struct tm& t, const CronExpr& ce) {
    if (!ce.minute.bits[t.tm_min])    return false;
    if (!ce.hour.bits[t.tm_hour])     return false;
    if (!ce.month.bits[t.tm_mon + 1]) return false;

    // Classic Vixie cron dom/dow semantics:
    //   both unrestricted  → any day
    //   only dom restricted → match dom
    //   only dow restricted → match dow
    //   both restricted    → match dom OR dow
    const bool dom_ok = ce.dom.bits[t.tm_mday];
    const bool dow_ok = ce.dow.bits[t.tm_wday];

    if (ce.dom.is_star && ce.dow.is_star) {
        // both bits arrays are fully set; dom_ok and dow_ok are both true
        return true;
    } else if (ce.dom.is_star) {
        return dow_ok;
    } else if (ce.dow.is_star) {
        return dom_ok;
    } else {
        return dom_ok || dow_ok;
    }
}

// ─── Schedule computations ────────────────────────────────────────────────────

static const int32_t FALLBACK_SECONDS = 3600;  // 1 hour safe fallback

// Legacy numeric schedule: mirrors existing contentmanager.cpp logic exactly.
static ScheduleResult legacySchedule(const char* s, time_t now) {
    ScheduleResult result{};

    int32_t minutes = (s && *s && strcmp(s, "null") != 0) ? (int32_t)atoi(s) : 0;
    int32_t interval = minutes * 60;

    if (interval == -1440 * 60) {
        // Next midnight in local time
        struct tm tm_local;
        localtime_r(&now, &tm_local);
        tm_local.tm_hour  = 0;
        tm_local.tm_min   = 0;
        tm_local.tm_sec   = 0;
        tm_local.tm_mday += 1;
        tm_local.tm_isdst = -1;
        const time_t midnight = mktime(&tm_local);
        interval = (int32_t)(midnight - now);
    } else if (interval < 0) {
        // Align to next interval boundary (preserve original formula verbatim)
        interval = -interval;
        const unsigned int secondsUntilNext =
            (unsigned int)((interval - (now % interval)) % interval);
        interval = (int32_t)secondsUntilNext;
    } else if (interval < 180) {
        interval = FALLBACK_SECONDS;
    }

    result.intervalSeconds = interval;
    return result;
}

// Cron-expression schedule: find next matching minute strictly after now.
static ScheduleResult cronSchedule(const char* expr, time_t now) {
    ScheduleResult result{};

    CronExpr ce;
    if (!parseCronExpr(expr, ce)) {
        result.error = "Invalid cron expression: ";
        result.error += expr;
        result.intervalSeconds = FALLBACK_SECONDS;
        return result;
    }

    // Start search at the beginning of the next full minute after now
    const time_t searchStart = now - (now % 60) + 60;

    // Bounded search: up to 2 years of minutes
    const int32_t maxMinutes = 2 * 366 * 24 * 60;

    for (int32_t i = 0; i < maxMinutes; i++) {
        const time_t candidate = searchStart + (time_t)i * 60;
        struct tm tm_local;
        localtime_r(&candidate, &tm_local);

        if (matchesCron(tm_local, ce)) {
            result.intervalSeconds = (int32_t)(candidate - now);
            return result;
        }
    }

    result.error = "No match within 2 years for cron expression: ";
    result.error += expr;
    result.intervalSeconds = FALLBACK_SECONDS;
    return result;
}

// ─── Public API ───────────────────────────────────────────────────────────────

// Returns true if s (trimmed) is a plain signed integer (or empty/"null").
static bool isSignedInteger(const char* s) {
    if (!s || *s == '\0') return true;
    if (strcmp(s, "null") == 0) return true;
    if (*s == '-' || *s == '+') s++;
    if (*s == '\0') return false;
    while (*s) {
        if (*s < '0' || *s > '9') return false;
        s++;
    }
    return true;
}

ScheduleResult computeSchedule(const char* intervalStr, time_t now) {
    // Trim leading/trailing whitespace into a local buffer
    char buf[256];
    if (!intervalStr) {
        return legacySchedule("0", now);
    }
    const char* s = intervalStr;
    while (*s == ' ' || *s == '\t') s++;

    size_t len = strlen(s);
    if (len >= sizeof(buf)) {
        // Suspiciously long → treat as invalid cron
        ScheduleResult r{};
        r.error = "Interval value too long";
        r.intervalSeconds = FALLBACK_SECONDS;
        return r;
    }
    memcpy(buf, s, len + 1);
    // Trim trailing whitespace
    while (len > 0 && (buf[len - 1] == ' ' || buf[len - 1] == '\t')) {
        buf[--len] = '\0';
    }

    if (isSignedInteger(buf)) {
        return legacySchedule(buf, now);
    } else {
        return cronSchedule(buf, now);
    }
}
