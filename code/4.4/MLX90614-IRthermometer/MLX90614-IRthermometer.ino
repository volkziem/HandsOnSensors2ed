// Read MLX90614 IRthermometer, V. Ziemann, 170717
#include <Wire.h>
const int MLX90614=0x5A;   // I2C address
float getTemperature(uint8_t addr) {  //.....getTemperature
  uint16_t val;
  uint8_t crc;
  Wire.beginTransmission(MLX90614); 
  Wire.write(addr);  // address 
  Wire.endTransmission(false);
  Wire.requestFrom(MLX90614,3);
  val = Wire.read();
  val |= (Wire.read()<<8);
  crc=Wire.read(); // not used
  return -273.15+0.02*(float)val;
}
void setup() { //....................................setup
  Serial.begin(9600);
  Wire.begin();
}
void loop() {  //.....................................loop
  float Ta=getTemperature(0x06);  // address for ambient temp
  float To=getTemperature(0x07);  // address for object1 temp
  Serial.print("Ta = "); Serial.print(Ta);
  Serial.print("\t"); 
  Serial.print("To = "); Serial.println(To);
  delay(1000);
}
