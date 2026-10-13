// Content: timestamp (button press log).
// Included once by contentmanager.cpp, not compiled on its own.

#ifdef CONTENT_TIMESTAMP
void drawTimestamp(String &filename, JsonObject &cfgobj, tagRecord *&taginfo, imgParam &imageParams) {
    Serial.println("make Timestamp");
    time_t now;
    time(&now);
    struct tm timeinfo;

    JsonDocument loc;
    getTemplate(loc, 1, taginfo->hwType);

    TFT_eSprite spr = TFT_eSprite(&tft);
    initSprite(spr, imageParams.width, imageParams.height, imageParams);

    if (!cfgobj["#init"]) {
        Serial.println("init");
        // init preload images

        char hexmac[17];
        mac2hex(taginfo->mac, hexmac);

        String filename2 = "/temp/" + String(hexmac) + "-2.raw";
        drawString(spr, cfgobj["button1"].as<String>(), spr.width() / 2, 40, "calibrib30.vlw", TC_DATUM, TFT_BLACK);
        drawString(spr, "Well done!", spr.width() / 2, 90, "calibrib30.vlw", TC_DATUM, TFT_BLACK);
        spr2buffer(spr, filename2, imageParams);

        if (imageParams.zlib) {
            imageParams.dataType = DATATYPE_IMG_ZLIB;
        } else if (imageParams.g5) {
            imageParams.dataType = DATATYPE_IMG_G5;
        }

        struct imageDataTypeArgStruct arg = {0};
        arg.preloadImage = 1;
        arg.specialType = 17;  // button 2
        arg.lut = 0;

        prepareDataAvail(filename2, imageParams.dataType, *((uint8_t *)&arg), taginfo->mac, 5 | 0x8000);

        spr.fillRect(0, 0, spr.width(), spr.height(), TFT_WHITE);

        filename2 = "/temp/" + String(hexmac) + "-3.raw";
        drawString(spr, cfgobj["button2"].as<String>(), spr.width() / 2, 40, "calibrib30.vlw", TC_DATUM, TFT_BLACK);
        drawString(spr, "Well done!", spr.width() / 2, 90, "calibrib30.vlw", TC_DATUM, TFT_BLACK);
        spr2buffer(spr, filename2, imageParams);

        arg.preloadImage = 1;
        arg.specialType = 16;  // button 1
        arg.lut = 0;
        prepareDataAvail(filename2, imageParams.dataType, *((uint8_t *)&arg), taginfo->mac, 5 | 0x8000);

        cfgobj["#init"] = "1";
    }
    uint8_t offsety = 0;
    uint8_t buttony = 145;
    spr.fillRect(0, 0, spr.width(), spr.height(), TFT_WHITE);
    if (imageParams.rotate == 2) {
        offsety = 25;
        buttony = 10;
        spr.fillTriangle(spr.width() - 27, spr.height() - 160, spr.width() - 37, spr.height() - 160, spr.width() - 32, spr.height() - 165, TFT_BLACK);
        spr.fillTriangle(spr.width() - 127, spr.height() - 160, spr.width() - 117, spr.height() - 160, spr.width() - 122, spr.height() - 165, TFT_BLACK);
        drawString(spr, cfgobj["button1"], spr.width() - 32, buttony, "calibrib16.vlw", TC_DATUM, TFT_BLACK);
        drawString(spr, cfgobj["button2"], spr.width() - 122, buttony, "calibrib16.vlw", TC_DATUM, TFT_BLACK);
    } else {
        spr.fillTriangle(27, 160, 37, 160, 32, 165, TFT_BLACK);
        spr.fillTriangle(127, 160, 117, 160, 122, 165, TFT_BLACK);
        drawString(spr, cfgobj["button1"], 32, buttony, "calibrib16.vlw", TC_DATUM, TFT_BLACK);
        drawString(spr, cfgobj["button2"], 122, buttony, "calibrib16.vlw", TC_DATUM, TFT_BLACK);
    }
    drawString(spr, cfgobj["title"], spr.width() / 2, 10 + offsety, "calibrib30.vlw", TC_DATUM, TFT_BLACK);
    spr.drawLine(0, 40 + offsety, spr.width(), 40 + offsety, TFT_BLACK);

    uint8_t mode = cfgobj["mode"].as<int>();
    switch (taginfo->wakeupReason) {
        case WAKEUP_REASON_BUTTON2:
            Serial.println("button 1");
            cfgobj["last1"] = now;
            if (mode == 0) {
                // 1 timestamp
                cfgobj["last2"] = cfgobj["button1"].as<String>();
            }
            break;
        case WAKEUP_REASON_BUTTON1:
            Serial.println("button 2");
            if (mode == 0) {
                // 1 timestamp
                cfgobj["last1"] = now;
                cfgobj["last2"] = cfgobj["button2"].as<String>();
            } else {
                cfgobj["last2"] = now;
                // 2 timestamps
            }
            break;
    }

    char dateString1[40];
    uint32_t nextaction = cfgobj["nextaction"].as<uint32_t>();
    String dateformat = languageDateFormat[0] + " %H:%M";
    time_t timestamp = cfgobj["last1"].as<uint32_t>();
    localtime_r(&timestamp, &timeinfo);
    strftime(dateString1, sizeof(dateString1), dateformat.c_str(), &timeinfo);
    if (timestamp == 0) strcpy(dateString1, "never");

    if (mode == 0) {
        drawString(spr, "last:", 10, 50 + offsety, "calibrib16.vlw", TL_DATUM, TFT_BLACK);
        drawString(spr, dateString1, spr.width() / 2, 50 + offsety, "bahnschrift30.vlw", TC_DATUM, TFT_BLACK);
        drawString(spr, cfgobj["last2"].as<String>(), spr.width() / 2, 80 + offsety, "bahnschrift30.vlw", TC_DATUM, TFT_BLACK);

        if (nextaction > 0 && timestamp > 0) {
            timestamp += nextaction * 24 * 3600;
            if (timestamp < taginfo->nextupdate) taginfo->nextupdate = timestamp;
            localtime_r(&timestamp, &timeinfo);
            strftime(dateString1, sizeof(dateString1), languageDateFormat[0].c_str(), &timeinfo);

            drawString(spr, "next:", 10, 115 + offsety, "calibrib16.vlw", TL_DATUM, TFT_BLACK);
            drawString(spr, dateString1, 50, 115 + offsety, "calibrib16.vlw", TL_DATUM, timestamp < now ? imageParams.highlightColor : TFT_BLACK);
        }
    } else {
        drawString(spr, cfgobj["button1"].as<String>(), 10, 50 + offsety, "calibrib16.vlw", TL_DATUM, TFT_BLACK);
        drawString(spr, dateString1, 20, 67 + offsety, "fonts/bahnschrift20", TL_DATUM, TFT_BLACK);

        if (nextaction > 0 && timestamp > 0) {
            timestamp += nextaction * 24 * 3600;
            if (timestamp < taginfo->nextupdate) taginfo->nextupdate = timestamp;
            localtime_r(&timestamp, &timeinfo);
            strftime(dateString1, sizeof(dateString1), languageDateFormat[0].c_str(), &timeinfo);

            drawString(spr, "next", 200, 50 + offsety, "calibrib16.vlw", TL_DATUM, TFT_BLACK);
            drawString(spr, dateString1, 210, 67 + offsety, "fonts/bahnschrift20", TL_DATUM, timestamp < now ? imageParams.highlightColor : TFT_BLACK);
        }

        char dateString2[40];
        time_t timestamp = cfgobj["last2"].as<uint32_t>();
        localtime_r(&timestamp, &timeinfo);
        strftime(dateString2, sizeof(dateString2), dateformat.c_str(), &timeinfo);
        if (timestamp == 0) strcpy(dateString2, "never");

        drawString(spr, cfgobj["button2"].as<String>(), 10, 90 + offsety, "calibrib16.vlw", TL_DATUM, TFT_BLACK);
        drawString(spr, dateString2, 20, 107 + offsety, "fonts/bahnschrift20", TL_DATUM, TFT_BLACK);

        if (nextaction > 0 && timestamp > 0) {
            timestamp += nextaction * 24 * 3600;
            localtime_r(&timestamp, &timeinfo);
            strftime(dateString2, sizeof(dateString2), languageDateFormat[0].c_str(), &timeinfo);

            drawString(spr, "next", 200, 90 + offsety, "calibrib16.vlw", TL_DATUM, TFT_BLACK);
            drawString(spr, dateString2, 210, 107 + offsety, "fonts/bahnschrift20", TL_DATUM, timestamp < now ? imageParams.highlightColor : TFT_BLACK);
        }
    }

    spr2buffer(spr, filename, imageParams);
    spr.deleteSprite();
}
#endif
