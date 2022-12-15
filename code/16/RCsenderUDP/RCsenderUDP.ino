// RCsenderUDP, V. Ziemann, 170705

const char* ssid     = "FireBot";
const char* password = "........";
const char* host = "192.168.4.1"; 
const int port=1137;
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
WiFiUDP client;

int adclast[8]={0,0,0,0,0,0,0,0},D3last=HIGH,D4last=HIGH;
int adccalib[8];

//...................................................ADC
#include <SPI.h>
#define CS 15
int mcp3208_read_adc(uint8_t channel) {  // 8 single ended
  int adcvalue=0, b1=0, hi=0, lo=0, reading;
  digitalWrite (CS, LOW);
  byte commandbits = B00001100; // Startbit+(single ended=1)
  commandbits |= ((channel>>1) & 0x03);
  SPI.transfer(commandbits);
  commandbits=(channel & 0x01) << 7;
  b1 = SPI.transfer(commandbits);     
  hi = b1 & B00011111;
  lo = SPI.transfer(0x00);     // input is don't care
  digitalWrite(CS, HIGH);
  reading = (hi << 7) + (lo >> 1);
  return reading;
}
void send_string(char line[]) {  //............send_string
  client.beginPacket(host,port);
  client.write(line);
  client.endPacket();
}
void setup() {  //..................................setup
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);
  pinMode(CS,OUTPUT);    
  digitalWrite(CS,HIGH); 
  SPI.begin();
  SPI.setFrequency(100000);
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0); 
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW); 
  Serial.begin(115200);
  pinMode(D3,INPUT_PULLUP);
  pinMode(D4,INPUT_PULLUP);
  delay(1000);
  WiFi.mode(WIFI_STA);  // needed for reliable communication
  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("."); delay(500);
  }
  Serial.print("\nConnected to ");  Serial.print(ssid);
  Serial.print(" with IP address: "); 
  Serial.println(WiFi.localIP());
  for (int k=0;k<8;k++) {
    adccalib[k]=mcp3208_read_adc(k);
  }
  client.begin(port);
  digitalWrite(LED_BUILTIN,HIGH); 
}
void loop() {  //..................................loop
  char line[30];
  int adc[8];
  for (int k=0;k<8;k++) {adc[k]=mcp3208_read_adc(k);}
  if ((abs(adc[0]-adclast[0]) > 16) || (abs(adc[1]-adclast[1]) > 16)) {
    adclast[0]=adc[0]; adclast[1]=adc[1];
    int val0=(adc[0]-adccalib[0])*1023.0/2048.0;
    int val1=(adc[1]-adccalib[1])*1023.0/2048.0;
    sprintf(line,"RSPEED %d",(int)(val0+0.5*val1)); send_string(line);
    sprintf(line,"LSPEED %d",(int)(val0-0.5*val1)); send_string(line);
  }
  /*
  if (abs(adc[0]-adclast[0]) > 16) {
    adclast[0]=adc[0];
    Serial.print("ADC[0] = "); Serial.println(adc[0]);
    int val=(adc[0]-adccalib[0])*1023.0/2048.0;
    sprintf(line,"RSPEED %d",val); send_string(line);
  }
  if (abs(adc[1]-adclast[1]) > 16) {
    adclast[1]=adc[1];
    Serial.print("ADC[1] = "); Serial.println(adc[1]);
    int val=adc[1]*180.0/4095;
  }
  */
  if (abs(adc[2]-adclast[2]) > 16) {
//    Serial.print("ADC[2] = "); Serial.println(adc[2]);
    adclast[2]=adc[2];
    int val=(adc[2]-adccalib[2])*1023.0/2048.0;
    sprintf(line,"LSPEED %d",val); send_string(line);   
  }
  if (abs(adc[3]-adclast[3]) > 16) {
//    Serial.print("ADC[3] = "); Serial.println(adc[3]);
    adclast[3]=adc[3];
    int val=adc[3]*180.0/4095;
  } 
  if (abs(adc[4]-adclast[4]) > 16) {
    adclast[4]=adc[4];
//    Serial.print("ADC[4] = "); Serial.println(adc[4]);
    int val=adc[4]*180.0/4095;
  }
  if (abs(adc[5]-adclast[5]) > 16) {
    adclast[5]=adc[5];
//    Serial.print("ADC[5] = "); Serial.println(adc[5]);
    int val=adc[5]*180.0/4095;
    sprintf(line,"SERVO %d",val); send_string(line); 
  }
  if (abs(adc[7]-adclast[7]) > 5) {  //  check the buttons
    adclast[7]=adc[7];
//  Serial.print("ADC[7] = "); Serial.println(adc[7]);
    if (adc[7] < 1000) { // red button right
      Serial.println("Red button right pressed");
      sprintf(line,"FINDFIRE 1"); send_string(line); 
    } else if (adc[7] < 1700) {
      Serial.println("Blue button right pressed");
      sprintf(line,"RANGE?"); send_string(line); 
    } else if (adc[7] < 2250) {
      Serial.println("Joystick button right pressed");
      sprintf(line,"NEXTEVENT 0"); send_string(line); 
    } else if (adc[7] < 3580) {
      Serial.println("Joystick button left pressed");
      sprintf(line,"NEXTEVENT 1"); send_string(line); 
    } else if (adc[7] < 3660) {
      Serial.println("Blue button left pressed");
      sprintf(line,"BEEP 1000"); send_string(line); 
      Serial.println(line);
    } else if (adc[7] < 3750) {
      Serial.println("Red button left pressed"); 
      sprintf(line,"NEXTEVENT 3"); send_string(line); 
    }
  }
  if (digitalRead(D3) != D3last) {
    D3last=digitalRead(D3);
    Serial.print("Left switch toggled to "); Serial.println(D3last);
    sprintf(line,"D0 %d",D3last); send_string(line);
  }
  if (digitalRead(D4) != D4last) {
    D4last=digitalRead(D4);
    Serial.print("Right switch toggled to "); Serial.println(D4last);
  }
  yield();
  int packetsize=client.parsePacket();
  if (packetsize) {
    char line[30];
//  Serial.print("packet received from "); Serial.println(client.remoteIP());
    int len=client.read(line,30); line[len]='\0';
    Serial.print("Message:"); Serial.println(line);
  }
  if (Serial.available()) {
    Serial.readStringUntil('\n').toCharArray(line,30);
    send_string(line);
  }
}
