// HYT221 humidity sensor, V. Ziemann, 161203
#include <Wire.h>
const int HYT=0x28;  // I2C address
void setup() {  //............................setup
  Serial.begin (9600); while (!Serial) {;}
  Wire.begin();
}
void loop() {  //..............................loop
  int b1,b2,b3,b4,raw;
  double humidity,temp;
  Wire.beginTransmission(HYT);
  Wire.requestFrom(HYT,4); 
  if (Wire.available()==4) {
    b1=Wire.read(); b2=Wire.read();  // humidity rawdata
    b3=Wire.read(); b4=Wire.read();  // temperature rawdata
    Wire.endTransmission(true);
  }
  raw=(256*b1+b2) & 0x3FFF;       // humidity
  humidity=100.0*raw/16384.0;       
  raw=((256*b3+b4) & 0xFFFC)/4;  // temperature
  temp=165.0*raw/16384.0-40.0;
  Serial.print(humidity); Serial.print("\t");
  Serial.println(temp);
  delay(1000);
}
