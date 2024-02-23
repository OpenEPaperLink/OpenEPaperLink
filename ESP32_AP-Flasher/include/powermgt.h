#include <Arduino.h>

void powerControl(bool powerState, uint8_t* pin, uint8_t pincount);
void rampTagPower(uint8_t* pin, bool up);