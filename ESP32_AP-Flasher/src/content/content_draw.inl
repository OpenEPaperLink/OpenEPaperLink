// Text and sprite helpers for all content types: fonts (cached per render), drawString, drawTextBox.
// Included once by contentmanager.cpp, not compiled on its own.

uint8_t processFontPath(String &font) {
    if (font == "") return 3;
    if (font == "glasstown_nbp_tf") return 1;
    if (font == "7x14_tf") return 1;
    if (font == "t0_14b_tf") return 1;
    if (font.indexOf('/') == -1) font = "/fonts/" + font;
    if (!font.startsWith("/")) font = "/" + font;
    if (font.endsWith(".vlw")) font = font.substring(0, font.length() - 4);
    if (font.endsWith(".ttf")) return 2;
    return 3;
}

void replaceVariables(String &format) {
    size_t startIndex = 0;
    size_t openBraceIndex, closeBraceIndex;

    time_t now;
    time(&now);
    struct tm timedef;
    localtime_r(&now, &timedef);
    char timeBuffer[80];
    strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", &timedef);
    setVarDB("ap_time", timeBuffer, false);

    while ((openBraceIndex = format.indexOf('{', startIndex)) != -1 &&
           (closeBraceIndex = format.indexOf('}', openBraceIndex + 1)) != -1) {
        const std::string variableName = format.substring(openBraceIndex + 1, closeBraceIndex).c_str();
        const std::string varKey = "{" + variableName + "}";
        const auto var = varDB.find(variableName);
        if (var != varDB.end()) {
            format.replace(varKey.c_str(), var->second.value);
        } else {
            format.replace(varKey.c_str(), "-");
        }
        startIndex = closeBraceIndex + 1;
    }
}

// Font files read into memory during one drawNew(), freed by clearFontCache() at its end. A forecast draws
// ~30 strings; decompressing a .vlw.gz (~100 ms) or opening a .ttf (~60 ms) for each of them took seconds.
// Only drawNew() (the loop task) draws, so the cache needs no lock.
struct CachedFont {
    uint8_t *data;
    size_t size;
};
static std::map<String, CachedFont> fontCache;

static void clearFontCache() {
    for (auto &font : fontCache) free(font.second.data);
    fontCache.clear();
}

/// @brief A font file in memory, decompressed for a .gz; nullptr if it can't be read
static uint8_t *getCachedFont(const String &path, bool gzip, size_t &size) {
    auto it = fontCache.find(path);
    if (it != fontCache.end()) {
        size = it->second.size;
        return it->second.data;
    }
    uint8_t *data = nullptr;
    if (gzip) {
        data = readGzipFile(*contentFS, path, size);
    } else {
#ifdef BOARD_HAS_PSRAM
        File file = contentFS->open(path, "r");
        if (file) {
            size = file.size();
            data = (uint8_t *)ps_malloc(size);
            if (data && file.read(data, size) != size) {
                free(data);
                data = nullptr;
            }
            file.close();
        }
#endif
    }
    if (data) fontCache[path] = {data, size};
    return data;
}

/// @brief Load a smooth font, font like "/fonts/calibrib30"
/// @note Uses <font>.vlw, or else decompresses <font>.vlw.gz into memory (#464): vlw files store a byte per pixel
/// and compress very well, so a gzipped font saves a lot of flash
static void loadVlwFont(TFT_eSprite &spr, const String &font) {
    const String gzFilename = font + ".vlw.gz";
    // a cached font was already checked: exists() costs ~20 ms on LittleFS
    if (fontCache.find(gzFilename) == fontCache.end() && (contentFS->exists(font + ".vlw") || !contentFS->exists(gzFilename))) {
        spr.loadFont(font.substring(1), *contentFS);
        return;
    }
    size_t size;
    uint8_t *fontData = getCachedFont(gzFilename, true, size);
    if (fontData == nullptr) {
        wsErr("Can't load font " + gzFilename);
        return;
    }
    spr.loadFont(fontData);
}

static void unloadVlwFont(TFT_eSprite &spr) {
    spr.unloadFont();
}

/// @brief Set up truetype to draw into spr, the font from memory where possible (PSRAM): no file reads per glyph
static bool beginTruetype(TinyTTF &truetype, TFT_eSprite &spr, const String &font, uint16_t size, uint16_t color) {
    truetype.setFramebuffer(spr.width(), spr.height(), spr.getColorDepth(), static_cast<uint8_t *>(spr.getPointer()));
    size_t fontSize;
    uint8_t *fontData = getCachedFont(font, false, fontSize);
    const bool loaded = fontData ? truetype.setTtfPointer(fontData, fontSize, 0, false) : truetype.setTtfFile(contentFS->open(font, "r"));
    if (!loaded) {
        Serial.println("read ttf failed");
        truetype.end();
        return false;
    }
    truetype.setCharacterSize(size);
    truetype.setCharacterSpacing(0);
    if (spr.getColorDepth() == 8) {
        truetype.setTextColor(spr.color16to8(color), spr.color16to8(color));
    } else {
        truetype.setTextColor(color, color);
    }
    return true;
}

/// @brief Word wrap content into lines of at most boxwidth (by width(text)), drawLine(text, y) for each line
/// that fits in boxheight; posy ends below the last line
template <typename Width, typename DrawLine>
static void wrapText(const String &content, int16_t &posy, int16_t boxwidth, int16_t boxheight, int16_t lineAdvance, float lineheight, Width width, DrawLine drawLine) {
    const int length = content.length();
    int startPos = 0;
    const int startPosY = posy;

    while (startPos < length && posy + lineAdvance <= startPosY + boxheight) {
        int endPos = startPos;
        bool hasspace = false;

        while (endPos < length && width(content.substring(startPos, endPos + 1)) <= boxwidth && content.charAt(endPos) != '\n') {
            if (content.charAt(endPos) == ' ' || content.charAt(endPos) == '-') hasspace = true;
            endPos++;
        }
        while (endPos < length && endPos > startPos && hasspace == true && content.charAt(endPos - 1) != ' ' && content.charAt(endPos - 1) != '-' && content.charAt(endPos) != '\n') {
            endPos--;
        }
        drawLine(content.substring(startPos, endPos), posy);
        posy += lineAdvance * lineheight;

        if (content.charAt(endPos) == '\n') endPos++;
        startPos = endPos;
        while (startPos < length && content.charAt(startPos) == ' ') {
            startPos++;
        }
    }
}

void drawString(TFT_eSprite &spr, String content, int16_t posx, int16_t posy, String font, byte align, uint16_t color, uint16_t size, uint16_t bgcolor) {
    // drawString(spr,"test",100,10,"bahnschrift30",TC_DATUM,TFT_RED);

    // backwards compitibility
    replaceVariables(content);
    if (font.startsWith("fonts/calibrib")) {
        String numericValueStr = font.substring(14);
        int calibriSize = numericValueStr.toInt();
        if (calibriSize != 30 && calibriSize != 16) {
            font = "Signika-SB.ttf";
            size = calibriSize;
        }
    }
    if (font == "glasstown_nbp_tf") {
        font = "tahoma9.vlw";
        posy -= 8;
    }
    if (font == "7x14_tf") {
        font = "REFSAN12.vlw";
        posy -= 10;
    }
    if (font == "t0_14b_tf") {
        font = "calibrib16.vlw";
        posy -= 11;
    }

    switch (processFontPath(font)) {
        case 2: {
            // truetype
            TinyTTF truetype;
            if (!beginTruetype(truetype, spr, font, size, color)) return;
            if (align == TC_DATUM) {
                posx -= truetype.getStringWidth(content) / 2;
            }
            if (align == TR_DATUM) {
                posx -= truetype.getStringWidth(content);
            }
            truetype.setTextBoundary(posx, spr.width(), spr.height());
            truetype.textDraw(posx, posy, content);
            truetype.end();
        } break;
        case 3: {
            // vlw bitmap font
            spr.setTextDatum(align);
            if (font != "") loadVlwFont(spr, font);
            spr.setTextColor(color, bgcolor);
            spr.setTextWrap(false, false);
            spr.drawString(content, posx, posy);
            if (font != "") unloadVlwFont(spr);
        }
    }
}

void drawTextBox(TFT_eSprite &spr, String &content, int16_t &posx, int16_t &posy, int16_t boxwidth, int16_t boxheight, String font, uint16_t color, uint16_t bgcolor, float lineheight, byte align, uint16_t size) {
    replaceVariables(content);
    switch (processFontPath(font)) {
        case 2: {
            // truetype: size in pixels (default 20), lines size * lineheight apart
            if (size == 0) size = 20;
            TinyTTF truetype;
            if (!beginTruetype(truetype, spr, font, size, color)) return;
            wrapText(
                content, posy, boxwidth, boxheight, size, lineheight, [&](const String &text) { return truetype.getStringWidth(text); },
                [&](const String &line, int16_t y) {
                    int16_t x = posx;
                    if (align == TC_DATUM) x -= truetype.getStringWidth(line) / 2;
                    if (align == TR_DATUM) x -= truetype.getStringWidth(line);
                    truetype.setTextBoundary(x, spr.width(), spr.height());
                    truetype.textDraw(x, y, line);
                });
            truetype.end();
        } break;
        case 3: {
            // vlw bitmap font
            // spr.drawRect(posx, posy, boxwidth, boxheight, TFT_BLACK);
            spr.setTextDatum(align);
            if (font != "") loadVlwFont(spr, font);
            spr.setTextWrap(false, false);
            spr.setTextColor(color, bgcolor);
            wrapText(
                content, posy, boxwidth, boxheight, spr.gFont.yAdvance, lineheight, [&](const String &text) { return spr.textWidth(text.c_str()); },
                [&](const String &line, int16_t y) { spr.drawString(line, posx, y); });
            if (font != "") unloadVlwFont(spr);
        }
    }
}

void initSprite(TFT_eSprite &spr, int w, int h, imgParam &imageParams) {
    spr.setColorDepth(16);
    spr.createSprite(w, h);
    if (spr.getPointer() == nullptr) {
        wsErr("low on memory. Fallback to 8bpp");
        util::printLargestFreeBlock();
        spr.setColorDepth(8);
        spr.createSprite(w, h);
    }
    if (spr.getPointer() == nullptr) {
        wsErr("low on memory. Fallback to 1bpp");
        util::printLargestFreeBlock();
        spr.setColorDepth(1);
        spr.setBitmapColor(TFT_WHITE, TFT_BLACK);
        imageParams.bufferbpp = 1;
        spr.createSprite(w, h);
    }
    if (spr.getPointer() == nullptr) {
        wsErr("Failed to create sprite");
    }
    spr.setRotation(3);
    spr.fillSprite(TFT_WHITE);
}

String utf8FromCodepoint(uint16_t cp) {
    char buf[4] = {0}; 
    if (cp < 0x80) {
        buf[0] = cp;
    } else if (cp < 0x800) {
        buf[0] = 0xC0 | (cp >> 6);
        buf[1] = 0x80 | (cp & 0x3F);
    } else {
        buf[0] = 0xE0 | ((cp >> 12) & 0x0F);
        buf[1] = 0x80 | ((cp >> 6) & 0x3F);
        buf[2] = 0x80 | (cp & 0x3F);
    }
    return String(buf);
}

uint16_t getColor(const String &color) {
    if (color == "0" || color == "white") return TFT_WHITE;
    if (color == "1" || color == "" || color == "black") return TFT_BLACK;
    if (color == "2" || color == "red") return TFT_RED;
    if (color == "3" || color == "yellow") return TFT_YELLOW;
    if (color == "4" || color == "lightgray") return 0xBDF7;
    if (color == "5" || color == "darkgray") return TFT_DARKGREY;
    if (color == "6" || color == "pink") return 0xFBCF;
    if (color == "7" || color == "brown") return 0x8400;
    if (color == "8" || color == "green") return TFT_GREEN;
    if (color == "9" || color == "blue") return TFT_BLUE;
    if (color == "10" || color == "orange") return 0xFBE0;
    uint16_t r, g, b;
    if (color.length() == 7 && color[0] == '#' &&
        sscanf(color.c_str(), "#%2hx%2hx%2hx", &r, &g, &b) == 3) {
        return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
    }
    return TFT_WHITE;
}
