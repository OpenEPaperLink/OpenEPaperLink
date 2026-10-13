// Content: image from a URL.
// Included once by contentmanager.cpp, not compiled on its own.

int getImgURL(String &filename, String URL, time_t fetched, imgParam &imageParams, String MAC) {
    // https://images.klari.net/kat-bw29.jpg

    HTTPClient http;
    logLine("http getImgURL " + URL);
    http.begin(URL);
    http.addHeader("If-Modified-Since", formatHttpDate(fetched));
    http.addHeader("X-ESL-MAC", MAC);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    http.setTimeout(5000);  // timeout in ms
    const int httpCode = http.GET();
    if (httpCode == 200) {
        // LockedFile: don't hold fsMutex while waiting for the network
        LockedFile f("/temp/temp.jpg", "w");
        if (f) {
            http.writeToStream(&f);
            f.close();
            jpg2buffer("/temp/temp.jpg", filename, imageParams);
        }
    } else {
        if (httpCode != 304) {
            wsErr("http " + URL + " " + String(httpCode));
            logLineRateLimited(urlForLog(URL), "http error " + String(httpCode) + " " + urlForLog(URL));
        }
    }
    http.end();
    return httpCode;
}
