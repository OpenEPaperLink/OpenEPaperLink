// Content: json templates (file or URL) and the drawing of their elements.
// Included once by contentmanager.cpp, not compiled on its own.

bool getJsonTemplateFile(String &filename, String jsonfile, tagRecord *&taginfo, imgParam &imageParams) {
    if (jsonfile.c_str()[0] != '/') {
        jsonfile = "/" + jsonfile;
    }
    File file = contentFS->open(jsonfile, "r");
    if (file) {
        drawJsonStream(file, filename, taginfo, imageParams);
        file.close();
        // contentFS->remove(jsonfile);
        return true;
    }
    return false;
}

/// @brief Extract a variable with the given path from the given json
/// @note Float and double values are rounded to 2 decimal places
/// @param json Json document
/// @param path Path in form of a.b.1.c
/// @return Value as string
String extractValueFromJson(JsonDocument &json, const String &path) {
    JsonVariant currentObj = json.as<JsonVariant>();
    char *segment = strtok(const_cast<char *>(path.c_str()), ".");

    while (segment != NULL) {
        if (currentObj.is<JsonObject>()) {
            currentObj = currentObj.as<JsonObject>()[segment];
        } else if (currentObj.is<JsonArray>()) {
            int index = atoi(segment);
            currentObj = currentObj.as<JsonArray>()[index];
        } else {
            Serial.printf("Invalid JSON structure at path segment: %s\r\n", segment);
            return "";
        }
        segment = strtok(NULL, ".");
    }

    if (!currentObj.is<int>() && currentObj.is<float>()) {
        return String(currentObj.as<float>(), 2);
    }

    return currentObj.as<String>();
}

/// @brief Replaces json placeholders ({.a.b.1.c}) with variables
class DataInterceptor : public Stream {
   private:
    /// @brief Stream being wrapped
    Stream &_stream;
    /// @brief Json containing variables
    JsonDocument &_variables;
    /// @brief Parsing buffer
    String _buffer;
    /// @brief Buffer size
    const size_t _bufferSize = 32;

   public:
    DataInterceptor(Stream &stream, JsonDocument &variables)
        : _stream(stream), _variables(variables) {
    }

    int available() override {
        return _buffer.length() + _stream.available();
    }

    int read() override {
        findAndReplace();

        if (_buffer.length() > 0) {
            const int data = _buffer[0];
            _buffer.remove(0, 1);
            return data;
        }

        return -1;  // No more data
    }

    int peek() override {
        findAndReplace();
        return _buffer.length() ? _buffer[0] : -1;
    }

    size_t write(uint8_t data) override {
        return _stream.write(data);
    }

   private:
    /// @brief Fill buffer, find and replace json variables
    void findAndReplace() {
        unsigned int len;
        while ((len = _buffer.length()) < _bufferSize) {
            const int data = _stream.read();
            if (data == -1) {
                break;  // No more data to read
            }
            _buffer += (char)data;
        }

        if (len < 4) {
            // There are no variables with less than 4 characters
            return;
        }

        int endIndex = findVar(_buffer, 0);
        if (endIndex == -1) {
            return;
        }

        const String varCleaned = _buffer.substring(1, endIndex - 1);
        String replacement = extractValueFromJson(_variables, varCleaned);

        // Check for operator and second variable
        if (endIndex + 3 < len) {
            const char op = _buffer[endIndex];
            if ((op == '*' || op == '/' || op == '+' || op == '-')) {
                const int endIndex2 = findVar(_buffer, endIndex + 1);
                if (endIndex2 != -1) {
                    const String var2Cleaned = _buffer.substring(endIndex + 2, endIndex2 - 1);
                    const float v2 = extractValueFromJson(_variables, var2Cleaned).toFloat();
                    endIndex = endIndex2;

                    if (op == '*') {
                        replacement = String(replacement.toFloat() * v2, 0);
                    } else if (op == '/') {
                        replacement = abs(v2) > 0.0f ? String(replacement.toFloat() / v2, 0) : "0";
                    } else if (op == '+') {
                        replacement = String(replacement.toFloat() + v2, 0);
                    } else if (op == '-') {
                        replacement = String(replacement.toFloat() - v2, 0);
                    }
                }
            }
        }

        _buffer = replacement + _buffer.substring(endIndex);
    }

    /// @brief Find a var at given start index
    /// @param buffer Buffer to search in
    /// @param index Index to look at
    /// @return Endindex
    int findVar(const String &buffer, const int index) {
        if (buffer[index] != '{' || buffer[index + 1] != '.') {
            return -1;
        }

        return buffer.indexOf("}", index + 2) + 1;
    }
};

bool getJsonTemplateFileExtractVariables(String &filename, String jsonfile, JsonDocument &variables, tagRecord *&taginfo, imgParam &imageParams) {
    if (jsonfile.c_str()[0] != '/') {
        jsonfile = "/" + jsonfile;
    }
    File file = contentFS->open(jsonfile, "r");
    if (file) {
        auto interceptor = DataInterceptor(file, variables);
        drawJsonStream(interceptor, filename, taginfo, imageParams);
        file.close();
        // contentFS->remove(jsonfile);
        return true;
    }
    return false;
}

int getJsonTemplateUrl(String &filename, String URL, time_t fetched, String MAC, tagRecord *&taginfo, imgParam &imageParams) {
    HTTPClient http;
    http.useHTTP10(true);
    logLine("http getJsonTemplateUrl " + URL);
    http.begin(URL);
    http.addHeader("If-Modified-Since", formatHttpDate(fetched));
    http.addHeader("X-ESL-MAC", MAC);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    http.setTimeout(20000);
    const int httpCode = http.GET();
    if (httpCode == 200) {
        drawJsonStream(http.getStream(), filename, taginfo, imageParams);
    } else {
        if (httpCode != 304) {
            wsErr("http " + URL + " status " + String(httpCode));
            logLineRateLimited(urlForLog(URL), "http error " + String(httpCode) + " " + urlForLog(URL));
        }
    }
    http.end();
    return httpCode;
}

void drawJsonStream(Stream &stream, String &filename, tagRecord *&taginfo, imgParam &imageParams) {
    TFT_eSprite spr = TFT_eSprite(&tft);
    initSprite(spr, imageParams.width, imageParams.height, imageParams);
    uint8_t screenCurrentOrientation = 0;
    JsonDocument doc;
    if (stream.find("[")) {
        do {
            DeserializationError error = deserializeJson(doc, stream);
            if (error) {
                wsErr("json error " + String(error.c_str()));
                break;
            } else {
                drawElement(doc.as<JsonObject>(), spr, imageParams, screenCurrentOrientation);
                doc.clear();
            }
        } while (stream.findUntil(",", "]"));
    }

    spr2buffer(spr, filename, imageParams);
    spr.deleteSprite();
}

void rotateBuffer(uint8_t rotation, uint8_t &currentOrientation, TFT_eSprite &spr, imgParam &imageParams) {
    rotation = rotation % 4;               // First of all, let's be sure that the rotation have a valid value (0, 1, 2 or 3)
    if (rotation != currentOrientation) {  // If we have a rotation to do, let's do it
        int stepToDo = (currentOrientation - rotation + 4) % 4;
        // 2: upside down
        // 3: 270° rotation
        // 1: 90° rotation

        if (abs(stepToDo) == 2) {                                        // If we have to do a 180° rotation:
            TFT_eSprite sprCpy = TFT_eSprite(&tft);                      // We create a new sprite that will act as a buffer
            initSprite(sprCpy, spr.width(), spr.height(), imageParams);  // initialisation of the new sprite
            spr.pushRotated(&sprCpy, 180, TFT_WHITE);                    // We fill the new sprite with the old one rotated by 180°
            spr.fillSprite(TFT_WHITE);                                   // We fill the old one in white as anything that's white will be ignored by the pushRotated function
            sprCpy.pushRotated(&spr, 0, TFT_WHITE);                      // We copy the buffer sprite to the main one
            sprCpy.deleteSprite();                                       // We delete the buffer sprite to avoid memory leak
        } else {
            int angle = (stepToDo == 3) ? 270 : 90;
            TFT_eSprite sprCpy = TFT_eSprite(&tft);
            initSprite(sprCpy, spr.height(), spr.width(), imageParams);
            spr.pushRotated(&sprCpy, angle, TFT_WHITE);
            spr.deleteSprite();
            initSprite(spr, sprCpy.width(), sprCpy.height(), imageParams);
            sprCpy.pushToSprite(&spr, 0, 0);
            sprCpy.deleteSprite();
            imageParams.rotatebuffer = 1 - (imageParams.rotatebuffer % 2);
        }
        currentOrientation = rotation;
    }
}

TFT_eSprite sprDraw = TFT_eSprite(&tft);
bool spr_draw(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
    sprDraw.pushImage(x, y, w, h, bitmap);
    return 1;
}
void drawElement(const JsonObject &element, TFT_eSprite &spr, imgParam &imageParams, uint8_t &currentOrientation) {
    if (element["text"].is<JsonArray>()) {
        const JsonArray &textArray = element["text"];
        const uint16_t align = textArray[5] | 0;
        const uint16_t size = textArray[6] | 0;
        const String bgcolorstr = textArray[7].as<String>();
        const uint16_t bgcolor = (bgcolorstr.length() > 0) ? getColor(bgcolorstr) : TFT_WHITE;
        drawString(spr, textArray[2], textArray[0].as<int>(), textArray[1].as<int>(), textArray[3], align, getColor(textArray[4]), size, bgcolor);
    } else if (element["textbox"].is<JsonArray>()) {
        // posx, posy, width, height, text, font, color, lineheight, align, size (truetype)
        const JsonArray &textArray = element["textbox"];
        float lineheight = textArray[7].as<float>();
        if (lineheight == 0) lineheight = 1;
        int16_t posx = textArray[0] | 0;
        int16_t posy = textArray[1] | 0;
        String text = textArray[4];
        const uint16_t align = textArray[8] | 0;
        const uint16_t size = textArray[9] | 0;
        drawTextBox(spr, text, posx, posy, textArray[2], textArray[3], textArray[5], getColor(textArray[6]), TFT_WHITE, lineheight, align, size);
    } else if (element["box"].is<JsonArray>()) {
        const JsonArray &boxArray = element["box"];
        spr.fillRect(boxArray[0].as<int>(), boxArray[1].as<int>(), boxArray[2].as<int>(), boxArray[3].as<int>(), getColor(boxArray[4]));
        if (boxArray.size()>=7) {
            for (int i=0; i < boxArray[6].as<int>(); i++) {
                spr.drawRect(boxArray[0].as<int>() + i, boxArray[1].as<int>() + i, boxArray[2].as<int>() - 2 * i, boxArray[3].as<int>() - 2 * i, getColor(boxArray[5]));
            }
        }
    } else if (element["rbox"].is<JsonArray>()) {
        const JsonArray &rboxArray = element["rbox"];
        spr.fillRoundRect(rboxArray[0].as<int>(), rboxArray[1].as<int>(), rboxArray[2].as<int>(), rboxArray[3].as<int>(), rboxArray[4].as<int>(), getColor(rboxArray[5]));
        if (rboxArray.size() >= 8) {
            for (int i = 0; i < rboxArray[7].as<int>(); i++) {
                spr.drawRoundRect(rboxArray[0].as<int>() + i, rboxArray[1].as<int>() + i, rboxArray[2].as<int>() - 2 * i, rboxArray[3].as<int>() - 2 * i, rboxArray[4].as<int>() - i / 1.41, getColor(rboxArray[6]));
                if (i > 0) {
                    spr.drawRoundRect(rboxArray[0].as<int>() + i - 1, rboxArray[1].as<int>() + i, rboxArray[2].as<int>() - 2 * i + 2, rboxArray[3].as<int>() - 2 * i, rboxArray[4].as<int>() - i / 1.41, getColor(rboxArray[6]));
                }
            }
        }
    } else if (element["line"].is<JsonArray>()) {
        const JsonArray &lineArray = element["line"];
        spr.drawLine(lineArray[0].as<int>(), lineArray[1].as<int>(), lineArray[2].as<int>(), lineArray[3].as<int>(), getColor(lineArray[4]));
    } else if (element["triangle"].is<JsonArray>()) {
        const JsonArray &lineArray = element["triangle"];
        spr.fillTriangle(lineArray[0].as<int>(), lineArray[1].as<int>(), lineArray[2].as<int>(), lineArray[3].as<int>(), lineArray[4].as<int>(), lineArray[5].as<int>(), getColor(lineArray[6]));
    } else if (element["circle"].is<JsonArray>()) {
        const JsonArray &circleArray = element["circle"];
        spr.fillCircle(circleArray[0].as<int>(), circleArray[1].as<int>(), circleArray[2].as<int>(), getColor(circleArray[3]));
        if (circleArray.size() >= 6) {
            for (int i = 0; i < circleArray[5].as<int>(); i++) {
                spr.drawCircle(circleArray[0].as<int>(), circleArray[1].as<int>(), circleArray[2].as<int>() - i, getColor(circleArray[4]));
                if (i > 0) {
                    spr.drawCircle(circleArray[0].as<int>(), circleArray[1].as<int>(), circleArray[2].as<int>() - i - 0.5, getColor(circleArray[4]));
                }
            }
        }
    } else if (element["image"].is<JsonArray>()) {
        const JsonArray &imgArray = element["image"];

        TJpgDec.setSwapBytes(true);
        TJpgDec.setJpgScale(1);
        TJpgDec.setCallback(spr_draw);
        uint16_t w = 0, h = 0;
        String filename = imgArray[0];
        if (filename[0] != '/') {
            filename = "/" + filename;
        }
        TJpgDec.getFsJpgSize(&w, &h, filename, *contentFS);
        if (w == 0 && h == 0) {
            wsErr("invalid jpg");
            return;
        }
        Serial.println("jpeg conversion " + String(w) + "x" + String(h));
        sprDraw.setColorDepth(16);
        sprDraw.createSprite(w, h);
        if (sprDraw.getPointer() == nullptr) {
            wsErr("Failed to create sprite in contentmanager");
        } else {
            TJpgDec.drawFsJpg(0, 0, filename, *contentFS);
            sprDraw.pushToSprite(&spr, imgArray[1].as<int>(), imgArray[2].as<int>());
            sprDraw.deleteSprite();
        }

    } else if (element["rotate"].is<uint8_t>()) {
        uint8_t rotation = element["rotate"].as<int>();
        rotateBuffer(rotation, currentOrientation, spr, imageParams);
    }
}
