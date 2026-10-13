// Content: QR code.
// Included once by contentmanager.cpp, not compiled on its own.

#ifdef CONTENT_QR
void drawQR(String &filename, String qrcontent, String title, tagRecord *&taginfo, imgParam &imageParams) {
    TFT_eSprite spr = TFT_eSprite(&tft);

    const char *text = qrcontent.c_str();
    QRCode qrcode;
    uint8_t version = findFittingVersion_text(ECC_MEDIUM, text);
    uint8_t qrcodeData[qrcode_getBufferSize(version)];
    // https://github.com/ricmoo/QRCode
    qrcode_initText(&qrcode, qrcodeData, version, ECC_MEDIUM, text);

    JsonDocument loc;
    getTemplate(loc, 10, taginfo->hwType);
    initSprite(spr, imageParams.width, imageParams.height, imageParams);
    drawString(spr, title, loc["title"][0], loc["title"][1], loc["title"][2], TC_DATUM, TFT_BLACK, loc["title"][3]);

    const int size = qrcode.size;
    const int dotsize = int((imageParams.height - loc["pos"][1].as<int>()) / size);
    const int xpos = loc["pos"][0].as<int>() - dotsize * size / 2;
    const int ypos = loc["pos"][1].as<int>() + (imageParams.height - loc["pos"][1].as<int>() - dotsize * size) / 2;

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            if (qrcode_getModule(&qrcode, x, y)) {
                spr.fillRect(xpos + x * dotsize, ypos + y * dotsize, dotsize, dotsize, TFT_BLACK);
            }
        }
    }

    spr2buffer(spr, filename, imageParams);
    spr.deleteSprite();
}
#endif
