#include "truetype.h"

#include <algorithm>

//Kerning is optional. Many fonts don't have kerning tables anyway.
//#define ENABLEKERNING

truetypeClass::truetypeClass() {}

void truetypeClass::end() {
    file.close();
    freePointsAll();
    freeGlyph();
    if (table != nullptr) free(table);
}

uint8_t truetypeClass::setTtfFile(File _file, uint8_t _checkCheckSum) {
    if (_file == 0) {
        return 0;
    }

    file = _file;
    if (readTableDirectory(_checkCheckSum) == 0) {
        file.close();
        return 0;
    }

    if (readCmap() == 0) {
        file.close();
        return 0;
    }

    if (readHMetric() == 0) {
        file.close();
        return 0;
    }
    iBufferedBytes = 0;
#ifdef ENABLEKERNING
    readKern();
#endif
    readHeadTable();
    readHhea();
    return 1;
}

void truetypeClass::setTtfDrawPixel(TTF_DRAWPIXEL *p) {
    pfnDrawPixel = p;
}

uint8_t truetypeClass::setTtfPointer(uint8_t *p, uint32_t u32Size, uint8_t _checkCheckSum, bool bF) {
    pTTF = p;
    u32TTFSize = u32Size;
    bFlash = bF;

    if (readTableDirectory(_checkCheckSum) == 0) {
        file.close();
        return 0;
    }

    if (readCmap() == 0) {
        file.close();
        return 0;
    }

    if (readHMetric() == 0) {
        file.close();
        return 0;
    }

#ifdef ENABLEKERNING
    readKern();
#endif
    readHeadTable();
    readHhea();
    return 1;

} 

int truetypeClass::ttfRead(uint8_t *d, int iLen) {
    if (!pTTF) {
        //return file.read(d, iLen);
        int totalBytesRead = 0;

        while (iLen > 0) {
            if (iBufferedBytes == 0) {
                u32BufStart = file.position();
                iBufferedBytes = file.read(u8FileBuf, FILE_BUF_SIZE);
                iCurrentBufSize = iBufferedBytes;
                if (iBufferedBytes <= 0) break;
                u32BufPosition = 0;
            }

            int bytesToCopy = min(iLen, iBufferedBytes);
            memcpy(d, u8FileBuf + u32BufPosition, bytesToCopy);

            d += bytesToCopy;
            iLen -= bytesToCopy;
            iBufferedBytes -= bytesToCopy;
            u32BufPosition += bytesToCopy;
            totalBytesRead += bytesToCopy;
        }
        return totalBytesRead;
    } else {

        if (u32TTFOffset + iLen > u32TTFSize) {
            iLen = u32TTFSize - u32TTFOffset;
        }
        if (bFlash) {
            memcpy_P(d, &pTTF[u32TTFOffset], iLen);
        } else {
            memcpy(d, &pTTF[u32TTFOffset], iLen);
        }
        u32TTFOffset += iLen;
        return iLen;

    }
    return 0;
} /* ttfRead() */

void truetypeClass::ttfSeek(uint32_t u32Offset) {
    if (!pTTF) {
        // within the loaded buffer: only move in the buffer (the file position stays at its end)
        if (iCurrentBufSize > 0 && u32Offset >= u32BufStart && u32Offset < u32BufStart + iCurrentBufSize) {
            u32BufPosition = u32Offset - u32BufStart;
            iBufferedBytes = iCurrentBufSize - u32BufPosition;
        } else {
            file.seek(u32Offset);
            iBufferedBytes = 0;
            iCurrentBufSize = 0;
        }
    } else {
        if (u32Offset > u32TTFSize) {
            u32Offset = u32TTFSize;
        }
        u32TTFOffset = u32Offset;
    }
} /* ttfSeek() */

uint32_t truetypeClass::ttfPosition(void) {
    if (!pTTF) {
        return file.position() - iBufferedBytes;
    } else {
        return u32TTFOffset;
    }
} /* ttfPosition() */

void truetypeClass::setFramebuffer(uint16_t _framebufferWidth, uint16_t _framebufferHeight, uint16_t _framebuffer_bit, uint8_t *_framebuffer) {
    displayWidth = _framebufferWidth;
    displayHeight = _framebufferHeight;
    framebufferBit = _framebuffer_bit;
    userFrameBuffer = _framebuffer;

    switch (framebufferBit) {
        case 16:  // 16bit horizontal
            displayWidthFrame = displayWidth * 2;
            break;
        case 8:  // 8bit Horizontal
            displayWidthFrame = displayWidth;
            break;
        case 4:  // 4bit Horizontal
            displayWidthFrame = (displayWidth + 1) / 2;
            break;
        case 1:  // 1bit Horizontal
        default:
            displayWidthFrame = (displayWidth + 7) / 8;
            break;
    }

    return;
}

void truetypeClass::setCharacterSize(uint16_t _characterSize) {
    characterSize = _characterSize;
}

void truetypeClass::setCharacterSpacing(int16_t _characterSpace, uint8_t _kerning) {
    characterSpace = _characterSpace;
    kerningOn = _kerning;
}

void truetypeClass::setTextBoundary(uint16_t _start_x, uint16_t _end_x, uint16_t _end_y) {
    start_x = _start_x;
    end_x = _end_x;
    end_y = _end_y;
}

void truetypeClass::setTextColor(uint16_t _onLine, uint16_t _inside) {
    colorLine = _onLine;
    colorInside = _inside;
}

void truetypeClass::setTextRotation(uint16_t _rotation) {
    switch (_rotation) {
        case ROTATE_90:
        case 90:
            _rotation = 1;
            break;
        case ROTATE_180:
        case 180:
            _rotation = 2;
            break;
        case ROTATE_270:
        case 270:
            _rotation = 3;
            break;
        default:
            _rotation = 0;
            break;
    }
    stringRotation = _rotation;
}

/* ----------------private---------------- */
/* calculate checksum */
uint32_t truetypeClass::calculateCheckSum(uint32_t offset, uint32_t length) {
    uint32_t checksum = 0L;

    length = (length + 3) / 4;
    ttfSeek(offset);

    while (length-- > 0) {
        checksum += getUInt32t();
    }
    return checksum;
}

/* read table directory */
int truetypeClass::readTableDirectory(int checkCheckSum) {
    ttfSeek(numTablesPos);
    numTables = getUInt16t();
    table = (ttTable_t *)malloc(sizeof(ttTable_t) * numTables);
    ttfSeek(tablePos);
    for (int i = 0; i < numTables; i++) {
        for (int j = 0; j < 4; j++) {
            table[i].name[j] = getUInt8t();
        }
        table[i].name[4] = '\0';
        table[i].checkSum = getUInt32t();
        table[i].offset = getUInt32t();
        table[i].length = getUInt32t();
    }

    if (checkCheckSum) {
        for (int i = 0; i < numTables; i++) {
            if (strcmp(table[i].name, "head") != 0) { /* checksum of "head" is invalid */
                uint32_t c = calculateCheckSum(table[i].offset, table[i].length);
                if (table[i].checkSum != c) {
                    return 0;
                }
            }
        }
    }
    return 1;
}

/* read head table */
void truetypeClass::readHeadTable() {
    for (int i = 0; i < numTables; i++) {
        if (strcmp(table[i].name, "head") == 0) {
            ttfSeek(table[i].offset);

            headTable.version = getUInt32t();
            headTable.revision = getUInt32t();
            headTable.checkSumAdjustment = getUInt32t();
            headTable.magicNumber = getUInt32t();
            headTable.flags = getUInt16t();
            headTable.unitsPerEm = getUInt16t();
            for (int j = 0; j < 8; j++) {
                headTable.created[j] = getUInt8t();
            }
            for (int j = 0; j < 8; j++) {
                headTable.modified[j] = getUInt8t();
            }
            xMin = headTable.xMin = getInt16t();
            yMin = headTable.yMin = getInt16t();
            xMax = headTable.xMax = getInt16t();
            yMax = headTable.yMax = getInt16t();
            headTable.macStyle = getUInt16t();
            headTable.lowestRecPPEM = getUInt16t();
            headTable.fontDirectionHint = getInt16t();
            headTable.indexToLocFormat = getInt16t();
            headTable.glyphDataFormat = getInt16t();
        }
    }
}

/* cmap */
/* read cmap format 4 */
uint8_t truetypeClass::readCmapFormat4() {
    ttfSeek(cmapFormat4.offset);
    if ((cmapFormat4.format = getUInt16t()) != 4) {
        return 0;
    }

    cmapFormat4.length = getUInt16t();
    cmapFormat4.language = getUInt16t();
    cmapFormat4.segCountX2 = getUInt16t();
    cmapFormat4.searchRange = getUInt16t();
    cmapFormat4.entrySelector = getUInt16t();
    cmapFormat4.rangeShift = getUInt16t();
    cmapFormat4.endCodeOffset = cmapFormat4.offset + 14;
    cmapFormat4.startCodeOffset = cmapFormat4.endCodeOffset + cmapFormat4.segCountX2 + 2;
    cmapFormat4.idDeltaOffset = cmapFormat4.startCodeOffset + cmapFormat4.segCountX2;
    cmapFormat4.idRangeOffsetOffset = cmapFormat4.idDeltaOffset + cmapFormat4.segCountX2;
    cmapFormat4.glyphIndexArrayOffset = cmapFormat4.idRangeOffsetOffset + cmapFormat4.segCountX2;

    return 1;
}

/* read cmap */
uint8_t truetypeClass::readCmap() {
    uint16_t platformId, platformSpecificId;
    uint32_t cmapOffset, tableOffset;
    uint8_t foundMap = 0;

    if ((cmapOffset = seekToTable("cmap")) == 0) {
        return 0;
    }

    cmapIndex.version = getUInt16t();
    cmapIndex.numberSubtables = getUInt16t();

    for (uint16_t i = 0; i < cmapIndex.numberSubtables; i++) {
        platformId = getUInt16t();
        platformSpecificId = getUInt16t();
        tableOffset = getUInt32t();
        if ((platformId == 3) && (platformSpecificId == 1)) {
            cmapFormat4.offset = cmapOffset + tableOffset;
            readCmapFormat4();
            foundMap = 1;
            break;
        }
    }

    if (foundMap == 0) {
        return 0;
    }

    return 1;
}

/* convert character code to glyph id */
uint16_t truetypeClass::codeToGlyphId(uint16_t _code) {
    uint16_t start, end, idRangeOffset;
    int16_t idDelta;
    uint8_t found = 0;
    uint16_t offset, glyphId;

    for (int i = 0; i < cmapFormat4.segCountX2 / 2; i++) {
        ttfSeek(cmapFormat4.endCodeOffset + 2 * i);
        end = getUInt16t();
        if (_code <= end) {
            ttfSeek(cmapFormat4.startCodeOffset + 2 * i);
            start = getUInt16t();
            if (_code >= start) {
                ttfSeek(cmapFormat4.idDeltaOffset + 2 * i);
                idDelta = getInt16t();
                ttfSeek(cmapFormat4.idRangeOffsetOffset + 2 * i);
                idRangeOffset = getUInt16t();
                if (idRangeOffset == 0) {
                    glyphId = (idDelta + _code) % 65536;
                } else {
                    offset = (idRangeOffset / 2 + i + _code - start - cmapFormat4.segCountX2 / 2) * 2;
                    ttfSeek(cmapFormat4.glyphIndexArrayOffset + offset);
                    glyphId = getUInt16t();
                }

                found = 1;
                break;
            }
        }
    }
    if (!found) {
        return 0;
    }
    return glyphId;
}

uint8_t truetypeClass::readHhea() {
    if (seekToTable("hhea") == 0) {
        ascender = yMax;
        return 0;
    }
    getUInt32t();
    ascender = getInt16t();
    // skip descender .. metricDataFormat (14 fields) to numberOfHMetrics, the last field at offset 34
    for (uint8_t i = 0; i < 14; i++) getUInt16t();
    numberOfHMetrics = getUInt16t();
    return 1;
}

#ifdef ENABLEKERNING
/* read kerning table */
uint8_t truetypeClass::readKern() {
    uint32_t nextTable;

    if (seekToTable("kern") == 0) {
        return 0;
    }
    kernHeader.nTables = getUInt32t();

    // only support up to 32 sub-tables
    if (kernHeader.nTables > 32) {
        kernHeader.nTables = 32;
    }

    for (uint8_t i = 0; i < kernHeader.nTables; i++) {
        uint16_t format;

        kernSubtable.length = getUInt32t();
        nextTable = ttfPosition() + kernSubtable.length;
        kernSubtable.coverage = getUInt16t();

        format = (uint16_t)(kernSubtable.coverage >> 8);

        // only support format0
        if (format != 0) {
            ttfSeek(nextTable);
            continue;
        }

        // only use horizontal kerning tables
        if ((kernSubtable.coverage & 0x0003) != 0x0001) {
            ttfSeek(nextTable);
            continue;
        }

        // format0
        kernFormat0.nPairs = getUInt16t();
        kernFormat0.searchRange = getUInt16t();
        kernFormat0.entrySelector = getUInt16t();
        kernFormat0.rangeShift = getUInt16t();
        kernTablePos = ttfPosition();

        break;
    }

    return 1;
}

int16_t truetypeClass::getKerning(uint16_t _left_glyph, uint16_t _right_glyph) {
    if (kernTablePos == 0) return 0;
    int16_t result = 0;
    uint32_t key0 = ((uint32_t)(_left_glyph) << 16) | (_right_glyph);
    ttfSeek(kernTablePos);
    for (uint16_t i = 0; i < kernFormat0.nPairs; i++) {
        uint32_t key1 = getUInt32t();
        if (key0 == key1) {
            result = getInt16t();
            break;
        }
        uint16_t dummy = getInt16t();
    }

    return result;
}
#endif

uint16_t truetypeClass::spaceAdvance(wchar_t _space) {
    const uint16_t id = codeToGlyphId(_space);
    if (id == 0) return characterSize / 4;
    return getHMetric(id).advanceWidth;
}

// hmtx. metric information for the horizontal layout each of the glyphs
uint8_t truetypeClass::readHMetric() {
    if (seekToTable("hmtx") == 0) {
        return 0;
    }

    hmtxTablePos = ttfPosition();
    return 1;
}

ttHMetric_t truetypeClass::getHMetric(uint16_t _code) {
    static ttHMetric_t result;
    result.advanceWidth = 0;

    if (numberOfHMetrics > 0 && _code >= numberOfHMetrics) {
        // glyphs after the last full entry use its advance width, and have only a leftSideBearing
        // (common in CJK fonts, where most glyphs have the same width)
        ttfSeek(hmtxTablePos + ((numberOfHMetrics - 1) * 4));
        result.advanceWidth = getUInt16t();
        ttfSeek(hmtxTablePos + (numberOfHMetrics * 4) + ((_code - numberOfHMetrics) * 2));
        result.leftSideBearing = getInt16t();
    } else {
        ttfSeek(hmtxTablePos + (_code * 4));
        result.advanceWidth = getUInt16t();
        result.leftSideBearing = getInt16t();
    }

    result.advanceWidth = (result.advanceWidth * characterSize) / headTable.unitsPerEm;
    result.leftSideBearing = (result.leftSideBearing * characterSize) / headTable.unitsPerEm;
    return result;
}

/* get glyph offset */
uint32_t truetypeClass::getGlyphOffset(uint16_t index) {
    uint32_t offset = 0;

    for (int i = 0; i < numTables; i++) {
        if (strcmp(table[i].name, "loca") == 0) {
            if (headTable.indexToLocFormat == 1) {
                ttfSeek(table[i].offset + index * 4);
                offset = getUInt32t();
            } else {
                ttfSeek(table[i].offset + index * 2);
                offset = getUInt16t() * 2;
            }
            break;
        }
    }

    for (int i = 0; i < numTables; i++) {
        if (strcmp(table[i].name, "glyf") == 0) {
            return (offset + table[i].offset);
        }
    }

    return 0;
}

/* read coords */
void truetypeClass::readCoords(char _xy, uint16_t _startPoint) {
    int16_t value = 0;
    uint8_t shortFlag, sameFlag;

    if (_xy == 'x') {
        shortFlag = FLAG_XSHORT;
        sameFlag = FLAG_XSAME;
    } else {
        shortFlag = FLAG_YSHORT;
        sameFlag = FLAG_YSAME;
    }

    for (uint16_t i = _startPoint; i < glyph.numberOfPoints; i++) {
        if (glyph.points[i].flag & shortFlag) {
            if (glyph.points[i].flag & sameFlag) {
                value += getUInt8t();
            } else {
                value -= getUInt8t();
            }
        } else if (~glyph.points[i].flag & sameFlag) {
            value += getUInt16t();
        }

        if (_xy == 'x') {
            if (glyphTransformation.enableScale) {
                glyph.points[i].x = value + glyphTransformation.dx;
            } else {
                glyph.points[i].x = value + glyphTransformation.dx;
            }
        } else {
            if (glyphTransformation.enableScale) {
                glyph.points[i].y = value + glyphTransformation.dy;
            } else {
                glyph.points[i].y = value + glyphTransformation.dy;
            }
        }
    }
}

/* read simple glyph */
uint8_t truetypeClass::readSimpleGlyph(uint8_t _addGlyph) {
    uint8_t repeatCount;
    uint8_t flag;
    static uint16_t counterContours;
    static uint16_t counterPoints;

    if (glyph.numberOfContours <= 0) {
        return 0;
    }

    if (!_addGlyph) {
        counterContours = 0;
        counterPoints = 0;
    }

    if (_addGlyph) {
        glyph.endPtsOfContours = (uint16_t *)realloc(glyph.endPtsOfContours, (sizeof(uint16_t) * glyph.numberOfContours));
    } else {
        glyph.endPtsOfContours = (uint16_t *)malloc((sizeof(uint16_t) * glyph.numberOfContours));
    }

    for (uint16_t i = counterContours; i < glyph.numberOfContours; i++) {
        glyph.endPtsOfContours[i] = counterPoints + getUInt16t();
    }

    ttfSeek(getUInt16t() + ttfPosition());

    for (uint16_t i = counterContours; i < glyph.numberOfContours; i++) {
        if (glyph.endPtsOfContours[i] > glyph.numberOfPoints) {
            glyph.numberOfPoints = glyph.endPtsOfContours[i];
        }
    }
    glyph.numberOfPoints++;

    if (_addGlyph) {
        glyph.points = (ttPoint_t *)realloc(glyph.points, sizeof(ttPoint_t) * (glyph.numberOfPoints + glyph.numberOfContours));
    } else {
        glyph.points = (ttPoint_t *)malloc(sizeof(ttPoint_t) * (glyph.numberOfPoints + glyph.numberOfContours));
    }

    for (uint16_t i = counterPoints; i < glyph.numberOfPoints; i++) {
        flag = getUInt8t();
        glyph.points[i].flag = flag;
        if (flag & FLAG_REPEAT) {
            repeatCount = getUInt8t();
            while (repeatCount--) {
                glyph.points[++i].flag = flag;
            }
        }
    }

    readCoords('x', counterPoints);
    readCoords('y', counterPoints);

    counterContours = glyph.numberOfContours;
    counterPoints = glyph.numberOfPoints;

    return 1;
}

/* read Compound glyph */
uint8_t truetypeClass::readCompoundGlyph() {
    uint16_t glyphIndex;
    uint16_t flags;
    uint8_t numberOfGlyphs = 0;
    uint32_t offset;
    int32_t arg1, arg2;

    glyph.numberOfContours = 0;

    do {
        flags = getUInt16t();
        glyphIndex = getUInt16t();

        glyphTransformation.enableScale = (flags & 0b00000001000) ? (1) : (0);

        if (flags & 0b00000000001) {
            arg1 = getInt16t();
            arg2 = getInt16t();
        } else if (flags & 0b00000000010) {
            // x/y offsets are signed bytes
            arg1 = (int8_t)getUInt8t();
            arg2 = (int8_t)getUInt8t();
        } else {
            // point numbers
            arg1 = getUInt8t();
            arg2 = getUInt8t();
        }
        // skip the scale (not applied), so the next component is read from the right place:
        // WE_HAVE_A_SCALE, WE_HAVE_AN_X_AND_Y_SCALE, WE_HAVE_A_TWO_BY_TWO (F2Dot14 values)
        if (flags & 0x0008) {
            getUInt16t();
        } else if (flags & 0x0040) {
            getUInt32t();
        } else if (flags & 0x0080) {
            getUInt32t();
            getUInt32t();
        }

        if (flags & 0b00000000010) {
            glyphTransformation.dx = arg1;
            glyphTransformation.dy = arg2;
        }

        if (flags & 0b01000000000) {
            charCode = glyphIndex;
        }

        offset = ttfPosition();

        uint32_t glyphOffset = getGlyphOffset(glyphIndex);
        ttfSeek(glyphOffset);
        glyph.numberOfContours += getInt16t();
        ttfSeek(glyphOffset + 10);

        if (numberOfGlyphs == 0) {
            readSimpleGlyph();
        } else {
            readSimpleGlyph(1);
        }
        ttfSeek(offset);

        numberOfGlyphs++;
        glyphTransformation = {0, 0, 0, 1, 1};  // init
    } while (flags & 0b00000100000);

    return 1;
}

/* read glyph */
uint8_t truetypeClass::readGlyph(uint16_t _code, uint8_t _justSize) {
    uint32_t offset = getGlyphOffset(_code);
    ttfSeek(offset);
    glyph.numberOfContours = getInt16t();
    glyph.numberOfPoints = 0;
    glyph.xMin = getInt16t();
    glyph.yMin = getInt16t();
    glyph.xMax = getInt16t();
    glyph.yMax = getInt16t();

    glyphTransformation = {0, 0, 0, 1, 1};  // init

    if (_justSize) {
        return 0;
    }

    if (glyph.numberOfContours >= 0) {
        return readSimpleGlyph();
    } else {
        return readCompoundGlyph();
    }
    return 0;
}

uint8_t truetypeClass::getMetrics(uint16_t _code, ttMetrics_t *pMetrics)
{
   ttHMetric_t HMetrics;
   uint16_t Id = codeToGlyphId(_code);
   uint32_t offset = getGlyphOffset(Id);

   ttfSeek(offset);
   getInt16t();  // eat numberOfContours
   pMetrics->xMin = getInt16t();
   pMetrics->yMin = getInt16t();
   pMetrics->xMax = getInt16t();
   pMetrics->yMax = getInt16t();
   HMetrics = getHMetric(Id);
   pMetrics->leftSideBearing = HMetrics.leftSideBearing;
   pMetrics->advanceWidth= HMetrics.advanceWidth;
    return 0;
}
/* free glyph */
void truetypeClass::freeGlyph() {
    if (glyph.points != nullptr) free(glyph.points);
    if (glyph.endPtsOfContours != nullptr) free(glyph.endPtsOfContours);
    glyph.points = nullptr;
    glyph.endPtsOfContours = nullptr;
    glyph.numberOfPoints = 0;
}

// generate Bitmap
void truetypeClass::generateOutline(int16_t _x, int16_t _y, uint16_t characterSize) {
    // keep the arrays of the previous glyph, only start counting again
    numPoints = 0;
    numBeginPoints = 0;
    numEndPoints = 0;

    float x0, y0, x1, y1;

    uint16_t j = 0;

    for (uint16_t i = 0; i < glyph.numberOfContours; i++) {
        uint16_t firstPointOfContour = j;
        uint16_t lastPointOfContour = glyph.endPtsOfContours[i];

        // Rotate to on-curve the first point
        uint16_t numberOfRotations = 0;
        while ((firstPointOfContour + numberOfRotations) <= lastPointOfContour) {
            if (glyph.points[(firstPointOfContour + numberOfRotations)].flag & FLAG_ONCURVE) {
                break;
            }
            numberOfRotations++;
        }
        if ((j + numberOfRotations) <= lastPointOfContour) {
            for (uint16_t ii = 0; ii < numberOfRotations; ii++) {
                ttPoint_t tmp = glyph.points[firstPointOfContour];
                for (uint16_t jj = firstPointOfContour; jj < lastPointOfContour; jj++) {
                    glyph.points[jj] = glyph.points[jj + 1];
                }
                glyph.points[lastPointOfContour] = tmp;
            }
        }

        ttCoordinate_t pointsOfCurve[6];
        pointsOfCurve[0].x = glyph.points[j].x;
        pointsOfCurve[0].y = glyph.points[j].y;

        while (j <= lastPointOfContour) {

            uint16_t searchPoint = (j == lastPointOfContour) ? (firstPointOfContour) : (j + 1);
            
            pointsOfCurve[1].x = glyph.points[searchPoint].x;
            pointsOfCurve[1].y = glyph.points[searchPoint].y;

            if (glyph.points[searchPoint].flag & FLAG_ONCURVE) {

                addLine(pointsOfCurve[0].x * characterSize / headTable.unitsPerEm + _x,
                        (ascender - pointsOfCurve[0].y) * characterSize / headTable.unitsPerEm + _y,
                        pointsOfCurve[1].x * characterSize / headTable.unitsPerEm + _x,
                        (ascender - pointsOfCurve[1].y) * characterSize / headTable.unitsPerEm + _y);

                pointsOfCurve[0] = pointsOfCurve[1];
                j += 1;

            } else {

                searchPoint = (searchPoint == lastPointOfContour) ? (firstPointOfContour) : (searchPoint + 1);

                if (glyph.points[searchPoint].flag & FLAG_ONCURVE) {
                    pointsOfCurve[2].x = glyph.points[searchPoint].x;
                    pointsOfCurve[2].y = glyph.points[searchPoint].y;
                    j += 2;
                } else {
                    pointsOfCurve[2].x = (pointsOfCurve[1].x + glyph.points[searchPoint].x) / 2;
                    pointsOfCurve[2].y = (pointsOfCurve[1].y + glyph.points[searchPoint].y) / 2;
                    j += 1;
                }

                x0 = pointsOfCurve[0].x;
                y0 = pointsOfCurve[0].y;

                // enough straight segments to stay within ~0.2 pixel of the curve: few for small text,
                // more for large digits (was always 9)
                const float scale = (float)characterSize / headTable.unitsPerEm;
                const float lengthPx = (hypotf(pointsOfCurve[1].x - pointsOfCurve[0].x, pointsOfCurve[1].y - pointsOfCurve[0].y) +
                                        hypotf(pointsOfCurve[2].x - pointsOfCurve[1].x, pointsOfCurve[2].y - pointsOfCurve[1].y)) * scale;
                const int steps = constrain((int)ceilf(sqrtf(lengthPx / 1.6f)), 1, 24);

                for (int step = 1; step <= steps; step++) {
                    const float t = (float)step / steps;
                    x1 = (1.0f - t) * (1.0f - t) * pointsOfCurve[0].x + 2.0f * t * (1.0f - t) * pointsOfCurve[1].x + t * t * pointsOfCurve[2].x;
                    y1 = (1.0f - t) * (1.0f - t) * pointsOfCurve[0].y + 2.0f * t * (1.0f - t) * pointsOfCurve[1].y + t * t * pointsOfCurve[2].y;

                    addLine(x0 * characterSize / headTable.unitsPerEm + _x, (ascender - y0) * characterSize / headTable.unitsPerEm + _y,
                            x1 * characterSize / headTable.unitsPerEm + _x, (ascender - y1) * characterSize / headTable.unitsPerEm + _y);

                    x0 = x1;
                    y0 = y1;
                }

                pointsOfCurve[0] = pointsOfCurve[2];

            }
        }
        addEndPoint(numPoints - 1);
        addBeginPoint(numPoints);
    }
    return;
}

void truetypeClass::addLine(float _x0, float _y0, float _x1, float _y1) {

    if (numPoints == 0) {
        addPoint(_x0, _y0);
        addBeginPoint(0);
    }
    addPoint(_x1, _y1);

    /*
        int16_t dx = abs(x1 - x0);
        int16_t dy = abs(y1 - y0);
        int16_t sx = (x0 < x1) ? 1 : -1;
        int16_t sy = (y0 < y1) ? 1 : -1;
        int16_t err = dx - dy;

        while (true) {
            addPixel(x0, y0, colorLine);
            if (x0 == x1 && y0 == y1) {
                break;
            }
            int16_t e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                x0 += sx;
            }
            if (e2 < dx) {
                err += dx;
                y0 += sy;
            }
        }
    */
}

void truetypeClass::fillGlyph(int16_t _x_min, int16_t _y_min, uint16_t characterSize) {
    // Scanline fill with the nonzero winding rule. Per scanline, collect where the edges cross it, sort
    // them, and walk the pixels left to right. A pixel is inside when the edges crossing to its right
    // add up to a nonzero winding number (+1 upward edge, -1 downward), like the per-pixel test that
    // was here before, without testing every edge for every pixel.
    if (numPoints == 0) return;
    ttCrossing_t *crossings = (ttCrossing_t *)malloc(sizeof(ttCrossing_t) * numPoints);
    if (crossings == nullptr) return;

    // a pixel is filled when its centre (x + 0.5, y + 0.5) is inside the outline
    const float scale = (float)characterSize / (float)headTable.unitsPerEm;
    const int16_t xStart = floorf(_x_min + glyph.xMin * scale);
    const int16_t xEnd = ceilf(_x_min + glyph.xMax * scale);
    const int16_t yStart = floorf((ascender - glyph.yMax) * scale + _y_min);
    const int16_t yEnd = ceilf((ascender - glyph.yMin) * scale + _y_min);

    for (int16_t y = yStart; y < yEnd; y++) {
        const float fy = (float)y + 0.5f;
        uint16_t numCrossings = 0;
        uint16_t bpCounter = 0;
        uint16_t epCounter = 0;
        int16_t total = 0;

        for (uint16_t i = 0; i < numPoints; i++) {
            const ttCoordinate_t &point1 = points[i];
            uint16_t p2Num;
            // Wrap?
            if (epCounter < numEndPoints && i == endPoints[epCounter]) {
                p2Num = beginPoints[bpCounter];
                epCounter++;
                bpCounter++;
            } else {
                p2Num = i + 1;
            }
            if (p2Num >= numPoints) continue;
            const ttCoordinate_t &point2 = points[p2Num];

            int8_t dir;
            if (point1.y <= fy && point2.y > fy) {
                dir = 1;  // upward
            } else if (point1.y > fy && point2.y <= fy) {
                dir = -1;  // downward
            } else {
                continue;
            }
            crossings[numCrossings].x = point1.x + (point2.x - point1.x) * (fy - point1.y) / (point2.y - point1.y);
            crossings[numCrossings].dir = dir;
            numCrossings++;
            total += dir;
        }
        if (numCrossings == 0) continue;

        std::sort(crossings, crossings + numCrossings, [](const ttCrossing_t &a, const ttCrossing_t &b) { return a.x < b.x; });

        // winding at x = sum over the crossings right of x = total - sum over the crossings at or left of x
        int16_t passed = 0;
        uint16_t k = 0;
        for (int16_t x = xStart; x < xEnd; x++) {
            while (k < numCrossings && crossings[k].x <= (float)x + 0.5f) {
                passed += crossings[k].dir;
                k++;
            }
            if (total - passed != 0) {
                addPixel(x, y, colorInside);
            }
        }
    }
    free(crossings);
}

void truetypeClass::textDraw(int16_t _x, int16_t _y, const wchar_t _character[]) {
    uint16_t c = 0;
    uint16_t prev_code = 0;

    while (_character[c] != '\0') {
        // space (half-width, full-width)
        if ((_character[c] == ' ') || (_character[c] == L'　')) {
            prev_code = 0;
            _x += spaceAdvance(_character[c]);
            c++;
            continue;
        }

        charCode = codeToGlyphId(_character[c]);

        //Serial.printf("code:%4d\n", charCode);
        readGlyph(charCode);

        _x += characterSpace;
#ifdef ENABLEKERNING
        if (prev_code != 0 && kerningOn) {
            int16_t kern = getKerning(prev_code, charCode);  // space between charctor
            _x += (kern * (int16_t)characterSize) / headTable.unitsPerEm;
        }
#endif
        prev_code = charCode;

        ttHMetric_t hMetric = getHMetric(charCode);

        // Line breaks when reaching the edge of the display
        if (c > 0 && (hMetric.advanceWidth + _x) > end_x) {
            _x = start_x;
            _y += characterSize;
            if (_y > end_y) {
                break;
            }
        }

        // Line breaks with line feed code
        if (_character[c] == '\n') {
            freePointsAll();
            freeGlyph();
            c++;
            _x = start_x;
            _y += characterSize;
            if (_y > end_y) {
                break;
            }
            continue;
        }

        if (glyph.numberOfContours >= 0) {
            generateOutline(_x, _y, characterSize);
            fillGlyph(_x, _y, characterSize);
        }
        freeGlyph();

        _x += hMetric.advanceWidth;
        c++;
    }
}

void truetypeClass::textDraw(int16_t _x, int16_t _y, const char _character[]) {
    uint16_t length = 0;
    while (_character[length] != '\0') {
        length++;
    }
    wchar_t *wcharacter = (wchar_t *)calloc(sizeof(wchar_t), length + 1);
    for (uint16_t i = 0; i < length; i++) {
        wcharacter[i] = _character[i];
    }
    textDraw(_x, _y, wcharacter);
    free(wcharacter);
    wcharacter = nullptr;
}

void truetypeClass::textDraw(int16_t _x, int16_t _y, const String _string) {
    uint16_t length = _string.length();
    wchar_t *wcharacter = (wchar_t *)calloc(sizeof(wchar_t), length + 1);
    stringToWchar(_string, wcharacter);
    textDraw(_x, _y, wcharacter);
    free(wcharacter);
    wcharacter = nullptr;
}

void truetypeClass::addPixel(int16_t _x, int16_t _y, uint16_t _colorCode) {
    uint8_t *buf_ptr;

    if (pfnDrawPixel) {  // user-supplied pixel function
        (*pfnDrawPixel)(_x, _y, _colorCode);
        return;
    }
    // limit to boundary co-ordinates the boundary is always in the same orientation as the string not the buffer
    if ((_x < start_x) || (_x >= end_x) || (_y >= end_y)) {
        return;
    }

    // Rotate co-ordinates relative to the buffer
    uint16_t temp = _x;
    switch (stringRotation) {
        case ROTATE_270:
            _x = _y;
            _y = displayHeight - 1 - temp;
            break;
        case ROTATE_180:
            _x = displayWidth - 1 - _x;
            _y = displayHeight - 1 - _y;
            break;
        case ROTATE_90:
            _x = displayWidth - 1 - _y;
            _y = temp;
            break;
        case 0:
        default:
            break;
    }

    // out of range
    if ((_x < 0) || ((uint16_t)_x >= displayWidth) || ((uint16_t)_y >= displayHeight) || (_y < 0)) {
        return;
    }

    switch (framebufferBit) {
        case 16:  // 16bit horizontal
        {
            uint16_t *p = (uint16_t *)&userFrameBuffer[(uint16_t)_x * 2 + (uint16_t)_y * displayWidthFrame];
            _colorCode = (_colorCode >> 8) | (_colorCode << 8);
            *p = _colorCode;
        } break;
        case 8:  // 8bit Horizontal
        {
            userFrameBuffer[(uint16_t)_x + (uint16_t)_y * displayWidthFrame] = (uint8_t)_colorCode;
        } break;
        case 4:  // 4bit Horizontal
        {
            buf_ptr = &userFrameBuffer[((uint16_t)_x / 2) + (uint16_t)_y * displayWidthFrame];
            _colorCode = _colorCode & 0b00001111;

            if ((uint16_t)_x % 2) {
                *buf_ptr = (*buf_ptr & 0b00001111) + (_colorCode << 4);
            } else {
                *buf_ptr = (*buf_ptr & 0b11110000) + _colorCode;
            }
        } break;
        case 1:  // 1bit Horizontal
        default: {
            buf_ptr = &userFrameBuffer[((uint16_t)_x / 8) + (uint16_t)_y * displayWidthFrame];
            uint8_t bitMask = 0b10000000 >> ((uint16_t)_x % 8);
            uint8_t bit = (_colorCode) ? (bitMask) : (0b00000000);
            *buf_ptr = (*buf_ptr & ~bitMask) + bit;
        } break;
    }
    return;
}

uint16_t truetypeClass::getStringWidth(const wchar_t _character[]) {
    uint16_t prev_code = 0;
    uint16_t c = 0;
    uint16_t output = 0;

    while (_character[c] != '\0') {
        // space (half-width, full-width)
        if ((_character[c] == ' ') || (_character[c] == L'　')) {
            prev_code = 0;
            output += spaceAdvance(_character[c]);
            c++;
            continue;
        }
        uint16_t code = codeToGlyphId(_character[c]);
        readGlyph(code, 1);

        output += characterSpace;
#ifdef ENABLEKERNING
        if (prev_code != 0 && kerningOn) {
            int16_t kern = getKerning(prev_code, code);  // space between charctor
            output += (kern * (int16_t)characterSize) / headTable.unitsPerEm;
        }
#endif
        prev_code = code;

        ttHMetric_t hMetric = getHMetric(code);
        output += hMetric.advanceWidth;
        c++;
    }

    return output;
}

uint16_t truetypeClass::getStringWidth(const char _character[]) {
    uint16_t length = 0;
    uint16_t output = 0;
    while (_character[length] != '\0') {
        length++;
    }
    wchar_t *wcharacter = (wchar_t *)calloc(sizeof(wchar_t), length + 1);
    for (uint16_t i = 0; i < length; i++) {
        wcharacter[i] = _character[i];
    }
    output = getStringWidth(wcharacter);
    free(wcharacter);
    wcharacter = nullptr;
    return output;
}

uint16_t truetypeClass::getStringWidth(const String _string) {
    uint16_t length = _string.length();
    uint16_t output = 0;

    wchar_t *wcharacter = (wchar_t *)calloc(sizeof(wchar_t), length + 1);
    stringToWchar(_string, wcharacter);

    output = getStringWidth(wcharacter);
    free(wcharacter);
    wcharacter = nullptr;
    return output;
}

/* Points*/
// grow an array to hold at least count + 1 elements; false if out of memory
template <typename T>
static bool reserveOneMore(T *&array, uint16_t count, uint16_t &capacity) {
    if (count < capacity) return true;
    const uint16_t newCapacity = capacity ? capacity * 2 : 64;
    T *grown = (T *)realloc(array, sizeof(T) * newCapacity);
    if (grown == nullptr) return false;
    array = grown;
    capacity = newCapacity;
    return true;
}

void truetypeClass::addPoint(float _x, float _y) {
    if (!reserveOneMore(points, numPoints, pointsCap)) return;
    points[numPoints].x = _x;
    points[numPoints].y = _y;
    numPoints++;
}

void truetypeClass::addBeginPoint(uint16_t _bp) {
    if (!reserveOneMore(beginPoints, numBeginPoints, beginPointsCap)) return;
    beginPoints[numBeginPoints++] = _bp;
}

void truetypeClass::addEndPoint(uint16_t _ep) {
    if (!reserveOneMore(endPoints, numEndPoints, endPointsCap)) return;
    endPoints[numEndPoints++] = _ep;
}

void truetypeClass::freePointsAll() {
    freePoints();
    freeBeginPoints();
    freeEndPoints();
}

void truetypeClass::freePoints() {
    free(points);
    points = nullptr;
    numPoints = 0;
    pointsCap = 0;
}

void truetypeClass::freeBeginPoints() {
    free(beginPoints);
    beginPoints = nullptr;
    numBeginPoints = 0;
    beginPointsCap = 0;
}

void truetypeClass::freeEndPoints() {
    free(endPoints);
    endPoints = nullptr;
    numEndPoints = 0;
    endPointsCap = 0;
}

/* file */
/* seek to the first position of the specified table name */
uint32_t truetypeClass::seekToTable(const char *name) {
    for (uint32_t i = 0; i < numTables; i++) {
        if (strcmp(table[i].name, name) == 0) {
            ttfSeek(table[i].offset);
            return table[i].offset;
        }
    }
    return 0;
}

/* calculate */
void truetypeClass::stringToWchar(String _string, wchar_t _charctor[]) {
    uint16_t s = 0;
    uint16_t c = 0;
    uint32_t codeu32;

    while (_string[s] != '\0') {
        int numBytes = GetU8ByteCount(_string[s]);
        switch (numBytes) {
            case 1:
                codeu32 = char32_t(uint8_t(_string[s]));
                s++;
                break;
            case 2:
                if (!IsU8LaterByte(_string[s + 1])) {
                    s++;  // invalid UTF-8: skip this byte, otherwise the loop never ends
                    continue;
                }
                if ((uint8_t(_string[s]) & 0x1E) == 0) {
                    s++;
                    continue;
                }

                codeu32 = char32_t(_string[s] & 0x1F) << 6;
                codeu32 |= char32_t(_string[s + 1] & 0x3F);
                s += 2;
                break;
            case 3:
                if (!IsU8LaterByte(_string[s + 1]) || !IsU8LaterByte(_string[s + 2])) {
                    s++;
                    continue;
                }
                if ((uint8_t(_string[s]) & 0x0F) == 0 &&
                    (uint8_t(_string[s + 1]) & 0x20) == 0) {
                    s++;
                    continue;
                }

                codeu32 = char32_t(_string[s] & 0x0F) << 12;
                codeu32 |= char32_t(_string[s + 1] & 0x3F) << 6;
                codeu32 |= char32_t(_string[s + 2] & 0x3F);
                s += 3;
                break;
            case 4:
                if (!IsU8LaterByte(_string[s + 1]) || !IsU8LaterByte(_string[s + 2]) ||
                    !IsU8LaterByte(_string[s + 3])) {
                    s++;
                    continue;
                }
                if ((uint8_t(_string[s]) & 0x07) == 0 &&
                    (uint8_t(_string[s + 1]) & 0x30) == 0) {
                    s++;
                    continue;
                }

                codeu32 = char32_t(_string[s] & 0x07) << 18;
                codeu32 |= char32_t(_string[s + 1] & 0x3F) << 12;
                codeu32 |= char32_t(_string[s + 2] & 0x3F) << 6;
                codeu32 |= char32_t(_string[s + 3] & 0x3F);
                s += 4;
                break;
            default:
                s++;
                continue;
        }

        if (codeu32 < 0 || codeu32 > 0x10FFFF) {
            continue;
        }

        if (codeu32 < 0x10000) {
            _charctor[c] = char16_t(codeu32);
        } else {
            _charctor[c] = ((char16_t((codeu32 - 0x10000) % 0x400 + 0xDC00)) << 8) | (char16_t((codeu32 - 0x10000) / 0x400 + 0xD800));
        }
        c++;
    }
    _charctor[c] = 0;
}

uint8_t truetypeClass::GetU8ByteCount(char _ch) {
    if (0 <= uint8_t(_ch) && uint8_t(_ch) < 0x80) {
        return 1;
    }
    if (0xC2 <= uint8_t(_ch) && uint8_t(_ch) < 0xE0) {
        return 2;
    }
    if (0xE0 <= uint8_t(_ch) && uint8_t(_ch) < 0xF0) {
        return 3;
    }
    if (0xF0 <= uint8_t(_ch) && uint8_t(_ch) < 0xF8) {
        return 4;
    }
    return 0;
}

bool truetypeClass::IsU8LaterByte(char _ch) {
    return 0x80 <= uint8_t(_ch) && uint8_t(_ch) < 0xC0;
}

/* get uint8_t at the current position */
uint8_t truetypeClass::getUInt8t() {
    uint8_t x;

    ttfRead(&x, 1);
    return x;
}

/* get int16_t at the current position */
int16_t truetypeClass::getInt16t() {
    byte x[2];

    ttfRead(x, 2);
    return (x[0] << 8) | x[1];
}

/* get uint16_t at the current position */
uint16_t truetypeClass::getUInt16t() {
    byte x[2];

    ttfRead(x, 2);
    return (x[0] << 8) | x[1];
}

/* get uint32_t at the current position */
uint32_t truetypeClass::getUInt32t() {
    byte x[4];

    ttfRead(x, 4);
    return (x[0] << 24) | (x[1] << 16) | (x[2] << 8) | x[3];
}
