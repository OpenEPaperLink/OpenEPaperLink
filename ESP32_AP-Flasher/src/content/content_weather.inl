// Content: Weather and Forecast (open-meteo).
// Included once by contentmanager.cpp, not compiled on its own.

/// @brief Get a weather icon
/// @param id Icon identifier/index
/// @param isNight Use night icons (true) or not (false)
/// @return String reference to icon
const String getWeatherIcon(const uint8_t id, const bool isNight = false) {
    const String weatherIcons[] = {"\uf00d", "\uf00c", "\uf002", "\uf013", "\uf013", "\uf014", "", "", "\uf014", "", "",
                                   "\uf01a", "", "\uf01a", "", "\uf01a", "\uf017", "\uf017", "", "", "",
                                   "\uf019", "", "\uf019", "", "\uf019", "\uf015", "\uf015", "", "", "",
                                   "\uf01b", "", "\uf01b", "", "\uf01b", "", "\uf076", "", "", "\uf01a",
                                   "\uf01a", "\uf01a", "", "", "\uf064", "\uf064", "", "", "", "",
                                   "", "", "", "", "\uf01e", "\uf01d", "", "", "\uf01e"};
    if (isNight && id <= 2) {
        const String nightIcons[] = {"\uf02e", "\uf083", "\uf086"};
        return nightIcons[id];
    }
    return weatherIcons[id];
}

void drawWeatherContent(JsonDocument &doc, JsonDocument &loc, TFT_eSprite &spr, JsonObject &cfgobj, imgParam &imageParams, bool isForecast = false) {
    const auto &currentWeather = doc["current_weather"];
    const double temperature = currentWeather["temperature"].as<double>();
    float windspeed = currentWeather["windspeed"].as<float>();
    int windval = 0;
    const int winddirection = currentWeather["winddirection"].as<int>();
    const bool isNight = currentWeather["is_day"].as<int>() == 0;
    uint8_t weathercode = currentWeather["weathercode"].as<int>();
    if (weathercode > 40) weathercode -= 40;

    const uint8_t beaufort = windSpeedToBeaufort(windspeed);
    if (cfgobj["units"] != "1") {
        windval = beaufort;
    } else {
        windval = int(windspeed);
    }

    if (!isForecast) {
        const auto &location = loc["location"];
        drawString(spr, cfgobj["location"], location[0], location[1], location[2]);
    }
    const auto &wind = isForecast ? loc["currentwind"] : loc["wind"];
    drawString(spr, String(windval), wind[0], wind[1], wind[2], TR_DATUM, (beaufort > 4 ? imageParams.highlightColor : TFT_BLACK));

    char tmpOutput[5];
    dtostrf(temperature, 2, 1, tmpOutput);
    const auto &temp = loc["temp"];

    String temperatureStr = String(tmpOutput);
    if (temp[3] && temp[3] == 1) {
        temperatureStr += (cfgobj["units"] == "1") ? "°" : "°";
    }
    drawString(spr, temperatureStr, temp[0], temp[1], temp[2], TL_DATUM, (temperature < 0 ? imageParams.highlightColor : TFT_BLACK));

    const int iconcolor = (weathercode == 55 || weathercode == 65 || weathercode == 75 || weathercode == 82 || weathercode == 86 || weathercode == 95 || weathercode == 96 || weathercode == 99)
                              ? imageParams.highlightColor
                              : TFT_BLACK;
    const auto &icon = isForecast ? loc["currenticon"] : loc["icon"];
    drawString(spr, getWeatherIcon(weathercode, isNight), icon[0], icon[1], "/fonts/weathericons.ttf", icon[3], iconcolor, icon[2]);
    const auto &dir = loc["dir"];
    drawString(spr, windDirectionIcon(winddirection), dir[0], dir[1], "/fonts/weathericons.ttf", TC_DATUM, TFT_BLACK, dir[2]);
    if (weathercode > 10) {
        const auto &umbrella = loc["umbrella"];
        drawString(spr, "\uf084", umbrella[0], umbrella[1], "/fonts/weathericons.ttf", TC_DATUM, imageParams.highlightColor, umbrella[2]);
    }
}

void drawWeather(String &filename, JsonObject &cfgobj, const tagRecord *taginfo, imgParam &imageParams) {
    wsLog("get weather");

    getLocation(cfgobj);

    const String lat = cfgobj["#lat"];
    const String lon = cfgobj["#lon"];
    const String tz = cfgobj["#tz"];
    String units = "";
    if (cfgobj["units"] == "1") {
        units += "&temperature_unit=fahrenheit&windspeed_unit=mph&precipitation_unit=inch";
    }

    JsonDocument doc;
    const bool success = util::httpGetJson("https://api.open-meteo.com/v1/forecast?latitude=" + lat + "&longitude=" + lon + "&current_weather=true&windspeed_unit=ms&timezone=" + tz + units, doc, 5000);
    if (!success) {
        return;
    }
    JsonDocument loc;
    getTemplate(loc, 4, taginfo->hwType);

    TFT_eSprite spr = TFT_eSprite(&tft);
    tft.setTextWrap(false, false);
    initSprite(spr, imageParams.width, imageParams.height, imageParams);

    drawWeatherContent(doc, loc, spr, cfgobj, imageParams);

    spr2buffer(spr, filename, imageParams);
    spr.deleteSprite();
}

void drawForecast(String &filename, JsonObject &cfgobj, const tagRecord *taginfo, imgParam &imageParams) {
    wsLog("get weather");
    getLocation(cfgobj);

    String lat = cfgobj["#lat"];
    String lon = cfgobj["#lon"];
    String tz = cfgobj["#tz"];
    String units = "";
    if (cfgobj["units"] == "1") {
        units += "&temperature_unit=fahrenheit&windspeed_unit=mph&precipitation_unit=inch";
    }

    JsonDocument doc;
    const bool success = util::httpGetJson("https://api.open-meteo.com/v1/forecast?latitude=" + lat + "&longitude=" + lon + "&daily=weathercode,temperature_2m_max,temperature_2m_min,precipitation_sum,windspeed_10m_max,winddirection_10m_dominant&current_weather=true&windspeed_unit=ms&timeformat=unixtime&timezone=" + tz + units, doc, 5000);
    if (!success) {
        return;
    }

    TFT_eSprite spr = TFT_eSprite(&tft);
    tft.setTextWrap(false, false);

    JsonDocument loc;
    getTemplate(loc, 8, taginfo->hwType);
    initSprite(spr, imageParams.width, imageParams.height, imageParams);

    if (loc["temp"]) drawWeatherContent(doc, loc, spr, cfgobj, imageParams, true);

    const auto &location = loc["location"];
    drawString(spr, cfgobj["location"], location[0], location[1], location[2], TL_DATUM, TFT_BLACK);
    const auto &daily = doc["daily"];
    const auto &column = loc["column"];
    const int column1 = column[1].as<int>();
    const auto &day = loc["day"];
    const unsigned long utc_offset = doc["utc_offset_seconds"];
    for (uint8_t dag = 0; dag < column[0]; dag++) {
        const time_t weatherday = (daily["time"][dag].as<time_t>() + utc_offset);
        const struct tm *datum = localtime(&weatherday);

        drawString(spr, String(languageDaysShort[datum->tm_wday]), dag * column1 + day[0].as<int>(), day[1], day[2], TC_DATUM, TFT_BLACK);

        uint8_t weathercode = daily["weathercode"][dag].as<int>();
        if (weathercode > 40) weathercode -= 40;

        const int iconcolor = (weathercode == 55 || weathercode == 65 || weathercode == 75 || weathercode == 82 || weathercode == 86 || weathercode == 95 || weathercode == 96 || weathercode == 99)
                                  ? imageParams.highlightColor
                                  : TFT_BLACK;
        drawString(spr, getWeatherIcon(weathercode), loc["icon"][0].as<int>() + dag * column1, loc["icon"][1], "/fonts/weathericons.ttf", TC_DATUM, iconcolor, loc["icon"][2]);

        drawString(spr, windDirectionIcon(daily["winddirection_10m_dominant"][dag]), loc["wind"][0].as<int>() + dag * column1, loc["wind"][1], "/fonts/weathericons.ttf", TC_DATUM, TFT_BLACK, loc["icon"][2]);

        const int8_t tmin = round(daily["temperature_2m_min"][dag].as<double>());
        const int8_t tmax = round(daily["temperature_2m_max"][dag].as<double>());
        uint8_t wind;
        const int8_t beaufort = windSpeedToBeaufort(daily["windspeed_10m_max"][dag].as<double>());
        if (cfgobj["units"] == "1") {
            wind = daily["windspeed_10m_max"][dag].as<int>();
        } else {
            wind = beaufort;
        }

        if (loc["rain"]) {
            if (cfgobj["units"] == "0") {
                const int8_t rain = round(daily["precipitation_sum"][dag].as<double>());
                if (rain > 0) {
                    drawString(spr, String(rain) + "mm", dag * column1 + loc["rain"][0].as<int>(), loc["rain"][1], day[2], TC_DATUM, (rain > 10 ? imageParams.highlightColor : TFT_BLACK));
                }
            } else {
                double fRain = daily["precipitation_sum"][dag].as<double>();
                fRain = round(fRain * 100.0) / 100.0;
                if (fRain > 0.0) {
                    // inch, display if > .01 inches
                    drawString(spr, String(fRain) + "in", dag * column1 + loc["rain"][0].as<int>(), loc["rain"][1], day[2], TC_DATUM, (fRain > 0.5 ? imageParams.highlightColor : TFT_BLACK));
                }
            }
        }

        drawString(spr, String(tmin) + " ", dag * column1 + day[0].as<int>(), day[4], day[2], TR_DATUM, (tmin < 0 ? imageParams.highlightColor : TFT_BLACK));
        drawString(spr, String(" ") + String(tmax), dag * column1 + day[0].as<int>(), day[4], day[2], TL_DATUM, (tmax < 0 ? imageParams.highlightColor : TFT_BLACK));
        drawString(spr, " " + String(wind), dag * column1 + column1 / 2, day[3], day[2], TL_DATUM, (beaufort > 5 ? imageParams.highlightColor : TFT_BLACK));
        if (dag > 0) {
            for (int i = loc["line"][0]; i < loc["line"][1]; i += 3) {
                spr.drawPixel(dag * column1, i, TFT_BLACK);
            }
        }
    }

    spr2buffer(spr, filename, imageParams);
    spr.deleteSprite();
}

int windSpeedToBeaufort(const float windSpeed) {
    constexpr const float speeds[] = {0.3, 1.5, 3.3, 5.5, 8, 10.8, 13.9, 17.2, 20.8, 24.5, 28.5, 32.7};
    constexpr const int numSpeeds = sizeof(speeds) / sizeof(speeds[0]);
    int beaufort = 0;
    for (int i = 0; i < numSpeeds; i++) {
        if (windSpeed >= speeds[i]) {
            beaufort = i + 1;
        }
    }
    return beaufort;
}

String windDirectionIcon(const int degrees) {
    const String directions[] = {"\uf044", "\uf043", "\uf048", "\uf087", "\uf058", "\uf057", "\uf04d", "\uf088"};
    int index = (degrees + 22) / 45;
    if (index >= 8) {
        index = 0;
    }
    return directions[index];
}

void getLocation(JsonObject &cfgobj) {
    const String lat = cfgobj["#lat"];
    const String lon = cfgobj["#lon"];

    if (util::isEmptyOrNull(lat) || util::isEmptyOrNull(lon)) {
        wsLog("get location");
        JsonDocument filter;
        filter["results"][0]["latitude"] = true;
        filter["results"][0]["longitude"] = true;
        filter["results"][0]["timezone"] = true;
        JsonDocument doc;
        if (util::httpGetJson("https://geocoding-api.open-meteo.com/v1/search?name=" + urlEncode(cfgobj["location"]) + "&count=1", doc, 5000, &filter)) {
            cfgobj["#lat"] = doc["results"][0]["latitude"].as<String>();
            cfgobj["#lon"] = doc["results"][0]["longitude"].as<String>();
            cfgobj["#tz"] = doc["results"][0]["timezone"].as<String>();
        }
    }
}
