// I2Crw.h, V. Ziemann, 221118
void I2Cwrite(int addr, uint8_t reg, uint8_t val) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission(true);
}
uint8_t I2Cread(int addr, uint8_t reg) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(addr,1);
  return Wire.read();
}
