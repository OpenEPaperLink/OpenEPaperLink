// Content: AP info.
// Included once by contentmanager.cpp, not compiled on its own.

void drawAPinfo(String &filename, JsonObject &cfgobj, tagRecord *&taginfo, imgParam &imageParams) {
    if (taginfo->hwType == SOLUM_SEG_UK) {
        imageParams.symbols = 0x00;
        sprintf(imageParams.segments, "");
        return;
    }

    TFT_eSprite spr = TFT_eSprite(&tft);
    JsonDocument loc;
    uint8_t screenCurrentOrientation = 0;
    getTemplate(loc, 21, taginfo->hwType);

    initSprite(spr, imageParams.width, imageParams.height, imageParams);
    const JsonArray jsonArray = loc.as<JsonArray>();
    for (const JsonVariant &elem : jsonArray) {
        drawElement(elem, spr, imageParams, screenCurrentOrientation);
    }

    spr2buffer(spr, filename, imageParams);
    spr.deleteSprite();
}
