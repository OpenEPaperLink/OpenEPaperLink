#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include <makeimage.h>

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);

void tftinit() {
    //tijdelijk: voorbeeld voor aanmaak van plaatje

    LittleFS.begin();
    long w = 296, h = 128;  // mag staand of liggend
    spr.createSprite(w, h);
    spr.setColorDepth(8);
    spr.fillSprite(TFT_WHITE);
    spr.setTextDatum(TC_DATUM);
    spr.loadFont("calibrib62", LittleFS);
    spr.setTextColor(TFT_RED, TFT_WHITE);
    spr.drawString("zondag", w / 2, 10);
    spr.loadFont("calibrib50", LittleFS);
    spr.setTextColor(TFT_BLACK, TFT_WHITE);
    spr.drawString("29 januari", w / 2, 73);
    spr.unloadFont();

    spr2grays(spr, w, h, "/testspr3.bmp");

    spr.deleteSprite();
}

bool spr_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
    spr.pushImage(x, y, w, h, bitmap);
    return 1;
}

void jpg2grays(String filein, String fileout) {
    TJpgDec.setJpgScale(1);
    TJpgDec.setCallback(spr_output);
    uint16_t w = 0, h = 0;
    TJpgDec.getFsJpgSize(&w, &h, filein);

    spr.createSprite(w, h);
    spr.setColorDepth(8);
    spr.fillSprite(TFT_WHITE);
    TJpgDec.drawFsJpg(0, 0, filein);

    spr2grays(spr, w, h, fileout);
    spr.deleteSprite();
}

static uint32_t repackPackedVals(uint32_t val, uint32_t pixelsPerPackedUnit, uint32_t packedMultiplyVal) {
    uint32_t ret = 0, i;
    for (i = 0; i < pixelsPerPackedUnit; i++) {
        ret = ret * packedMultiplyVal + val % packedMultiplyVal;
        val /= packedMultiplyVal;
    }
    return ret;
}

void spr2grays(TFT_eSprite &spr, long w, long h, String fileout) {
    // based on bmp2grays function by Dmitry.GR

    Serial.println("start writing BMP");
    long t = millis();
    LittleFS.begin();

    fs::File f_out = LittleFS.open(fileout, "w");

    uint32_t c, rowBytesOut, rowBytesIn, outBpp, i, numRows, pixelsPerPackedUnit = 1, packedMultiplyVal = 0x01000000, packedOutBpp = 0;
    uint32_t numGrays, extraColor = 0;
    struct BitmapFileHeader hdr;
    memset(&hdr, 0, sizeof(hdr));
    enum EinkClut clutType;
    uint8_t clut[256][3];
    bool dither = false, rotated = false;
    int skipBytes;

    clutType = EinkClutTwoBlacksAndRed;

    if (w > h) {
        hdr.width = h;
        hdr.height = w;
        rotated = true;
    } else {
        hdr.width = w;
        hdr.height = h;
    }
    hdr.bpp = 24;
    hdr.sig[0] = 'B';
    hdr.sig[1] = 'M';
    hdr.colorplanes = 1;

    switch (clutType) {
        case EinkClutTwoBlacks:
            numGrays = 2;
            outBpp = 1;
            break;

        case EinkClutTwoBlacksAndRed:
            extraColor = 0xff0000;
            numGrays = 2;
            outBpp = 2;
            break;

        case EinkClutFourBlacks:
            numGrays = 4;
            outBpp = 2;
            break;

        case EinkClutThreeBlacksAndRed:
            numGrays = 3;
            extraColor = 0xff0000;
            outBpp = 2;
            break;
    }

    packedOutBpp = outBpp;

    rowBytesIn = (hdr.width * hdr.bpp + 31) / 32 * 4;
    rowBytesOut = ((hdr.width + pixelsPerPackedUnit - 1) / pixelsPerPackedUnit) * packedOutBpp;
    rowBytesOut = (rowBytesOut + 31) / 32 * 4;

    numRows = hdr.height < 0 ? -hdr.height : hdr.height;
    hdr.bpp = outBpp;
    hdr.numColors = 1 << outBpp;
    hdr.numImportantColors = 1 << outBpp;
    hdr.dataOfst = sizeof(struct BitmapFileHeader) + 4 * hdr.numColors;
    hdr.dataLen = numRows * rowBytesOut;
    hdr.fileSz = hdr.dataOfst + hdr.dataLen;
    hdr.headerSz = 40;
    hdr.compression = 0;

    f_out.write((uint8_t *)&hdr, sizeof(hdr));

    // emit & record grey clut entries
    for (i = 0; i < numGrays; i++) {
        uint32_t val = 255 * i / (numGrays - 1);

        f_out.write(val);
        f_out.write(val);
        f_out.write(val);
        f_out.write(val);

        clut[i][0] = val;
        clut[i][1] = val;
        clut[i][2] = val;
    }

    if (extraColor) {
        f_out.write((extraColor >> 0) & 0xff);   // B
        f_out.write((extraColor >> 8) & 0xff);   // G
        f_out.write((extraColor >> 16) & 0xff);  // R
        f_out.write(0x00);                       // A

        clut[i][0] = (extraColor >> 0) & 0xff;
        clut[i][1] = (extraColor >> 8) & 0xff;
        clut[i][2] = (extraColor >> 16) & 0xff;
    }

    // pad clut to size
    for (i = numGrays + (extraColor ? 1 : 0); i < hdr.numColors; i++) {
        f_out.write(0x00);
        f_out.write(0x00);
        f_out.write(0x00);
        f_out.write(0x00);
    }

    while (numRows--) {
        uint32_t pixelValsPackedSoFar = 0, numPixelsPackedSoFar = 0, valSoFar = 0, bytesIn = 0, bytesOut = 0, bitsSoFar = 0;

        for (c = 0; c < hdr.width; c++, bytesIn += 3) {
            int64_t bestDist = 0x7fffffffffffffffll;
            uint8_t bestIdx = 0;
            int32_t ditherFudge = 0;
            uint16_t color565;
            if (rotated) {
                color565 = spr.readPixel(hdr.height - 1 - numRows, c);
            } else {
                color565 = spr.readPixel(c, numRows);
            }

            uint8_t red = ((color565 >> 11) & 0x1F) * 8;
            uint8_t green = ((color565 >> 5) & 0x3F) * 4;
            uint8_t blue = (color565 & 0x1F) * 8;

            if (dither)
                ditherFudge = (rand() % 255 - 127) / (int)numGrays;

            for (i = 0; i < hdr.numColors; i++) {
                int64_t dist = 0;

                dist += (blue - clut[i][0] + ditherFudge) * (blue - clut[i][0] + ditherFudge) * 4750ll;
                dist += (green - clut[i][1] + ditherFudge) * (green - clut[i][1] + ditherFudge) * 47055ll;
                dist += (red - clut[i][2] + ditherFudge) * (red - clut[i][2] + ditherFudge) * 13988ll;

                if (dist < bestDist) {
                    bestDist = dist;
                    bestIdx = i;
                }
            }

            // pack pixels as needed
            pixelValsPackedSoFar = pixelValsPackedSoFar * packedMultiplyVal + bestIdx;
            if (++numPixelsPackedSoFar != pixelsPerPackedUnit)
                continue;

            numPixelsPackedSoFar = 0;

            // it is easier to display when low val is first pixel. currently last pixel is low - reverse this
            pixelValsPackedSoFar = repackPackedVals(pixelValsPackedSoFar, pixelsPerPackedUnit, packedMultiplyVal);

            valSoFar = (valSoFar << packedOutBpp) | pixelValsPackedSoFar;
            pixelValsPackedSoFar = 0;
            bitsSoFar += packedOutBpp;

            if (bitsSoFar >= 8) {
                f_out.write(valSoFar >> (bitsSoFar -= 8));
                valSoFar &= (1 << bitsSoFar) - 1;
                bytesOut++;
            }
        }

        // see if we have unfinished pixel packages to write
        if (numPixelsPackedSoFar) {
            while (numPixelsPackedSoFar++ != pixelsPerPackedUnit)
                pixelValsPackedSoFar *= packedMultiplyVal;

            // it is easier to display when low val is first pixel. currently last pixel is low - reverse this
            pixelValsPackedSoFar = repackPackedVals(pixelValsPackedSoFar, pixelsPerPackedUnit, packedMultiplyVal);

            valSoFar = (valSoFar << packedOutBpp) | pixelValsPackedSoFar;
            pixelValsPackedSoFar = 0;
            bitsSoFar += packedOutBpp;

            if (bitsSoFar >= 8) {
                f_out.write(valSoFar >> (bitsSoFar -= 8));
                valSoFar &= (1 << bitsSoFar) - 1;
                bytesOut++;
            }
        }

        if (bitsSoFar) {
            valSoFar <<= 8 - bitsSoFar;  // left-align it as is expected
            f_out.write(valSoFar);
            bytesOut++;
        }

        while (bytesOut++ < rowBytesOut)
            f_out.write(0);
    }
    f_out.close();
    Serial.println(millis() - t);
    Serial.println("finished writing BMP");
}

void bmp2grays(String filein, String fileout) {
    // based on bmp2grays function by Dmitry.GR

    Serial.println("start writing BMP2");
    long t = millis();
    LittleFS.begin();

    fs::File f_in = LittleFS.open(filein, "r");
    fs::File f_out = LittleFS.open(fileout, "w");

    uint32_t c, rowBytesOut, rowBytesIn, outBpp, i, numRows, pixelsPerPackedUnit = 1, packedMultiplyVal = 0x01000000, packedOutBpp = 0;
    uint32_t numGrays, extraColor = 0;
    struct BitmapFileHeader hdr;
    enum EinkClut clutType;
    uint8_t clut[256][3];
    bool dither = false;
    int skipBytes;

    clutType = EinkClutTwoBlacksAndRed;

    f_in.read((uint8_t *)&hdr, sizeof(hdr));

    if (hdr.sig[0] != 'B' || hdr.sig[1] != 'M' || hdr.headerSz < 40 || hdr.colorplanes != 1 || hdr.bpp != 24 || hdr.compression) {
        Serial.println("BITMAP HEADER INVALID, use uncompressed 24 bits RGB");
        return;
    }

    switch (clutType) {
        case EinkClutTwoBlacks:
            numGrays = 2;
            outBpp = 1;
            break;

        case EinkClutTwoBlacksAndRed:
            extraColor = 0xff0000;
            numGrays = 2;
            outBpp = 2;
            break;

        case EinkClutFourBlacks:
            numGrays = 4;
            outBpp = 2;
            break;

        case EinkClutThreeBlacksAndRed:
            numGrays = 3;
            extraColor = 0xff0000;
            outBpp = 2;
            break;
    }

    packedOutBpp = outBpp;

    skipBytes = hdr.dataOfst - sizeof(hdr);
    if (skipBytes < 0) {
        fprintf(stderr, "file header was too short!\n");
        exit(-1);
    }
    f_in.read(NULL, skipBytes);

    rowBytesIn = (hdr.width * hdr.bpp + 31) / 32 * 4;
    rowBytesOut = ((hdr.width + pixelsPerPackedUnit - 1) / pixelsPerPackedUnit) * packedOutBpp;
    rowBytesOut = (rowBytesOut + 31) / 32 * 4;

    numRows = hdr.height < 0 ? -hdr.height : hdr.height;
    hdr.bpp = outBpp;
    hdr.numColors = 1 << outBpp;
    hdr.numImportantColors = 1 << outBpp;
    hdr.dataOfst = sizeof(struct BitmapFileHeader) + 4 * hdr.numColors;
    hdr.dataLen = numRows * rowBytesOut;
    hdr.fileSz = hdr.dataOfst + hdr.dataLen;
    hdr.headerSz = 40;
    hdr.compression = 0;

    f_out.write((uint8_t *)&hdr, sizeof(hdr));

    // emit & record grey clut entries
    for (i = 0; i < numGrays; i++) {
        uint32_t val = 255 * i / (numGrays - 1);

        f_out.write(val);
        f_out.write(val);
        f_out.write(val);
        f_out.write(val);

        clut[i][0] = val;
        clut[i][1] = val;
        clut[i][2] = val;
    }

    // if there is a color CLUT entry, emit that
    if (extraColor) {
        f_out.write((extraColor >> 0) & 0xff);   // B
        f_out.write((extraColor >> 8) & 0xff);   // G
        f_out.write((extraColor >> 16) & 0xff);  // R
        f_out.write(0x00);                       // A

        clut[i][0] = (extraColor >> 0) & 0xff;
        clut[i][1] = (extraColor >> 8) & 0xff;
        clut[i][2] = (extraColor >> 16) & 0xff;
    }

    // pad clut to size
    for (i = numGrays + (extraColor ? 1 : 0); i < hdr.numColors; i++) {
        f_out.write(0x00);
        f_out.write(0x00);
        f_out.write(0x00);
        f_out.write(0x00);
    }

    while (numRows--) {
        uint32_t pixelValsPackedSoFar = 0, numPixelsPackedSoFar = 0, valSoFar = 0, bytesIn = 0, bytesOut = 0, bitsSoFar = 0;

        for (c = 0; c < hdr.width; c++, bytesIn += 3) {
            int64_t bestDist = 0x7fffffffffffffffll;
            uint8_t rgb[3], bestIdx = 0;
            int32_t ditherFudge = 0;

            f_in.read(rgb, sizeof(rgb));

            if (dither)
                ditherFudge = (rand() % 255 - 127) / (int)numGrays;

            for (i = 0; i < hdr.numColors; i++) {
                int64_t dist = 0;

                dist += (rgb[0] - clut[i][0] + ditherFudge) * (rgb[0] - clut[i][0] + ditherFudge) * 4750ll;
                dist += (rgb[1] - clut[i][1] + ditherFudge) * (rgb[1] - clut[i][1] + ditherFudge) * 47055ll;
                dist += (rgb[2] - clut[i][2] + ditherFudge) * (rgb[2] - clut[i][2] + ditherFudge) * 13988ll;

                if (dist < bestDist) {
                    bestDist = dist;
                    bestIdx = i;
                }
            }

            // pack pixels as needed
            pixelValsPackedSoFar = pixelValsPackedSoFar * packedMultiplyVal + bestIdx;
            if (++numPixelsPackedSoFar != pixelsPerPackedUnit)
                continue;

            numPixelsPackedSoFar = 0;

            // it is easier to display when low val is first pixel. currently last pixel is low - reverse this
            pixelValsPackedSoFar = repackPackedVals(pixelValsPackedSoFar, pixelsPerPackedUnit, packedMultiplyVal);

            valSoFar = (valSoFar << packedOutBpp) | pixelValsPackedSoFar;
            pixelValsPackedSoFar = 0;
            bitsSoFar += packedOutBpp;

            if (bitsSoFar >= 8) {
                f_out.write(valSoFar >> (bitsSoFar -= 8));
                valSoFar &= (1 << bitsSoFar) - 1;
                bytesOut++;
            }
        }

        // see if we have unfinished pixel packages to write
        if (numPixelsPackedSoFar) {
            while (numPixelsPackedSoFar++ != pixelsPerPackedUnit)
                pixelValsPackedSoFar *= packedMultiplyVal;

            // it is easier to display when low val is first pixel. currently last pixel is low - reverse this
            pixelValsPackedSoFar = repackPackedVals(pixelValsPackedSoFar, pixelsPerPackedUnit, packedMultiplyVal);

            valSoFar = (valSoFar << packedOutBpp) | pixelValsPackedSoFar;
            pixelValsPackedSoFar = 0;
            bitsSoFar += packedOutBpp;

            if (bitsSoFar >= 8) {
                f_out.write(valSoFar >> (bitsSoFar -= 8));
                valSoFar &= (1 << bitsSoFar) - 1;
                bytesOut++;
            }
        }

        if (bitsSoFar) {
            valSoFar <<= 8 - bitsSoFar;  // left-align it as is expected
            f_out.write(valSoFar);
            bytesOut++;
        }

        while (bytesIn++ < rowBytesIn)
            f_in.read(NULL, 1);
        while (bytesOut++ < rowBytesOut)
            f_out.write(0);
    }
    f_in.close();
    f_out.close();
    Serial.println(millis() - t);
    Serial.println("finished writing BMP2");
}