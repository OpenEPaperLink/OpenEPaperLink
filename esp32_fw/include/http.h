#include <Arduino.h>

uint32_t getDataFromHTTP(uint8_t** data, String filename);
String postDataToHTTP(String json);