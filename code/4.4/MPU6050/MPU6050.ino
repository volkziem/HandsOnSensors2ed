// MPU6050, V. Ziemann, 221118
#include <Wire.h>
#include "I2Crw.h"
const int MPU6050=0x68;
void mpu6050_init() {  //.......................mpu6050_init
  Wire.begin();
  int whoami=I2Cread(MPU6050,0x75);
  if (whoami!=0x68) {
    Serial.println("No MPU6050 found!"); while (1) {yield();}
  } 
  I2Cwrite(MPU6050,0x6B,0); // wake up device
  I2Cwrite(MPU6050,0x1B,0); // Gyro config, FS=0
  I2Cwrite(MPU6050,0x1C,0); // Acc config, AFS=0
}
void mpu6050_read_float(int addr, float fdata[7]) { //...read_float
  int16_t intval;
  Wire.beginTransmission(addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(addr,14);
  intval=Wire.read()<<8 | Wire.read(); fdata[0]=intval/16.384; //ax
  intval=Wire.read()<<8 | Wire.read(); fdata[1]=intval/16.384; //ay
  intval=Wire.read()<<8 | Wire.read(); fdata[2]=intval/16.384; //ay
  intval=Wire.read()<<8 | Wire.read(); fdata[3]=intval/340.0+36.53; //T
  intval=Wire.read()<<8 | Wire.read(); fdata[4]=intval/131.0; //gx
  intval=Wire.read()<<8 | Wire.read(); fdata[5]=intval/131.0; //gy
  intval=Wire.read()<<8 | Wire.read(); fdata[6]=intval/131.0; //gz
  Wire.endTransmission();
}
void setup() {  //.................................setup
  Serial.begin(115200);
  while (!Serial) {yield();}
  mpu6050_init();
}
void loop() { //..................................loop
  float fdata[7];
  mpu6050_read_float(MPU6050,fdata);
  Serial.print(fdata[0],0); Serial.print("\t");
  Serial.print(fdata[1],0); Serial.print("\t");
  Serial.print(fdata[2],0); Serial.print("\t");
  Serial.print(fdata[4],0); Serial.print("\t");
  Serial.print(fdata[5],0); Serial.print("\t");
  Serial.print(fdata[6],0); Serial.print("\t"); 
  Serial.println(fdata[3]);
  delay(1000);
}
