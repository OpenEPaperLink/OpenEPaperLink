/*******************************************************************************
 * Keyboard libraries:
 * M5Stack CardKB: no extra libraries required
 https://github.com/solderparty/arduino_bbq10kbd
 ******************************************************************************/

/* uncomment for M5Stack CardKB */
// #define KEYBOARD_CARDKB
// #define KEYBOARD_CARDKB_SDA 19
// #define KEYBOARD_CARDKB_SCL 20
// #define KEYBOARD_CARDKB_I2C_ADDR 0x5f

/* uncomment for BBQKeyboard */
// #define KEYBOARD_BBQKB
// #define KEYBOARD_BBQKB_SDA 21
// #define KEYBOARD_BBQKB_SCL 22

#if defined(KEYBOARD_CARDKB)
#include <Wire.h>
#elif defined(KEYBOARD_BBQKB)
#include <Wire.h>
#include <BBQ10Keyboard.h>
BBQ10Keyboard keyboard;
#endif

void keyboard_init() {
#if defined(KEYBOARD_CARDKB)
  Wire1.begin(KEYBOARD_CARDKB_SDA, KEYBOARD_CARDKB_SCL);
#elif defined(KEYBOARD_BBQKB)
  Wire1.begin(KEYBOARD_BBQKB_SDA, KEYBOARD_BBQKB_SCL);
  keyboard.begin();
  keyboard.setBacklight(0.5f);
#endif
}

char keyboard_get_key() {
#if defined(KEYBOARD_CARDKB)
  uint8_t bytesReceived = Wire1.requestFrom(KEYBOARD_CARDKB_I2C_ADDR, 1);
  if ((bool)bytesReceived) {  //If received more than zero bytes
    return Wire1.read();
  } else {
    return 0;
  }
#elif defined(KEYBOARD_BBQKB)
  const int keyCount = keyboard.keyCount();
  if (keyCount > 0) {
    const BBQ10Keyboard::KeyEvent key = keyboard.keyEvent();
    String state = "pressed";
    if (key.state == BBQ10Keyboard::StateLongPress)
      state = "held down";
    else if (key.state == BBQ10Keyboard::StateRelease)
      state = "released";

    Serial.printf("key: '%c' (dec %d, hex %02x) %s\r\n", key.key, key.key, key.key, state.c_str());

    if ((key.key != 0) && (key.state != BBQ10Keyboard::StateRelease)) {
      return key.key;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
#endif
}