// Content: day-ahead energy prices.
// Included once by contentmanager.cpp, not compiled on its own.

#ifdef CONTENT_DAYAHEAD
uint16_t getPercentileColor(const double *prices, int numPrices, double price, HwType hwdata) {
    const char *colorsDefault[] = {"black", "darkgray", "pink", "red"};
    const double boundariesDefault[] = {40.0, 80.0, 90.0};

    const char *colors3bpp[] = {"blue", "green", "yellow", "orange", "red"};
    const double boundaries3bpp[] = {20.0, 50.0, 70.0, 90.0};

    const char **colors;
    const double *boundaries;
    int numColors, numBoundaries;

    if (hwdata.bpp == 3 || hwdata.bpp == 4) {
        colors = colors3bpp;
        boundaries = boundaries3bpp;
        numColors = sizeof(colors3bpp) / sizeof(colors3bpp[0]);
        numBoundaries = sizeof(boundaries3bpp) / sizeof(boundaries3bpp[0]);
    } else {
        colors = colorsDefault;
        boundaries = boundariesDefault;
        numColors = sizeof(colorsDefault) / sizeof(colorsDefault[0]);
        numBoundaries = sizeof(boundariesDefault) / sizeof(boundariesDefault[0]);
        if (hwdata.highlightColor == 3) {
            colors[2] = "brown";
            colors[3] = "yellow";
        }
    }
    int colorIndex = numColors - 1;

    for (int i = 0; i < numBoundaries; i++) {
        if (price < prices[int(numPrices * boundaries[i] / 100.0)]) {
            colorIndex = i;
            break;
        }
    }
    return getColor(String(colors[constrain(colorIndex, 0, numColors - 1)]));
}

struct YAxisScale {
    double min;
    double max;
    double step;
};

double roundToNearest(double value, double factor) {
    return round(value / factor) * factor;
}

int mapDouble(double value, double fromMin, double fromMax, int toMin, int toMax) {
    return static_cast<int>(round((value - fromMin) / (fromMax - fromMin) * (toMax - toMin) + toMin));
}

YAxisScale calculateYAxisScale(double priceMin, double priceMax, int divisions) {
    double range = priceMax - priceMin;
    double stepSize = range / divisions;
    double orderOfMagnitude = pow(10, floor(log10(stepSize)));
    double significantDigit = stepSize / orderOfMagnitude;

    double roundedSignificantDigit;
    double thresholds[] = {1.0, 2.0, 5.0, 10.0};
    for (double threshold : thresholds) {
        if (significantDigit <= threshold) {
            roundedSignificantDigit = threshold;
            break;
        }
    }

    double roundedStepSize = roundedSignificantDigit * orderOfMagnitude;
    double newRange = roundedStepSize * divisions;
    double minY = floor(priceMin / roundedStepSize) * roundedStepSize;
    double maxY = minY + newRange;

    return {minY, maxY, roundedStepSize};
}

// Which raw samples become bars. The bars fill the bar area: first the pixels per bar are picked, as close
// to the target bar width plus 1 px spacing as the data from now - 1h on allows. That gives the number of bars,
// and the first bar moves back into the past until the area is full, so the gap at the right is always less
// than one bar.
struct DataRange {
    int startIndex;          // first raw sample of the first bar
    int endIndex;            // one past the last raw sample used
    int numAveragedSamples;  // number of bars
    int historicBars;        // bars before now - 1h, added to fill the area
    int pixelsPerBar;
};

DataRange calculateDataRange(const JsonDocument& doc, int originalDataSize, time_t now, int avgFactor,
                             int targetIntervalMinutes, int barAreaWidth, double targetBarwidth) {
    DataRange result = {0, 0, 0, 0, 0};

    // A bar of several samples starts on a multiple of the bar interval (:00 and :30 for 30 minutes)
    int firstAligned = 0;
    if (avgFactor > 1) {
        firstAligned = -1;
        for (int i = 0; i < originalDataSize; i++) {
            const time_t dataTime = doc[i]["time"];
            struct tm dt;
            localtime_r(&dataTime, &dt);
            if ((dt.tm_hour * 60 + dt.tm_min) % targetIntervalMinutes == 0) {
                firstAligned = i;
                break;
            }
        }
        if (firstAligned < 0) return result;
    }
    const int totalBars = (originalDataSize - firstAligned) / avgFactor;
    if (totalBars <= 0) return result;

    // Everything from the bar that holds now - 1h on has to be shown
    int hourAgoIndex = 0;
    for (int i = originalDataSize - 1; i >= 0; i--) {
        const time_t dataTime = doc[i]["time"];
        if (dataTime <= now - 3600) {
            hourAgoIndex = i;
            break;
        }
    }
    const int firstNeededBar = (hourAgoIndex > firstAligned) ? (hourAgoIndex - firstAligned) / avgFactor : 0;
    int endBar = totalBars;
    const int neededBars = endBar - firstNeededBar;

    // Widest bars that still fit the needed data
    int pixelsPerBar = (int)targetBarwidth + 1;
    while (pixelsPerBar > 1 && barAreaWidth / pixelsPerBar < neededBars) pixelsPerBar--;
    int numBars = barAreaWidth / pixelsPerBar;
    if (numBars < neededBars) endBar = firstNeededBar + numBars;  // even 1 px bars don't fit: drop the far future
    // Not enough history to fill the area (hourly bars from a feed of three days): wider bars instead of a gap
    while (numBars > endBar && barAreaWidth / (pixelsPerBar + 1) >= neededBars) {
        pixelsPerBar++;
        numBars = barAreaWidth / pixelsPerBar;
    }
    if (numBars > endBar) numBars = endBar;
    const int startBar = endBar - numBars;

    result.startIndex = firstAligned + startBar * avgFactor;
    result.endIndex = firstAligned + endBar * avgFactor;
    if (result.endIndex > originalDataSize) result.endIndex = originalDataSize;
    result.numAveragedSamples = numBars;
    result.historicBars = firstNeededBar - startBar;
    result.pixelsPerBar = pixelsPerBar;
    return result;
}

// Data structure for averaged price points
struct AveragedDataPoint {
    time_t time;
    double price;
    int hour;
    int minute;
};

// Helper function: the cheapest block of the given length per day, for appliance scheduling: one for today
// (also when it is over or still running) and one for tomorrow once its prices are known. Worked out on the
// raw prices of the whole day, so a block before the start of the graph still counts. Returned as the bars
// that overlap it; -1 when no bar does.
struct CheapBlock {
    int start;
    int end;
};

struct CheapBlocks {
    CheapBlock block1;
    CheapBlock block2;
};

CheapBlocks findCheapestBlockPerDay(const std::vector<time_t>& rawTimes, const std::vector<double>& rawPrices,
                                    int nativeIntervalMinutes, double blockHours, const AveragedDataPoint* avgData,
                                    int numBars, int barIntervalMinutes, time_t now) {
    CheapBlocks result;
    result.block1.start = -1;
    result.block1.end = -1;
    result.block2.start = -1;
    result.block2.end = -1;

    const int numRaw = rawTimes.size();
    const int blockSamples = (int)(blockHours * 60 / nativeIntervalMinutes + 0.5);
    if (blockSamples <= 0 || blockSamples > numRaw || numBars <= 0) {
        return result;  // Invalid or disabled
    }
    const time_t step = nativeIntervalMinutes * 60;

    struct tm nowTm;
    localtime_r(&now, &nowTm);
    CheapBlock* slots[2] = {&result.block1, &result.block2};

    for (int day = 0; day < 2; day++) {
        // Local midnight at the start and the end of today (day 0) and tomorrow (day 1)
        struct tm startTm = nowTm;
        startTm.tm_hour = startTm.tm_min = startTm.tm_sec = 0;
        startTm.tm_mday += day;
        startTm.tm_isdst = -1;
        const time_t dayStart = mktime(&startTm);
        struct tm endTm = nowTm;
        endTm.tm_hour = endTm.tm_min = endTm.tm_sec = 0;
        endTm.tm_mday += day + 1;
        endTm.tm_isdst = -1;
        const time_t dayEnd = mktime(&endTm);

        double bestSum = std::numeric_limits<double>::max();
        time_t bestStart = 0;
        for (int i = 0; i + blockSamples <= numRaw; i++) {
            if (rawTimes[i] < dayStart) continue;
            if (rawTimes[i + blockSamples - 1] + step > dayEnd) break;
            if (rawTimes[i + blockSamples - 1] - rawTimes[i] != (time_t)(blockSamples - 1) * step) continue;  // gap
            double blockSum = 0;
            for (int j = 0; j < blockSamples; j++) blockSum += rawPrices[i + j];
            if (blockSum < bestSum) {
                bestSum = blockSum;
                bestStart = rawTimes[i];
            }
        }
        if (bestStart == 0) continue;  // no prices for this day (yet)

        const time_t bestEnd = bestStart + blockSamples * step;
        for (int b = 0; b < numBars; b++) {
            const time_t barStart = avgData[b].time;
            if (barStart + barIntervalMinutes * 60 > bestStart && barStart < bestEnd) {
                if (slots[day]->start < 0) slots[day]->start = b;
                slots[day]->end = b;
            }
        }
    }

    return result;
}

bool getDayAheadFeed(String& filename, JsonObject& cfgobj, tagRecord*& taginfo, imgParam& imageParams) {
    wsLog("get dayahead prices");

    // Magic number constants for bar width calculations
    const double TARGET_BARWIDTH = 5.0;  // Optimal bar width in pixels (good visibility)
    const double MIN_BARWIDTH = 2.0;     // Minimum bar width (below this, bars hard to see)
    const int MIN_LABEL_SPACING = 30;    // Minimum pixels between label centers
    const int DASH_LENGTH = 3;           // Dashed line segment length
    const int GAP_LENGTH = 2;            // Gap between dashed line segments
    const int STEM_WIDTH = 3;            // Current time arrow stem width
    const int ARROW_WIDTH = 8;           // Current time arrow head width
    const int ARROW_HEIGHT = 6;          // Current time arrow head height
    const int STEM_HEIGHT = 10;          // Current time arrow stem height
    const int GAP_AFTER_ARROW = 2;       // Gap between arrow tip and vertical line

    JsonDocument loc;
    getTemplate(loc, 27, taginfo->hwType);

    // This is a link to a Google Apps Script script, which fetches (and caches) the tariff from https://transparency.entsoe.eu/
    // I made it available to provide easy access to the data, but please don't use this link in any projects other than OpenEpaperLink.
    String URL = "https://script.google.com/macros/s/AKfycbwMmeGAaPrWzVZrESSpmPmD--O132PzW_acnBsuEottKNATTqCRn6h8zN0Yts7S56ggsg/exec?country=" + cfgobj["country"].as<String>();

    time_t now;
    time(&now);

    HTTPClient http;
    http.begin(URL);
    http.setTimeout(10000);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();
    if (httpCode != 200) {
        wsErr("getDayAhead http error " + String(httpCode));
        logLineRateLimited("getDayAhead", "dayahead: http error " + String(httpCode));
        return false;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, http.getString());
    if (error) {
        wsErr(error.c_str());
    }
    http.end();

    TFT_eSprite spr = TFT_eSprite(&tft);

    initSprite(spr, imageParams.width, imageParams.height, imageParams);

    int originalDataSize = doc.size();
    if (originalDataSize == 0) {
        wsErr("No data in dayahead feed");
        return false;
    }

    // Detect native data interval (15-min, hourly, etc.)
    int nativeIntervalMinutes = 15;  // Default to 15 minutes (most common in Europe)
    if (originalDataSize >= 2) {
        time_t time0 = doc[0]["time"];
        time_t time1 = doc[1]["time"];
        nativeIntervalMinutes = (time1 - time0) / 60;
        if (nativeIntervalMinutes <= 0) nativeIntervalMinutes = 15;
    }

    // Without a price for the current time (a day missing in the feed), the chart shows the wrong day and
    // "nan" as the current price. Keep the previous image instead and try again later.
    bool hasCurrentPrice = false;
    for (int i = 0; i < originalDataSize; i++) {
        const time_t dataTime = doc[i]["time"];
        if (now >= dataTime && now < dataTime + nativeIntervalMinutes * 60) {
            hasCurrentPrice = true;
            break;
        }
    }
    if (!hasCurrentPrice) {
        wsErr("No price for the current time in the dayahead feed");
        logLineRateLimited("getDayAheadNow", "dayahead: no price for the current time in the feed (" + cfgobj["country"].as<String>() + ")");
        return false;
    }

    // Get user's preferred display interval (0 = native, 30 = 30 minutes, 60 = 1 hour)
    int targetIntervalMinutes = cfgobj["interval"] ? cfgobj["interval"].as<int>() : 0;
    if (targetIntervalMinutes == 0) {
        targetIntervalMinutes = nativeIntervalMinutes;  // Use native interval
    }

    // Calculate averaging factor (how many native samples per target interval)
    int avgFactor = targetIntervalMinutes / nativeIntervalMinutes;
    if (avgFactor < 1) avgFactor = 1;  // Safety: no upsampling

    // Get bar area width for calculations
    int barAreaWidth = loc["bars"][1].as<int>();

    // Store RAW current price BEFORE averaging (for accurate "now" display)
    // We'll calculate this after we have units/tariff variables below
    double rawCurrentPrice = std::numeric_limits<double>::quiet_NaN();
    int rawCurrentIndex = -1;
    int rawCurrentHour = 0;    // Hour from the native interval containing "now"
    int rawCurrentMinute = 0;  // Minute from the native interval containing "now"

    // Which samples become bars, filling the bar area (Steps 1-3)
    DataRange range = calculateDataRange(doc, originalDataSize, now, avgFactor, targetIntervalMinutes,
                                         barAreaWidth, TARGET_BARWIDTH);
    int startIndex = range.startIndex;
    int endIndex = range.endIndex;
    int numAveragedSamples = range.numAveragedSamples;

    // Safety check
    if (numAveragedSamples == 0) {
        wsErr("Not enough data for selected interval");
        return false;
    }

    // Debug logging (always visible in web console)
    wsLog("Day-ahead bars: " + String(numAveragedSamples) + " x " + String(range.pixelsPerBar) + "px, " +
          String(range.historicBars) + " of them before now-1h, area " + String(barAreaWidth) + "px");

    int units = cfgobj["units"].as<int>();
    if (units == 0) units = 1;
    double tarifkwh;
    double tariftax = cfgobj["tarifftax"].as<double>();

    // Create averaged data array
    AveragedDataPoint avgData[numAveragedSamples];

    // Parse tariff array if provided
    JsonDocument doc2;
    JsonArray tariffArray;
    std::string tariffString = cfgobj["tariffkwh"].as<std::string>();
    if (tariffString.front() == '[') {
        if (deserializeJson(doc2, tariffString) == DeserializationError::Ok) {
            tariffArray = doc2.as<JsonArray>();
        } else {
            Serial.println("Error in tariffkwh array");
        }
    }

    // Average the data according to selected interval
    for (int i = 0; i < numAveragedSamples; i++) {
        double priceSum = 0;
        time_t blockTime = doc[startIndex + i * avgFactor]["time"];

        // Average prices across the interval
        for (int j = 0; j < avgFactor; j++) {
            int idx = startIndex + i * avgFactor + j;
            if (idx >= endIndex) break;  // Safety check - respect endIndex

            const JsonObject& obj = doc[idx];
            const time_t item_time = obj["time"];
            struct tm item_timeinfo;
            localtime_r(&item_time, &item_timeinfo);

            if (tariffArray.size() == 24) {
                tarifkwh = tariffArray[item_timeinfo.tm_hour].as<double>();
            } else {
                tarifkwh = cfgobj["tariffkwh"].as<double>();
            }

            double price = (obj["price"].as<double>() / 10 + tarifkwh) * (1 + tariftax / 100) / units;
            priceSum += price;
        }

        // Store averaged data
        avgData[i].price = priceSum / avgFactor;
        avgData[i].time = blockTime;
        struct tm blockTimeInfo;
        localtime_r(&blockTime, &blockTimeInfo);
        avgData[i].hour = blockTimeInfo.tm_hour;
        avgData[i].minute = blockTimeInfo.tm_min;
    }

    // Now work with averaged data (n becomes numAveragedSamples)
    int n = numAveragedSamples;

    // Calculate RAW current price (find closest PAST/CURRENT data point, never future)
    if (std::isnan(rawCurrentPrice)) {
        time_t closestTimeDiff = std::numeric_limits<time_t>::max();

        for (int i = 0; i < originalDataSize; i++) {
            time_t dataTime = doc[i]["time"];

            // Skip future timestamps - only consider past and current
            if (dataTime > now) continue;

            time_t diff = now - dataTime;

            if (diff < closestTimeDiff) {
                closestTimeDiff = diff;
                rawCurrentIndex = i;

                // Calculate raw price with tariff (same logic as averaging loop)
                struct tm item_timeinfo;
                localtime_r(&dataTime, &item_timeinfo);

                if (tariffArray.size() == 24) {
                    tarifkwh = tariffArray[item_timeinfo.tm_hour].as<double>();
                } else {
                    tarifkwh = cfgobj["tariffkwh"].as<double>();
                }

                rawCurrentPrice = (doc[i]["price"].as<double>() / 10 + tarifkwh) * (1 + tariftax / 100) / units;
                rawCurrentHour = item_timeinfo.tm_hour;   // Store the native interval's hour
                rawCurrentMinute = item_timeinfo.tm_min;  // Store the native interval's minute
            }
        }
    }

    // Calculate min/max and create sorted price array for percentiles
    double minPrice = std::numeric_limits<double>::max();
    double maxPrice = std::numeric_limits<double>::lowest();
    double prices[n];

    for (int i = 0; i < n; i++) {
        prices[i] = avgData[i].price;
        minPrice = std::min(minPrice, prices[i]);
        maxPrice = std::max(maxPrice, prices[i]);
    }
    std::sort(prices, prices + n);

    YAxisScale yAxisScale = calculateYAxisScale(minPrice, maxPrice, loc["yaxis"][2].as<int>());
    minPrice = yAxisScale.min;

    uint16_t yAxisX = loc["yaxis"][1].as<int>();
    uint16_t yAxisY = loc["yaxis"][3].as<int>() | 9;
    uint16_t barBottom = loc["bars"][3].as<int>();

    for (double i = minPrice; i <= maxPrice; i += yAxisScale.step) {
        int y = mapDouble(i, minPrice, maxPrice, spr.height() - barBottom, spr.height() - barBottom - loc["bars"][2].as<int>());
        spr.drawLine(0, y, spr.width(), y, TFT_BLACK);
        if (loc["yaxis"][0]) {
            String label = (maxPrice * units < 10) ? String(i * units, 1) : String(int(i * units));
            drawString(spr, label, yAxisX, y - yAxisY, loc["yaxis"][0], TL_DATUM, TFT_BLACK);
        }
    }

    // Step 4: Calculate optimal bar width and spacing
    // Goal: Fill entire width with bars, using spacing when room allows
    int availableWidth = loc["bars"][1].as<int>();
    int pixelsPerBar = availableWidth / n;  // How many pixels each bar+spacing can use
    int barwidth;
    int barSpacing;

    if (pixelsPerBar >= 3 && targetIntervalMinutes > 15) {
        // Room for spacing - use 1px gap between bars. Not between quarter-hour bars: they are narrow, and
        // the white lines between them stand out more than the bars.
        barSpacing = 1;
        barwidth = pixelsPerBar - barSpacing;  // Remaining pixels for bar itself
    } else {
        // Tight fit - no room for spacing
        barSpacing = 0;
        barwidth = pixelsPerBar;  // Use all available pixels for bar
    }

    // Result: n × (barwidth + barSpacing) ≈ availableWidth (within rounding)

    uint16_t barheight = loc["bars"][2].as<int>() / (maxPrice - minPrice);
    uint16_t arrowY = 0;
    if (loc["bars"].size() >= 5) arrowY = loc["bars"][4].as<int>();
    uint16_t barX = loc["bars"][0].as<int>();
    double pricenow = std::numeric_limits<double>::quiet_NaN();
    bool showcurrent = true;
    if (cfgobj["showcurr"] && cfgobj["showcurr"] == "0") showcurrent = false;

    // Calculate label interval based on display width and number of bars
    // Goal: Space labels far enough apart to avoid overlap
    // Note: availableWidth already declared above, reuse it here
    int maxLabels = availableWidth / MIN_LABEL_SPACING;
    int labelInterval = (n + maxLabels - 1) / maxLabels;  // Round up division

    // Ensure labelInterval is at least 1
    if (labelInterval < 1) labelInterval = 1;

    // For better aesthetics, round to nearest multiple based on target interval
    int labelsPerHour = 60 / targetIntervalMinutes;
    int hoursPerLabel = 0;  // 0: label every labelInterval-th bar (bars longer than an hour)
    if (labelsPerHour > 0) {
        // Try to show labels at nice intervals (every hour, every 2 hours, etc.)
        hoursPerLabel = (labelInterval + labelsPerHour - 1) / labelsPerHour;  // Round up
        if (hoursPerLabel < 1) hoursPerLabel = 1;
        labelInterval = hoursPerLabel * labelsPerHour;

        // But don't exceed our max labels constraint
        if (n / labelInterval > maxLabels) {
            // Fall back to spacing-based calculation
            labelInterval = (n + maxLabels - 1) / maxLabels;
            hoursPerLabel = 0;
        }
    }

    // Find the cheapest block per day for appliance scheduling (Step 5), on the raw prices with the same
    // surcharge and tax as the bars
    double cheapBlockHoursRaw = cfgobj["cheapblock"] ? cfgobj["cheapblock"].as<double>() : 3.0;
    std::vector<time_t> rawTimes;
    std::vector<double> rawPrices;
    rawTimes.reserve(originalDataSize);
    rawPrices.reserve(originalDataSize);
    for (JsonObject obj : doc.as<JsonArray>()) {
        const time_t item_time = obj["time"];
        struct tm item_timeinfo;
        localtime_r(&item_time, &item_timeinfo);
        const double kwh = (tariffArray.size() == 24) ? tariffArray[item_timeinfo.tm_hour].as<double>()
                                                      : cfgobj["tariffkwh"].as<double>();
        rawTimes.push_back(item_time);
        rawPrices.push_back((obj["price"].as<double>() / 10 + kwh) * (1 + tariftax / 100) / units);
    }

    CheapBlocks cheapBlocks = findCheapestBlockPerDay(rawTimes, rawPrices, nativeIntervalMinutes, cheapBlockHoursRaw,
                                                      avgData, n, targetIntervalMinutes, now);
    int cheapBlockStart1 = cheapBlocks.block1.start;
    int cheapBlockEnd1 = cheapBlocks.block1.end;
    int cheapBlockStart2 = cheapBlocks.block2.start;
    int cheapBlockEnd2 = cheapBlocks.block2.end;

    // Store pointer position for drawing after all bars
    int pointerX = -1;         // -1 means no pointer to draw
    int currentHour = 0;       // Hour of the current interval
    int currentMinute = 0;     // Minute of the current interval
    int currentBarHeight = 0;  // Height of the current bar (for narrow bar line skipping)

    // Draw light grey background for cheapest blocks (BEFORE labels/ticks so they draw on top)
    int baselineY = spr.height() - barBottom;
    int xAxisHeight = 20;  // Height of X-axis area (ticks + labels)

    if (cheapBlockStart1 >= 0 && cheapBlockEnd1 >= 0) {
        int rectX = barX + cheapBlockStart1 * (barwidth + barSpacing);
        int rectWidth = (cheapBlockEnd1 - cheapBlockStart1 + 1) * (barwidth + barSpacing);
        spr.fillRect(rectX, baselineY + 1, rectWidth, xAxisHeight - 1, TFT_LIGHTGREY);
    }
    if (cheapBlockStart2 >= 0 && cheapBlockEnd2 >= 0) {
        int rectX = barX + cheapBlockStart2 * (barwidth + barSpacing);
        int rectWidth = (cheapBlockEnd2 - cheapBlockStart2 + 1) * (barwidth + barSpacing);
        spr.fillRect(rectX, baselineY + 1, rectWidth, xAxisHeight - 1, TFT_LIGHTGREY);
    }

    for (int i = 0; i < n; i++) {
        // Get data from avgData array (already trimmed to visible range)
        const time_t item_time = avgData[i].time;
        const int item_hour = avgData[i].hour;
        const int item_minute = avgData[i].minute;
        const double price = avgData[i].price;

        uint16_t barcolor = getPercentileColor(prices, numAveragedSamples, price, imageParams.hwdata);
        uint16_t thisbarh = mapDouble(price, minPrice, maxPrice, 0, loc["bars"][2].as<int>());
        spr.fillRect(barX + i * (barwidth + barSpacing), spr.height() - barBottom - thisbarh, barwidth, thisbarh, barcolor);

        // Draw tickmarks and labels at appropriate intervals
        int labelX = barX + i * (barwidth + barSpacing) + barwidth / 2;
        int tickY = spr.height() - barBottom;

        // Check if this bar represents the start of an hour (minute = 0)
        bool isHourStart = (item_minute == 0);
        // Check if this bar represents a half-hour (minute = 30)
        bool isHalfHour = (item_minute == 30);

        // Skip tick marks on displays with limited vertical space (height < 150px)
        // to prevent labels from being pushed off the bottom edge
        bool skipTicks = (spr.height() < 150);

        // Every 2 hours: Black tick (4px) + label
        // Labels on whole hours, so "18" is at 18:00 whatever time the first bar starts
        const bool labelHere = hoursPerLabel > 0 ? (item_minute == 0 && item_hour % hoursPerLabel == 0)
                                                 : (i % labelInterval == 0);
        if (labelHere && loc["time"][0]) {
            if (!skipTicks) {
                spr.drawLine(labelX, tickY, labelX, tickY + 4, TFT_BLACK);
            }
            drawString(spr, String(item_hour), labelX, tickY + (skipTicks ? 3 : 6), loc["time"][0], TC_DATUM, TFT_BLACK);
        }
        // Every hour (not already labeled): Dark grey tick (3px)
        else if (isHourStart && !skipTicks) {
            spr.drawLine(labelX, tickY, labelX, tickY + 3, getColor("darkgray"));
        }
        // Every half hour: Very short dark grey tick (1px)
        else if (isHalfHour && !skipTicks) {
            spr.drawPixel(labelX, tickY + 1, getColor("darkgray"));
        }

        // Store pointer position for drawing after all bars
        // Use RAW current price (stored before averaging) for accurate display
        // Find the bar that contains "now" by checking if now falls within this bar's time interval
        // Important: Use targetIntervalMinutes (the actual bar interval after averaging/display)
        if (std::isnan(pricenow) && showcurrent) {
            time_t barEndTime = item_time + (targetIntervalMinutes * 60);  // End of this bar's time range
            if (now >= item_time && now < barEndTime) {
                pointerX = barX + i * (barwidth + barSpacing) + barwidth / 2;  // Center of the bar
                pricenow = rawCurrentPrice;                                    // Use raw price, not averaged
                currentHour = rawCurrentHour;                                  // Use native interval's hour (not averaged block)
                currentMinute = rawCurrentMinute;                              // Use native interval's minute (not averaged block)
                currentBarHeight = thisbarh;                                   // Store bar height for line drawing
            }
        }
    }

    // Draw current hour pointer AFTER all bars (so it won't be chopped off)
    // Use constant narrow stem with wider arrow head for a proper arrow shape
    if (pointerX >= 0 && showcurrent) {
        // Arrow stem (narrow rectangle)
        spr.fillRect(pointerX - STEM_WIDTH / 2, 5 + arrowY, STEM_WIDTH, STEM_HEIGHT, imageParams.highlightColor);
        // Arrow head (wider triangle)
        spr.fillTriangle(pointerX - ARROW_WIDTH / 2, 15 + arrowY,
                         pointerX + ARROW_WIDTH / 2, 15 + arrowY,
                         pointerX, 15 + arrowY + ARROW_HEIGHT, imageParams.highlightColor);

        // Vertical line: dashed from arrow to bar, solid from baseline to bottom
        int lineStartY = 21 + arrowY + GAP_AFTER_ARROW;
        int baselineY = spr.height() - barBottom;
        int barTopY = baselineY - currentBarHeight;

        // Draw DASHED line from arrow to top of bar (with gap) - for visual clarity
        for (int y = lineStartY; y < barTopY - 2; y += DASH_LENGTH + GAP_LENGTH) {
            int segmentEnd = min(y + DASH_LENGTH, barTopY - 2);
            spr.drawLine(pointerX, y, pointerX, segmentEnd, TFT_BLACK);
        }

        // Draw solid line from baseline to bottom (for time indication in X-axis area)
        spr.drawLine(pointerX, baselineY, pointerX, spr.height(), TFT_BLACK);
    }

    if (showcurrent) {
        if (barwidth < 5) {
            drawString(spr, String(currentHour) + ":" + (currentMinute < 10 ? "0" : "") + String(currentMinute), spr.width() / 2, 5, "calibrib16.vlw", TC_DATUM, TFT_BLACK, 30);
            drawString(spr, String(pricenow) + "/kWh", spr.width() / 2, 25, loc["head"][0], TC_DATUM, TFT_BLACK, 30);
        } else {
            drawString(spr, String(currentHour) + ":" + (currentMinute < 10 ? "0" : "") + String(currentMinute), barX, 5, loc["head"][0], TL_DATUM, TFT_BLACK, 30);
            drawString(spr, String(pricenow) + "/kWh", spr.width() - barX, 5, loc["head"][0], TR_DATUM, TFT_BLACK, 30);
        }
    }

    spr2buffer(spr, filename, imageParams);
    spr.deleteSprite();
    return true;
}
#endif
