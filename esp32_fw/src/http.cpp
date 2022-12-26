#include "http.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>

#include "settings.h"

#ifdef NETWORK_MODE
uint32_t getDataFromHTTP(uint8_t** data, String filename) {
    HTTPClient http;
    uint16_t ret = 0;
    http.begin(WEBSERVER_PATH + filename);
    int httpCode = http.GET();

    if (httpCode > 0) {
        String payload = http.getString();
        ret = http.getSize();
        *data = (uint8_t*)calloc(ret, 1);
        memcpy(*data, payload.begin(), ret);
    } else {
        data = 0;
        Serial.printf("Error on HTTP request! Tried to get %s but something went TERRIBLY wrong.\n", filename);
    }
    http.end();  // Free the resources
    return ret;
}

String postDataToHTTP(String json) {
    HTTPClient http;
    http.begin(CHECK_IN_URL);
    int httpCode = http.POST(json);
    String ret = "";
    if (httpCode > 0) {
        ret = http.getString();
    }
    http.end();  // Free the resources
    return ret;
}
#endif
