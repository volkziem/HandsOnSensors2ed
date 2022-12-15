// MCP23017, V. Ziemann, 221118
#include <Wire.h>
#include "I2Crw.h"
#define MCP23017 0x20
//unsigned buttons,led_status=0;
volatile uint8_t pin_has_changed=0,portB=0,portA=0;
void action() {  //..................................action
  pin_has_changed=1;             
}
void setup() {  //..................................setup
  Wire.begin();                   // Init I2C bus
  I2Cwrite(MCP23017,0x00,0x00);   // IODIRA=all output
  I2Cwrite(MCP23017,0x0D,0xFF);   // GPPUB Pullups on
  I2Cwrite(MCP23017,0x03,0xFF);   // IPOLB reverse polarity
//  I2Cwrite(MCP23017,0x07,0xFF);   // DEFVALB, Default is HIGH
//  I2Cwrite(MCP23017,0x09,0xFF);   // INTCONB
  I2Cwrite(MCP23017,0x05,0xFF);   // GPINTENB, interrupts on 
  pinMode(2,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2),action,FALLING);
  Serial.begin(9600); while (!Serial) {;}
  Serial.println("Starting up");
}
void loop() {  //..................................loop
  if (Serial.available()) {
    char line[30];
    Serial.readStringUntil('\n').toCharArray(line,30);
    if (strstr(line,"A7 ")==line) {
      int val=(int)atof(&line[3]);
      if (val==0) {portA &= B01111111;} else {portA |= B10000000;}
      I2Cwrite(MCP23017,0x012,portA);  // write GPIOA
      Serial.print("PortA = "); Serial.println(portA,BIN);
    }
  }
  if (pin_has_changed) {
//  portB=I2Cread(MCP23017,0x13);  // read GPIOB
    portB=I2Cread(MCP23017,0x11);  // INTCAPB
    pin_has_changed=0;
    if (portB & B00000001) {
       Serial.println("Button B0 pressed");
    }
//    Serial.print("PORTB = "); Serial.println(portB,BIN);
  }
  delay(1);
}
