// MCP3304, V. Ziemann, 170726
#include <SPI.h>
#define CS 15  // D8
int mcp3304_read_adc(int channel) {  //...0 to 3.......read_adc
  int adcvalue=0, b1=0, hi=0, lo=0, sign=0;;
  digitalWrite (CS, LOW);
  byte commandbits = B00001000; // Startbit+(diff=0)
  commandbits |= channel & 0x03;
  SPI.transfer(commandbits);
  b1 = SPI.transfer(0x00);     // always D0=0
  sign = b1 & B00010000;
  hi = b1 & B00001111;
  lo = SPI.transfer(0x00);     // input is don't care
  digitalWrite(CS, HIGH);
  adcvalue = (hi << 8) + lo;
  if (sign) {adcvalue = adcvalue-4096;}
  return adcvalue;
}
void setup() {//..............................setup
  pinMode(CS,OUTPUT); 
  SPI.begin();
  SPI.setFrequency(2100000);
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0); 
  Serial.begin(115200); 
  while (!Serial) {yield();}
}
void loop() {    //...........................loop
  int val=mcp3304_read_adc(0);
  Serial.print("CH0 = "); Serial.println(val); 
  delay(1000); 
}
