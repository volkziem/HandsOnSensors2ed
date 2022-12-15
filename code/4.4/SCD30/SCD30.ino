// Sensirion SCD30 CO2 concentration, V. Ziemann, 221006
#include <Wire.h>
const int SCD30=0x61;
char line[30];
int running=0;

int data_ready() {
  byte b1,b2,b3;   
  Wire.beginTransmission(SCD30);
  Wire.write(0x02); Wire.write(0x02);
  Wire.endTransmission();
  Wire.requestFrom(SCD30, 3); 
  b1 = Wire.read(); b2 = Wire.read(); b3= Wire.read(); 
  return  (b1<<8) | b2;    
}

void setup() {
  Serial.begin(9600); while(!Serial){;}
  Wire.begin();
}

void loop() {
  if (Serial.available()){
    Serial.readStringUntil('\n').toCharArray(line,30);
    if (strstr(line,"V?")) { // Get firmware version
      byte  b1,b2,b3;     
      Wire.beginTransmission(SCD30);
      Wire.write(0xD1); Wire.write(0x00);
      Wire.endTransmission();
      Wire.requestFrom(SCD30,3);
      b1 = Wire.read(); b2 = Wire.read(); b3=Wire.read(); 
      Serial.print("V "); Serial.print(b1);
      Serial.print("."); Serial.println(b2);
    } else if (strstr(line,"Start")) {  // Start data taking
      Wire.beginTransmission(SCD30);
      Wire.write(0x00); Wire.write(0x10); // two command bytes
      Wire.write(0x00); Wire.write(0x00); // ambient pressure
      Wire.write(0x81); //CRC
      Wire.endTransmission(); 
      running=1;  
    } else if (strstr(line,"Stop")) {
      Wire.beginTransmission(SCD30);
      Wire.write(0x01); Wire.write(0x04); // two command bytes
      Wire.endTransmission();
      running=0;
    }      
  }  
  if (running) {   // measure
    unsigned long  b1,b2,b3,b4,b5,b6,raw; 
    float CO2,temp,hum;     
    while (!data_ready()) {delay(10);}  // wait until data ready  
    delay(20);    
    Wire.beginTransmission(SCD30);
    Wire.write(0x03); Wire.write(0x00);  // command bytes
    Wire.endTransmission();
    Wire.requestFrom(SCD30,18);
    b1 = Wire.read(); b2 = Wire.read(); b3 = Wire.read();  
    b4 = Wire.read(); b5 = Wire.read(); b6 = Wire.read(); 
    raw=(b1<<24) | (b2<<16) | (b4<<8) | b5;  // CO2
    CO2=*(float*)&raw;
    b1 = Wire.read(); b2 = Wire.read(); b3 = Wire.read();  
    b4 = Wire.read(); b5 = Wire.read(); b6 = Wire.read(); 
    raw=(b1<<24) | (b2<<16) | (b4<<8) | b5;  // temperature
    temp=*(float*)&raw;  
    b1 = Wire.read(); b2 = Wire.read(); b3 = Wire.read();  
    b4 = Wire.read(); b5 = Wire.read(); b6 = Wire.read(); 
    raw=(b1<<24) | (b2<<16) | (b4<<8) | b5;  // temperature
    hum=*(float*)&raw;    
    Serial.print(CO2); Serial.print("\t");
    Serial.print(temp); Serial.print("\t"); Serial.println(hum); 
    delay(2000);    
  }
}
