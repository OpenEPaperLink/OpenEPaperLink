#include "Arduino.h"
#include <Touch_GT911.h>
#include <Wire.h>

Touch_GT911::Touch_GT911(uint8_t _sda, uint8_t _scl, uint16_t _width, uint16_t _height) :
  pinSda(_sda), pinScl(_scl), width(_width), height(_height) {
}
TPoint::TPoint(void) {
  id = x = y = size = 0;
}
TPoint::TPoint(uint8_t _id, uint16_t _x, uint16_t _y, uint16_t _size) {
  id = _id;
  x = _x;
  y = _y;
  size = _size;
}
bool TPoint::operator==(TPoint point) {
  return ((point.x == x) && (point.y == y) && (point.size == size));
}
bool TPoint::operator!=(TPoint point) {
  return ((point.x != x) || (point.y != y) || (point.size != size));
}
void Touch_GT911::begin(uint8_t _addr) {
  addr = _addr;
  Wire.begin(pinSda, pinScl);
}
void Touch_GT911::calculateChecksum() {
  uint8_t checksum;
  for (uint8_t i=0; i<GT911_CONFIG_SIZE; i++) {
    checksum += configBuf[i];
  }
  checksum = (~checksum) + 1;
  configBuf[GT911_CONFIG_CHKSUM - GT911_CONFIG_START] = checksum;
}
void Touch_GT911::reConfig() {
  calculateChecksum();
  writeByteData(GT911_CONFIG_CHKSUM, configBuf[GT911_CONFIG_CHKSUM-GT911_CONFIG_START]);
  writeByteData(GT911_CONFIG_FRESH, 1);
}
void Touch_GT911::setResolution(uint16_t _width, uint16_t _height) {
  configBuf[GT911_X_OUTPUT_MAX_LOW - GT911_CONFIG_START] = lowByte(_width);
  configBuf[GT911_X_OUTPUT_MAX_HIGH - GT911_CONFIG_START] = highByte(_width);
  configBuf[GT911_Y_OUTPUT_MAX_LOW - GT911_CONFIG_START] = lowByte(_height);
  configBuf[GT911_Y_OUTPUT_MAX_HIGH - GT911_CONFIG_START] = highByte(_height);
  reConfig();
}
void Touch_GT911::read(void) {
  uint8_t data[7];
  uint8_t id;
  uint16_t x, y, size;

  uint8_t pointInfo = readByteData(GT911_POINT_INFO);
  uint8_t bufferStatus = pointInfo >> 7 & 1;
  uint8_t proximityValid = pointInfo >> 5 & 1;
  uint8_t haveKey = pointInfo >> 4 & 1;
  isLargeDetect = pointInfo >> 6 & 1;
  touches = pointInfo & 0xF;
  isTouched = touches > 0;
  if (bufferStatus == 1 && isTouched) {
    for (uint8_t i=0; i<touches; i++) {
      readBlockData(data, GT911_POINT_1 + i * 8, 7);
      points[i] = readPoint(data);
    }
  }
  writeByteData(GT911_POINT_INFO, 0);
}
TPoint Touch_GT911::readPoint(uint8_t *data) {
  uint16_t temp;
  uint8_t id = data[0];
  uint16_t x = data[1] + (data[2] << 8);
  uint16_t y = data[3] + (data[4] << 8);
  uint16_t size = data[5] + (data[6] << 8);
  x = width - x;
  y = height - y;
  return TPoint(id, x, y, size);
}
void Touch_GT911::writeByteData(uint16_t reg, uint8_t val) {
  Wire.beginTransmission(addr);
  Wire.write(highByte(reg));
  Wire.write(lowByte(reg));
  Wire.write(val);
  Wire.endTransmission();
}
uint8_t Touch_GT911::readByteData(uint16_t reg) {
  uint8_t x;
  Wire.beginTransmission(addr);
  Wire.write(highByte(reg));
  Wire.write(lowByte(reg));
  Wire.endTransmission();
  Wire.requestFrom(addr, (uint8_t)1);
  x = Wire.read();
  return x;
}
void Touch_GT911::writeBlockData(uint16_t reg, uint8_t *val, uint8_t size) {
  Wire.beginTransmission(addr);
  Wire.write(highByte(reg));
  Wire.write(lowByte(reg));
  // Wire.write(val, size);
  for (uint8_t i=0; i<size; i++) {
    Wire.write(val[i]);
  }
  Wire.endTransmission();
}
void Touch_GT911::readBlockData(uint8_t *buf, uint16_t reg, uint8_t size) {
  Wire.beginTransmission(addr);
  Wire.write(highByte(reg));
  Wire.write(lowByte(reg));
  Wire.endTransmission();
  Wire.requestFrom(addr, size);
  for (uint8_t i=0; i<size; i++) {
    buf[i] = Wire.read();
  }
}

