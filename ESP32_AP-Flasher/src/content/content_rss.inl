// Content: RSS feed.
// Included once by contentmanager.cpp, not compiled on its own.

#ifdef CONTENT_RSS
rssClass reader;
#endif

void replaceHTMLentities(String &text) {
    text.replace("&gt;", ">");
    text.replace("&lt;", "<");
    text.replace("&quot;", "\"");
    text.replace("&apos;", "'");
    text.replace("&amp;", "&");
}

void removeHTML(String &text) {
    int len = text.length();
    bool insideTag = false;
    int j = 0;
    for (int i = 0; i < len; ++i) {
        char c = text[i];
        if (c == '<') {
            insideTag = true;
        } else if (c == '>') {
            insideTag = false;
        } else if (!insideTag) {
            text[j++] = c;
        }
    }
    text.remove(j);
}

void stampTime(TFT_eSprite &spr) {
    time_t now;
    time(&now);
    char timeStr[24];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d  %H:%M:%S ", localtime(&now));
    drawString(spr, timeStr, spr.width() - 1, 12, "glasstown_nbp_tf", TR_DATUM, TFT_BLACK);
}

#ifdef CONTENT_RSS
bool getRssFeed(String &filename, String URL, String title, tagRecord *&taginfo, imgParam &imageParams) {
    // https://github.com/garretlab/shoddyxml2

    // http://feeds.feedburner.com/tweakers/nieuws
    // https://www.nu.nl/rss/Algemeen

    wsLog("get rss feed");

    const char *url = URL.c_str();
    const int rssTitleSize = 255;
    const int rssDescSize = 1000;

    TFT_eSprite spr = TFT_eSprite(&tft);

    JsonDocument loc;
    getTemplate(loc, 9, taginfo->hwType);
    initSprite(spr, imageParams.width, imageParams.height, imageParams);
    // stampTime(spr);

    if (util::isEmptyOrNull(title)) title = "RSS feed";
    drawString(spr, title, loc["title"][0], loc["title"][1], loc["title"][2], TL_DATUM, TFT_BLACK, loc["title"][3]);
    int16_t posx = loc["line"][0];
    int16_t posy = loc["line"][1];
    int n = reader.getArticles(url, rssTitleSize, rssDescSize, loc["items"]);

    float lineheight = loc["desc"][3].as<float>();
    for (int i = 0; i < n; i++) {
        // if (reader.titleData[i] != NULL && *reader.titleData[i] != NULL) {
        if (reader.titleData[i] != NULL) {
            String title = String(reader.titleData[i]);
            replaceHTMLentities(title);
            removeHTML(title);

            if (!util::isEmptyOrNull(title)) {
                drawTextBox(spr, title, posx, posy, imageParams.width - 2 * posx, 100, loc["line"][2], TFT_BLACK);
            }
        }
        // if (reader.descData[i] != NULL && *reader.descData[i] != NULL) {
        if (reader.descData[i] != NULL && loc["desc"][2] != "") {
            String desc = String(reader.descData[i]);
            replaceHTMLentities(desc);
            removeHTML(desc);

            if (!util::isEmptyOrNull(desc)) {
                posy += loc["desc"][0].as<int>();
                drawTextBox(spr, desc, posx, posy, imageParams.width - 2 * posx, 100, loc["desc"][2], TFT_BLACK, TFT_WHITE, lineheight);
                posy += loc["desc"][1].as<int>();
            }
        } else {
            posy += loc["desc"][1].as<int>();
        }
    }
    reader.clearItemData();

    spr2buffer(spr, filename, imageParams);
    spr.deleteSprite();

    return true;
}
#endif
