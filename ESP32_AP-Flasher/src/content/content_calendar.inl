// Content: calendar (Google Apps Script), list and week view.
// Included once by contentmanager.cpp, not compiled on its own.

char *epoch_to_display(time_t utc) {
    static char display[6];
    struct tm local_tm;
    localtime_r(&utc, &local_tm);
    time_t now;
    time(&now);
    struct tm now_tm;
    localtime_r(&now, &now_tm);
    if (local_tm.tm_year < now_tm.tm_year ||
        (local_tm.tm_year == now_tm.tm_year && local_tm.tm_mon < now_tm.tm_mon) ||
        (local_tm.tm_year == now_tm.tm_year && local_tm.tm_mon == now_tm.tm_mon && local_tm.tm_mday < now_tm.tm_mday) ||
        (local_tm.tm_hour == 0 && local_tm.tm_min == 0) ||
        difftime(utc, now) >= 86400) {
        strftime(display, sizeof(display), languageDateFormat[1].c_str(), &local_tm);
    } else {
        strftime(display, sizeof(display), "%H:%M", &local_tm);
    }
    return display;
}

#ifdef CONTENT_CAL
bool getCalFeed(String &filename, JsonObject &cfgobj, tagRecord *&taginfo, imgParam &imageParams) {
    // google apps scripts method to retrieve calendar
    // see https://github.com/OpenEPaperLink/OpenEPaperLink/wiki/Google-Apps-Scripts for description

    wsLog("get calendar");

    JsonDocument loc;
    getTemplate(loc, 11, taginfo->hwType);

    String URL = cfgobj["apps_script_url"].as<String>() + "?days=" + loc["days"].as<String>();

    time_t now;
    time(&now);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    char dateString[40];
    strftime(dateString, sizeof(dateString), languageDateFormat[0].c_str(), &timeinfo);

    HTTPClient http;
    // logLine("http getCalFeed " + URL);
    http.begin(URL);
    http.setTimeout(10000);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();
    if (httpCode != 200) {
        wsErr("getCalFeed http error " + String(httpCode));
        logLineRateLimited("getCalFeed", "calendar: http error " + String(httpCode));
        return false;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, http.getString());
    if (error) {
        wsErr(error.c_str());
    }
    http.end();

    TFT_eSprite spr = TFT_eSprite(&tft);

    if (loc["rotate"] == 1) {
        int temp = imageParams.height;
        imageParams.height = imageParams.width;
        imageParams.width = temp;
        imageParams.rotatebuffer = 1 - (imageParams.rotatebuffer % 2);
        initSprite(spr, imageParams.width, imageParams.height, imageParams);
    } else {
        initSprite(spr, imageParams.width, imageParams.height, imageParams);
    }

    switch (loc["mode"].as<int>()) {
        case 0: {
            // appointment list
            String title = cfgobj["title"];
            if (util::isEmptyOrNull(title)) title = "Calendar";
            drawString(spr, title, loc["title"][0], loc["title"][1], loc["title"][2], TL_DATUM, TFT_BLACK);
            drawString(spr, dateString, loc["date"][0], loc["date"][1], loc["title"][2], TR_DATUM, TFT_BLACK);

            int n = doc.size();
            if (n > loc["items"]) n = loc["items"];
            for (int i = 0; i < n; i++) {
                const JsonObject &obj = doc[i];
                const String eventtitle = obj["title"];
                const time_t starttime = obj["start"];
                const time_t endtime = obj["end"];

                if (starttime <= now && endtime > now) {
                    spr.fillRect(loc["red"][0], loc["red"][1].as<int>() + i * loc["line"][2].as<int>(), loc["red"][2], loc["red"][3], imageParams.highlightColor);
                    drawString(spr, epoch_to_display(obj["start"]), loc["line"][0], loc["line"][1].as<int>() + i * loc["line"][2].as<int>(), loc["line"][3], TL_DATUM, TFT_WHITE, 0, imageParams.highlightColor);
                    drawString(spr, eventtitle, loc["line"][4], loc["line"][1].as<int>() + i * loc["line"][2].as<int>(), loc["line"][3], TL_DATUM, TFT_WHITE, 0, imageParams.highlightColor);
                } else {
                    drawString(spr, epoch_to_display(obj["start"]), loc["line"][0], loc["line"][1].as<int>() + i * loc["line"][2].as<int>(), loc["line"][3], TL_DATUM, TFT_BLACK, 0, TFT_WHITE);
                    drawString(spr, eventtitle, loc["line"][4], loc["line"][1].as<int>() + i * loc["line"][2].as<int>(), loc["line"][3], TL_DATUM, TFT_BLACK, 0, TFT_WHITE);
                }
            }
            break;
        }
        case 1: {
#ifdef CONTENT_BIGCAL
            // week view

            // gridparam:
            // 0: 10; offset top
            // 1: 20; offset cal block
            // 2: 30; offset left
            // 3: calibrib16.vlw; headers font
            // 4: tahoma9.vlw; appointments font
            // 5: 14; line height

            timeinfo.tm_hour = 0;
            timeinfo.tm_min = 0;
            timeinfo.tm_sec = 0;
            time_t midnightEpoch = mktime(&timeinfo);

            int calWidth = imageParams.width - 1;
            int calHeight = imageParams.height;
            int calDays = loc["days"];

            int colLeft = loc["gridparam"][2].as<int>();
            int colWidth = (calWidth - colLeft) / calDays;

            int calTop = loc["gridparam"][0].as<int>();
            int calBottom = calHeight;
            int calYOffset = loc["gridparam"][1].as<int>();
            int lineHeight = loc["gridparam"][5].as<int>();

            uint16_t backgroundLight = getColor("lightgray");
            uint16_t backgroundDark = getColor("darkgray");
            if (imageParams.hwdata.bpp >= 3) {
                backgroundLight = getColor("#BEFAFF");
                backgroundDark = getColor("#79FAFF");
            }

            // drawString(spr, String(timeinfo.tm_mday), calWidth / 2, -calHeight/5, "Signika-SB.ttf", TC_DATUM, imageParams.highlightColor, calHeight * 1.2);

            for (int i = 0; i < calDays; i++) {
                struct tm dayTimeinfo = *localtime(&midnightEpoch);
                dayTimeinfo.tm_mday += i;
                time_t dayEpoch = mktime(&dayTimeinfo);
                struct tm *dayInfo = localtime(&dayEpoch);

                int colStart = colLeft + i * colWidth;
                spr.drawLine(colStart, calTop, colStart, calBottom, TFT_BLACK);
                drawString(spr, String(languageDaysShort[dayInfo->tm_wday]) + " " + String(dayInfo->tm_mday), colStart + colWidth / 2, calTop, loc["gridparam"][3], TC_DATUM, TFT_BLACK);

                if (dayInfo->tm_wday == 0 || dayInfo->tm_wday == 6) {
                    spr.fillRect(colStart + 1, calTop + calYOffset, colWidth - 1, calHeight - 1, backgroundDark);
                } else {
                    spr.fillRect(colStart + 1, calTop + calYOffset, colWidth - 1, calHeight - 1, backgroundLight);
                }
            }

            int colStart = colLeft + calDays * colWidth;
            spr.drawLine(colStart, calTop, colStart, calBottom, TFT_BLACK);
            spr.drawLine(0, calTop + calYOffset, calWidth, calTop + calYOffset, TFT_BLACK);

            int minHour = loc["period"][0] | 9;
            int maxHour = loc["period"][1] | 17;

            int n = doc.size();
            int maxBlock = 0;
            int *block = new int[n];

            for (int i = 0; i < n; i++) {
                const JsonObject &obj = doc[i];
                String eventtitle = obj["title"];
                const time_t startdatetime = obj["start"];
                const time_t enddatetime = obj["end"];
                const bool isallday = obj["isallday"];
                const int calendarId = obj["calendar"];

                if (!isallday) {
                    localtime_r(&startdatetime, &timeinfo);
                    if (timeinfo.tm_hour < minHour) minHour = timeinfo.tm_hour;

                    localtime_r(&enddatetime, &timeinfo);
                    if (timeinfo.tm_hour > maxHour) maxHour = timeinfo.tm_hour;
                    if (timeinfo.tm_min > 1 && timeinfo.tm_hour + 1 > maxHour) maxHour = timeinfo.tm_hour + 1;
                } else {
                    int fulldaystart = constrain((startdatetime - midnightEpoch) / (24 * 3600), 0, calDays);
                    int fulldayend = constrain((enddatetime - midnightEpoch) / (24 * 3600), 0, calDays);
                    if (fulldaystart < calDays) {
                        int line = 1;
                        bool overlap = false;
                        do {
                            overlap = false;
                            for (int j = 0; j < i; j++) {
                                const JsonObject &obj2 = doc[j];
                                const bool isallday2 = obj2["isallday"];
                                const time_t startdatetime2 = obj2["start"];
                                const time_t enddatetime2 = obj2["end"];
                                if (startdatetime < enddatetime2 && enddatetime > startdatetime2 &&
                                    line == block[j] && isallday2) {
                                    overlap == true;
                                    line++;
                                }
                            }
                        } while (overlap == true);

                        int16_t eventX = colLeft + fulldaystart * colWidth + 3;
                        int16_t eventY = calTop + calYOffset + (line - 1) * lineHeight + 3;
                        uint16_t background = TFT_WHITE;
                        uint16_t border = TFT_BLACK;
                        uint16_t textcolor = TFT_BLACK;
                        if (loc["colors1"].is<JsonArray>() && loc["colors1"].size() > calendarId) {
                            background = getColor(loc["colors1"][calendarId]);
                            border = getColor(loc["colors2"][calendarId]);
                            if (loc["colors3"][calendarId]) textcolor = getColor(loc["colors3"][calendarId]);
                        }
                        spr.fillRect(eventX - 1, eventY - 2, colWidth * (fulldayend - fulldaystart) - 3, lineHeight - 1, background);
                        spr.drawRect(eventX - 2, eventY - 3, colWidth * (fulldayend - fulldaystart) - 1, lineHeight + 1, border);
                        drawTextBox(spr, eventtitle, eventX, eventY, colWidth * (fulldayend - fulldaystart) - 3, 15, loc["gridparam"][4], textcolor);

                        block[i] = line;
                        if (line > maxBlock) maxBlock = line;
                    }
                }

                const int starttime = timeinfo.tm_hour * 60 + timeinfo.tm_min;

                int hours = timeinfo.tm_hour;
                int minutes = timeinfo.tm_min;
            }
            calYOffset += maxBlock * lineHeight;

            spr.drawLine(0, calTop + calYOffset, calWidth, calTop + calYOffset, TFT_BLACK);
            int hourHeight = (calHeight - calYOffset - calTop) / (maxHour - minHour);
            for (int i = 0; i < (maxHour - minHour); i++) {
                spr.drawLine(0, calTop + calYOffset + i * hourHeight, colLeft + 10, calTop + calYOffset + i * hourHeight, TFT_BLACK);
                for (int j = 1; j <= calDays; j++) {
                    spr.drawLine(colLeft + j * colWidth - 5, calTop + calYOffset + i * hourHeight, colLeft + j * colWidth + 5, calTop + calYOffset + i * hourHeight, TFT_BLACK);
                }
                drawString(spr, String(minHour + i), colLeft - 2, calTop + calYOffset + i * hourHeight + 2, loc["gridparam"][3], TR_DATUM, TFT_BLACK);
            }
            spr.drawLine(0, calTop + calYOffset + (maxHour - minHour) * hourHeight, calWidth, calTop + calYOffset + (maxHour - minHour) * hourHeight, TFT_BLACK);

            for (int i = 0; i < n; i++) {
                const JsonObject &obj = doc[i];
                String eventtitle = obj["title"];
                const time_t startdatetime = obj["start"];
                const time_t enddatetime = obj["end"];
                const bool isallday = obj["isallday"];
                const int calendarId = obj["calendar"];

                if (!isallday) {
                    int fulldaystart = constrain((startdatetime - midnightEpoch) / (24 * 3600), 0, calDays);
                    int fulldayend = constrain((enddatetime - midnightEpoch) / (24 * 3600), 0, calDays);

                    for (int day = fulldaystart; day <= fulldayend; day++) {
                        localtime_r(&startdatetime, &timeinfo);
                        int starttime = timeinfo.tm_hour * 60 + timeinfo.tm_min - minHour * 60;
                        int duration = (enddatetime - startdatetime) / 60;
                        if (day > fulldaystart) {
                            starttime = 0;
                            localtime_r(&enddatetime, &timeinfo);
                            duration = timeinfo.tm_hour * 60 + timeinfo.tm_min - minHour * 60;
                        }
                        char formattedTime[6];
                        strftime(formattedTime, sizeof(formattedTime), "%H:%M", &timeinfo);
                        String formattedTimeString = String(formattedTime);

                        int indent = 1;
                        bool overlap = false;
                        do {
                            overlap = false;
                            for (int j = 0; j < i; j++) {
                                const JsonObject &obj2 = doc[j];
                                const bool isallday2 = obj2["isallday"];
                                const time_t startdatetime2 = obj2["start"];
                                const time_t enddatetime2 = obj2["end"];
                                if (startdatetime < enddatetime2 && enddatetime > startdatetime2 &&
                                    indent == block[j] && isallday2 == false) {
                                    overlap == true;
                                    indent++;
                                }
                            }
                        } while (overlap == true);

                        block[i] = indent;
                        int16_t eventX = colLeft + day * colWidth + (indent - 1) * 5;
                        int16_t eventY = calTop + calYOffset + (starttime * hourHeight / 60);
                        uint16_t background = TFT_WHITE;
                        uint16_t border = TFT_BLACK;
                        uint16_t textcolor = TFT_BLACK;
                        if (loc["colors1"].is<JsonArray>() && loc["colors1"].size() > calendarId) {
                            background = getColor(loc["colors1"][calendarId]);
                            border = getColor(loc["colors2"][calendarId]);
                            if (loc["colors3"][calendarId]) textcolor = getColor(loc["colors3"][calendarId]);
                        }
                        spr.fillRect(eventX + 2, eventY + 1, colWidth - 3, (duration * hourHeight / 60) - 1, background);
                        spr.drawRect(eventX + 1, eventY, colWidth - 1, (duration * hourHeight / 60) + 1, border);
                        eventX += 2;
                        eventY += 2;
                        if (day == fulldaystart) {
                            eventtitle = formattedTimeString + " " + String(eventtitle);
                            drawTextBox(spr, formattedTimeString, eventX, eventY, colWidth - 1, (duration * hourHeight / 60) - 1, loc["gridparam"][4], textcolor, TFT_WHITE, 1);
                            eventX++;
                            eventY = calTop + calYOffset + (starttime * hourHeight / 60) + 2;
                        } else {
                            eventtitle = obj["title"].as<String>();
                        }
                        drawTextBox(spr, eventtitle, eventX, eventY, colWidth - 1, (duration * hourHeight / 60) - 1, loc["gridparam"][4], textcolor, TFT_WHITE, 1);
                    }
                }
            }

            delete[] block;
#endif
            break;
        }
    }

    spr2buffer(spr, filename, imageParams);
    spr.deleteSprite();
    return true;
}
#endif
