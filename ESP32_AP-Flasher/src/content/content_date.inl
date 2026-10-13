// Content: Today (date), CountDays / CountHours (number).
// Included once by contentmanager.cpp, not compiled on its own.

void drawDate(String &filename, JsonObject &cfgobj, tagRecord *&taginfo, imgParam &imageParams) {
    time_t now;
    time(&now);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);

    const int month_number = timeinfo.tm_mon;
    const int year_number = timeinfo.tm_year + 1900;

    if (taginfo->hwType == SOLUM_SEG_UK) {
        sprintf(imageParams.segments, "%2d%2d%-2.2s%04d", timeinfo.tm_mday, month_number + 1, languageDays[timeinfo.tm_wday], year_number);
        imageParams.symbols = 0x04;
        return;
    }

    JsonDocument loc;
    getTemplate(loc, 1, taginfo->hwType);

    TFT_eSprite spr = TFT_eSprite(&tft);
    initSprite(spr, imageParams.width, imageParams.height, imageParams);

    auto date = loc["date"];
    auto weekday = loc["weekday"];
    const auto &month = loc["month"];
    const auto &day = loc["day"];

    if (cfgobj["location"]) {
        const double lat = cfgobj["#lat"].as<String>().toDouble();
        const double lon = cfgobj["#lon"].as<String>().toDouble();

        String sunrise, sunset;
        const bool sunOk = calculateSunriseSunset(lat, lon, now, sunrise, sunset);
        if (sunOk && loc["sunrise"].is<JsonArray>()) {
            const auto &sunriseicon = loc["sunrise"];
            const auto &sunseticon = loc["sunset"];
            drawString(spr, String("\uF046 "), sunriseicon[0], sunriseicon[1].as<int>(), "/fonts/weathericons.ttf", TR_DATUM, TFT_BLACK, sunriseicon[3]);
            drawString(spr, String("\uF047 "), sunseticon[0], sunseticon[1].as<int>(), "/fonts/weathericons.ttf", TR_DATUM, TFT_BLACK, sunseticon[3]);
            drawString(spr, sunrise, sunriseicon[0], sunriseicon[1], sunriseicon[2], TL_DATUM, TFT_BLACK, sunriseicon[3]);
            drawString(spr, sunset, sunseticon[0], sunseticon[1], sunseticon[2], TL_DATUM, TFT_BLACK, sunseticon[3]);

            date = loc["altdate"];
            weekday = loc["altweekday"];
        }

        if (loc["moonicon"].is<JsonArray>()) {
            int moonage = calculateMoonPhase(now);
            const auto &moonicon = loc["moonicon"];
            uint16_t moonIconId = 0xf095 + moonage;
            String moonIcon = utf8FromCodepoint(moonIconId);
            drawString(spr, moonIcon, moonicon[0], moonicon[1], "/fonts/weathericons.ttf", TC_DATUM, TFT_BLACK, moonicon[2]);
        }
    }
    if (date.is<JsonArray>()) {
        drawString(spr, languageDays[timeinfo.tm_wday], weekday[0], weekday[1], weekday[2], TC_DATUM, imageParams.highlightColor, weekday[3]);
        drawString(spr, String(timeinfo.tm_mday) + " " + languageMonth[timeinfo.tm_mon], date[0], date[1], date[2], TC_DATUM, TFT_BLACK, date[3]);
    } else {
        drawString(spr, languageDays[timeinfo.tm_wday], weekday[0], weekday[1], weekday[2], TC_DATUM, TFT_BLACK, weekday[3]);
        drawString(spr, String(languageMonth[timeinfo.tm_mon]), month[0], month[1], month[2], TC_DATUM, TFT_BLACK, month[3]);
        drawString(spr, String(timeinfo.tm_mday), day[0], day[1], day[2], TC_DATUM, imageParams.highlightColor, day[3]);
    }
    spr2buffer(spr, filename, imageParams);
    spr.deleteSprite();
}

void drawNumber(String &filename, int32_t count, int32_t thresholdred, tagRecord *&taginfo, imgParam &imageParams) {
    int32_t countTemp = count;
    count = abs(count);
    if (taginfo->hwType == SOLUM_SEG_UK) {
        imageParams.symbols = 0x00;
        if (count > 19999) {
            sprintf(imageParams.segments, "over  flow");
            return;
        } else if (count > 9999) {
            imageParams.symbols = 0x02;
            sprintf(imageParams.segments, "%04d", count - 10000);
        } else {
            sprintf(imageParams.segments, "%4d", count);
        }
        if (taginfo->contentMode == 3) {
            strcat(imageParams.segments, "  hour");
        } else {
            strcat(imageParams.segments, "  days");
        }
        return;
    }

    TFT_eSprite spr = TFT_eSprite(&tft);

    JsonDocument loc;
    getTemplate(loc, 2, taginfo->hwType);

    initSprite(spr, imageParams.width, imageParams.height, imageParams);
    uint16_t color = TFT_BLACK;
    if (countTemp > thresholdred) {
        color = imageParams.highlightColor;
    }
    String font = loc["fonts"][0].as<String>();
    uint8_t size = loc["fonts"][1].as<uint16_t>();
    if (count > 9) size = loc["fonts"][2].as<uint16_t>();
    if (count > 99) size = loc["fonts"][3].as<uint16_t>();
    if (count > 999) size = loc["fonts"][4].as<uint16_t>();
    if (count > 9999) size = loc["fonts"][5].as<uint16_t>();
    if (count > 99999) size = loc["fonts"][6].as<uint16_t>();
    drawString(spr, String(count), loc["xy"][0].as<uint16_t>(), loc["xy"][1].as<uint16_t>() - size / 1.8, font, TC_DATUM, color, size);

    spr2buffer(spr, filename, imageParams);
    spr.deleteSprite();
}
