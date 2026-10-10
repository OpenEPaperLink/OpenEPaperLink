#include "util.h"

#include <Arduino.h>
#include <time.h>

String formatUtcToLocal(const String &s) {
    int h, m, ss = 0;
    if (sscanf(s.c_str(), "%d:%d:%d", &h, &m, &ss) < 2 || h > 23 || m > 59 || ss > 59)
        return "-";

    time_t n = time(nullptr);
    struct tm tm = *localtime(&n);
    tm.tm_hour = h;
    tm.tm_min = m;
    tm.tm_sec = ss;

    time_t utc = mktime(&tm) - _timezone + (tm.tm_isdst ? 3600 : 0);
    localtime_r(&utc, &tm);

    char buf[6];
    snprintf(buf, 6, "%02d:%02d", tm.tm_hour, tm.tm_min);
    return buf;
}

bool calculateSunriseSunset(double lat, double lon, time_t now, String &sunrise, String &sunset) {
    struct tm utc_tm;
    gmtime_r(&now, &utc_tm);
    int year = utc_tm.tm_year + 1900;
    int month = utc_tm.tm_mon + 1;
    int day = utc_tm.tm_mday;

    // Day of year
    int n1 = (int)(275.0 * month / 9.0);
    int n2 = (int)((month + 9.0) / 12.0);
    int n3 = 1 + (int)((year - 4 * (int)(year / 4.0) + 2.0) / 3.0);
    int doy = n1 - n2 * n3 + day - 30;

    double lngHour = lon / 15.0;
    const double zen = 90.833;  // official sunset/sunrise zenith angle

    double tRise = doy + (6.0 - lngHour) / 24.0;
    double tSet = doy + (18.0 - lngHour) / 24.0;

    auto meanAnomaly = [](double t) { return 0.9856 * t - 3.289; };

    auto trueLong = [](double M) {
        double Mrad = M * M_PI / 180.0;
        double L = M + 1.916 * sin(Mrad) + 0.020 * sin(2.0 * Mrad) + 282.634;
        if (L >= 360.0) L -= 360.0;
        if (L < 0.0) L += 360.0;
        return L;
    };

    auto rightAsc = [](double L) {
        double Lrad = L * M_PI / 180.0;
        double RA = atan(0.91764 * tan(Lrad)) * 180.0 / M_PI;
        double Lquad = floor(L / 90.0) * 90.0;
        double RAquad = floor(RA / 90.0) * 90.0;
        RA += Lquad - RAquad;
        return RA / 15.0;
    };

    auto declination = [](double L) {
        return asin(0.39782 * sin(L * M_PI / 180.0)) * 180.0 / M_PI;
    };

    auto hourAngle = [zen](double latVal, double dec) {
        double latRad = latVal * M_PI / 180.0;
        double decRad = dec * M_PI / 180.0;
        double cosH = (cos(zen * M_PI / 180.0) - sin(latRad) * sin(decRad)) / (cos(latRad) * cos(decRad));
        if (cosH > 1.0) return -1.0;   // never rises
        if (cosH < -1.0) return -2.0;  // never sets
        return acos(cosH) * 180.0 / M_PI;
    };

    double mR = meanAnomaly(tRise);
    double mS = meanAnomaly(tSet);
    double lR = trueLong(mR);
    double lS = trueLong(mS);
    double raR = rightAsc(lR);
    double raS = rightAsc(lS);
    double decR = declination(lR);
    double decS = declination(lS);
    double haR = hourAngle(lat, decR);
    double haS = hourAngle(lat, decS);

    if (haR < 0.0 || haS < 0.0) return false;

    // local mean time (T = H + RA - 0.06571 t - 6.622), then to UTC (UT = T - lngHour)
    double utcRise_h = raR - haR / 15.0 - 0.06571 * tRise - 6.622 - lngHour;
    double utcSet_h = raS + haS / 15.0 - 0.06571 * tSet - 6.622 - lngHour;

    while (utcRise_h < 0.0) utcRise_h += 24.0;
    while (utcRise_h >= 24.0) utcRise_h -= 24.0;
    while (utcSet_h < 0.0) utcSet_h += 24.0;
    while (utcSet_h >= 24.0) utcSet_h -= 24.0;

    // Format as UTC time string and convert to local via formatUtcToLocal
    char buf[16];
    int rh = (int)utcRise_h;
    int rm = (int)((utcRise_h - rh) * 60.0 + 0.5);
    if (rm >= 60) { rm -= 60; rh++; }
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d", rh, rm, 0);
    sunrise = formatUtcToLocal(String(buf));

    int sh = (int)utcSet_h;
    int sm = (int)((utcSet_h - sh) * 60.0 + 0.5);
    if (sm >= 60) { sm -= 60; sh++; }
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d", sh, sm, 0);
    sunset = formatUtcToLocal(String(buf));

    return true;
}

int calculateMoonPhase(time_t now) {
    // The date content is drawn once a day, so take the phase at local noon as the phase of the whole day
    struct tm noon_tm;
    localtime_r(&now, &noon_tm);
    noon_tm.tm_hour = 12;
    noon_tm.tm_min = noon_tm.tm_sec = 0;
    noon_tm.tm_isdst = -1;
    const time_t noon = mktime(&noon_tm);

    // Elongation of the moon from the sun with the largest periodic terms (Meeus, ch. 47), which puts new and
    // full moon within about 20 minutes; the mean synodic month alone can be more than half a day off
    const double T = ((double)noon / 86400.0 + 2440587.5 - 2451545.0) / 36525.0;
    const double rad = M_PI / 180.0;
    const double D = (297.8501921 + 445267.1114034 * T) * rad;  // mean elongation
    const double M = (357.5291092 + 35999.0502909 * T) * rad;   // sun's mean anomaly
    const double Mp = (134.9633964 + 477198.8675055 * T) * rad; // moon's mean anomaly
    const double F = (93.2720950 + 483202.0175233 * T) * rad;   // moon's argument of latitude
    double elong = D / rad + 6.289 * sin(Mp) + 1.274 * sin(2 * D - Mp) + 0.658 * sin(2 * D) + 0.214 * sin(2 * Mp) -
                   0.114 * sin(2 * F) - 2.101 * sin(M) - 0.020 * sin(2 * M);
    elong = fmod(elong, 360.0);
    if (elong < 0) elong += 360.0;

    // Map 0..360 degrees to the 28 moon icons (0..27); the end of the cycle rounds to the next new moon
    int idx = (int)(elong / 360.0 * 28.0 + 0.5);
    if (idx >= 28) idx = 0;
    return idx;
}