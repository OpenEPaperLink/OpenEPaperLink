// Content: Buienradar rain forecast.
// Included once by contentmanager.cpp, not compiled on its own.

#ifdef CONTENT_BUIENRADAR
uint8_t drawBuienradar(String &filename, JsonObject &cfgobj, tagRecord *&taginfo, imgParam &imageParams) {
    uint8_t refresh = 60;
    wsLog("get buienradar");

    getLocation(cfgobj);
    HTTPClient http;

    String lat = cfgobj["#lat"];
    String lon = cfgobj["#lon"];
    // logLine("http drawBuienradar");
    http.begin("https://gadgets.buienradar.nl/data/raintext/?lat=" + lat + "&lon=" + lon);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    http.setTimeout(5000);
    int httpCode = http.GET();

    if (httpCode == 200) {
        TFT_eSprite spr = TFT_eSprite(&tft);

        JsonDocument loc;
        getTemplate(loc, 16, taginfo->hwType);
        initSprite(spr, imageParams.width, imageParams.height, imageParams);

        tft.setTextWrap(false, false);

        String response = http.getString();

        drawString(spr, cfgobj["location"], loc["location"][0], loc["location"][1], loc["location"][2]);

        const auto &bars = loc["bars"];
        const auto &cols = loc["cols"];
        const int cols0 = cols[0].as<int>();
        const int cols1 = cols[1].as<int>();
        const int cols2 = cols[2].as<int>();
        const String cols3 = cols[3].as<String>();
        const int bars0 = bars[0].as<int>();
        const int bars1 = bars[1].as<int>();
        const int bars2 = bars[2].as<int>();

        float factor = (float)bars1 / 111;
        for (int i = 0; i < imageParams.width; i += 4) {
            int yCoordinates[] = {1, 20, 29, 39, 49, 55, 59};
            for (int y : yCoordinates) {
                spr.drawPixel(i, bars1 - (y * factor), TFT_BLACK);
            }
        }

        drawString(spr, "Buienradar", loc["title"][0], loc["title"][1], loc["title"][2]);

        for (int i = 0; i < 24; i++) {
            const int startPos = i * 10;
            uint8_t value = response.substring(startPos, startPos + 3).toInt();
            const String timestring = response.substring(startPos + 4, startPos + 9);
            const int minutes = timestring.substring(3).toInt();
            if (value < 70) {
                value = 70;
            } else if (value > 180) {
                value = 180;
            }
            if (value > 70) {
                if (i < 12) {
                    refresh = 10;
                } else if (refresh > 10) {
                    refresh = 20;
                }
            }
            value = value - 70;

            spr.fillRect(i * cols2 + bars0, bars1 - (value * factor), bars2, value * factor, (value > 50 ? imageParams.highlightColor : TFT_BLACK));

            if (minutes % 15 == 0) {
                drawString(spr, timestring, i * cols2 + cols0, cols1, cols3);
            }
        }

        spr2buffer(spr, filename, imageParams);
        spr.deleteSprite();
    } else {
        wsErr("Buitenradar http " + String(httpCode));
    }
    http.end();
    return refresh;
}
#endif
