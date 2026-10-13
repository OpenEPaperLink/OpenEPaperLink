// Data for tags that is not an image: NFC URL, tag config, time.
// Included once by contentmanager.cpp, not compiled on its own.

#ifdef CONTENT_NFCLUT
void prepareNFCReq(const uint8_t *dst, const char *url) {
    uint8_t *data;
    size_t len = strlen(url);
    data = new uint8_t[len + 8];

    // TLV
    data[0] = 0x03;  // NDEF message (TLV type)
    data[1] = 4 + len + 1;
    // ndef record
    data[2] = 0xD1;
    data[3] = 0x01;     // well known record type
    data[4] = len + 1;  // payload length
    data[5] = 0x55;     // payload type (URI record)
    data[6] = 0x00;     // URI identifier code (no prepending)

    memcpy(data + 7, reinterpret_cast<const uint8_t *>(url), len);
    len = 7 + len;
    data[len] = 0xFE;
    len = 1 + len;
    prepareDataAvail(data, len, DATATYPE_NFC_RAW_CONTENT, dst);  // copies the data
    delete[] data;
}
#endif

#ifdef CONTENT_TAGCFG
void prepareConfigFile(const uint8_t *dst, const JsonObject &config) {
    struct tagsettings tagSettings;
    tagSettings.settingsVer = 1;
    tagSettings.enableFastBoot = config["fastboot"].as<int>();
    tagSettings.enableRFWake = config["rfwake"].as<int>();
    tagSettings.enableTagRoaming = config["tagroaming"].as<int>();
    tagSettings.enableScanForAPAfterTimeout = config["tagscanontimeout"].as<int>();
    tagSettings.enableLowBatSymbol = config["showlowbat"].as<int>();
    tagSettings.enableNoRFSymbol = config["shownorf"].as<int>();
    tagSettings.customMode = 0;
    tagSettings.fastBootCapabilities = 0;
    // seconds; 1 = no minimum: the tag uses its own default (40 s)
    const int minCheckIn = config["mincheckin"].as<int>();
    tagSettings.minimumCheckInTime = minCheckIn > 0 ? minCheckIn : 1;
    tagSettings.fixedChannel = config["fixedchannel"].as<int>();
    tagSettings.batLowVoltage = config["lowvoltage"].as<int>();
    prepareDataAvail((uint8_t *)&tagSettings, sizeof(tagSettings), 0xA8, dst);
}
#endif

#ifdef CONTENT_TIME_RAWDATA
bool is_leap_year(int year) {// Somehow mktime did not return the local unix time so lets to it in a manual way
    year += 1900;
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}
int days_in_month(int month, int year) {
    static const int days[] = { 31, 28, 31, 30, 31, 30,
                                31, 31, 30, 31, 30, 31 };
    if (month == 1 && is_leap_year(year)) {
        return 29;
    }
    return days[month];
}
uint32_t convert_tm_to_seconds(struct tm *t) {
    const int SECONDS_PER_MINUTE = 60;
    const int SECONDS_PER_HOUR = 3600;
    const int SECONDS_PER_DAY = 86400;
    long long total_days = 0;
    for (int year = 70; year < t->tm_year; year++) {
        total_days += is_leap_year(year) ? 366 : 365;
    }
    for (int month = 0; month < t->tm_mon; month++) {
        total_days += days_in_month(month, t->tm_year);
    }
    total_days += (t->tm_mday - 1);
    long long total_seconds = total_days * SECONDS_PER_DAY;
    total_seconds += t->tm_hour * SECONDS_PER_HOUR;
    total_seconds += t->tm_min * SECONDS_PER_MINUTE;
    total_seconds += t->tm_sec;
    return total_seconds;
}

void prepareTIME_RAW(const uint8_t *dst, time_t now) {
    uint8_t *data;
    size_t len = 1 + 4 + 4 + 1;
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    uint32_t local_time = convert_tm_to_seconds(&timeinfo) + 20;// Adding 20 seconds for the average of upload time
    uint32_t unix_time = now + 20;
    data = new uint8_t[len + 1];
    data[0] = len;// Length all
    data[1] = 0x01;// Version of Time and RAW
    data[2] = ((uint8_t*)&local_time)[0];
    data[3] = ((uint8_t*)&local_time)[1];
    data[4] = ((uint8_t*)&local_time)[2];
    data[5] = ((uint8_t*)&local_time)[3];
    data[6] = ((uint8_t*)&unix_time)[0];
    data[7] = ((uint8_t*)&unix_time)[1];
    data[8] = ((uint8_t*)&unix_time)[2];
    data[9] = ((uint8_t*)&unix_time)[3];
    data[10] = 1; // Design, 1 = Full DateTime 2 = Just Time Segment Style
    prepareDataAvail(data, len + 1, DATATYPE_TIME_RAW_DATA, dst);  // copies the data
    delete[] data;
}
#endif
