#pragma once
#include <Arduino.h>

namespace util {

/// @brief Prints free heap, allocatbale heap and free stack
static void printHeap() {
    const uint32_t freeStack = uxTaskGetStackHighWaterMark(NULL);
    Serial.printf("Free heap: %d allocatable: %d stack: %d\n", ESP.getFreeHeap(), ESP.getMaxAllocHeap(), freeStack);
}

}  // namespace util
