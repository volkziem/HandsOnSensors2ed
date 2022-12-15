// MCP4921 DAC, V. Ziemann, 170801
#include <SPI.h>
#define CS 10
void setup() {  //...............................setup
  Serial.begin(9600);
  while (!Serial){;}
  pinMode(CS,OUTPUT); digitalWrite(CS,HIGH);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
}
void loop() {  //.................................loop
  char line[30];
  if (Serial.available()) {
    Serial.readStringUntil('\n').toCharArray(line,30);
    if (strstr(line,"DAC ")==line) {
      uint16_t val=(int)atof(&line[3]);
      val|=(B0011 << 12);  // 0=chA,0=unBuf,1=x1,1=ON
      digitalWrite(CS,LOW);
      SPI.transfer(highByte(val));
      SPI.transfer(lowByte(val));
      digitalWrite(CS,HIGH);
      delay(10);
      Serial.print("A0 "); Serial.println(analogRead(0));
    } else if (strstr(line,"A0?")==line) {
      Serial.print("A0 "); Serial.println(analogRead(0));
    } else {
      Serial.print("unknown: "); Serial.println(line);
    }
  }
}
