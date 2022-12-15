// RCreceiver, V. Ziemann, 170701

#define Max(a,b) ((a)>(b)?(a):(b))
#define Min(a,b) ((a)<(b)?(a):(b))

const char *ap_ssid = "FireBot";
const char *ap_password = "........";
const int port=1137;

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
WiFiUDP server;

#include <Servo.h>
Servo myServo;

long lasttime=0,sleeptime=1000,nextevent=1;
int servo_pos=90,servo_inc=5;
int right_sensor=-1,left_sensor=-1;

void send_string(char line[]) {  //.......send_string
  server.beginPacket(server.remoteIP(),port);
  server.write(line);
  server.endPacket();
}
int range() { //..............................range
  digitalWrite(D1,LOW);
  delayMicroseconds(2);
  digitalWrite(D1,HIGH);
  delayMicroseconds(10);
  digitalWrite(D1,LOW);
  int val=(int)(0.017*pulseIn(D2,HIGH));
  if (val<10) tone(D8,1000,200); // whistle();
  return val;
}
void motor_speed(int left, int right) {  //...........motor_speed
  left=Max(-1023,Min(1023,left));
  analogWrite(D3,0);
  analogWrite(D4,0);
  if (left<0) {analogWrite(D3,abs(left));} else {analogWrite(D4,abs(left));} 
  right=Max(-1023,Min(1023,right));
  analogWrite(D5,0);
  analogWrite(D6,0);
  if (right<0) {analogWrite(D5,abs(right));} else {analogWrite(D6,abs(right));} 
}
void motor_stop() {  //...................motor_stop
  analogWrite(D3,0);  
  analogWrite(D4,0);
  analogWrite(D5,0);
  analogWrite(D6,0);
}
void setup() {  //..............................setup
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);
  pinMode(D1,OUTPUT);  // HCSR04-TRIG
  digitalWrite(D1,LOW);
  pinMode(D2,INPUT);   // HCSR04-ECHO
  pinMode(D3,OUTPUT);
  pinMode(D4,OUTPUT);
  analogWrite(D3,0);
  analogWrite(D4,0);
  pinMode(D5,OUTPUT);
  pinMode(D6,OUTPUT);
  analogWrite(D5,0);
  analogWrite(D6,0);
  Serial.begin(38400);
  WiFi.softAP(ap_ssid,ap_password);
  IPAddress myIP = WiFi.softAPIP();
  server.begin(port);
  Serial.print("\nAccess point and server started at address: ");
  Serial.print(myIP); Serial.print(" and port: "); Serial.println(port);
  Serial.print("with SSID: "); Serial.println(ap_ssid);
  pinMode(D7,OUTPUT);  // D7=Servo, D8=Tone
  myServo.attach(D7);
  digitalWrite(LED_BUILTIN,HIGH); 
  tone(D8,880,500);
  lasttime=millis();
}

void loop() {  //...................................loop
  char line[30];
  int packetsize=server.parsePacket();
  if (packetsize) {
//  Serial.print("packet received from "); Serial.println(server.remoteIP());
    int len=server.read(line,30);
//    Serial.print("len = "); Serial.println(len);
    line[len]='\0';
//    Serial.print("Request:"); Serial.println(line);
    if (strstr(line,"LSPEED ")) {
      int val=(int)atof(&line[7]);
      val=Max(-1023,Min(1023,val));
      analogWrite(D3,0);
      analogWrite(D4,0);
      if (val<0) {analogWrite(D3,abs(val));} else {analogWrite(D4,abs(val));}
    } else if (strstr(line,"RSPEED ")) {
      int val=(int)atof(&line[7]);
      val=Max(-1023,Min(1023,val));
      analogWrite(D5,0);
      analogWrite(D6,0);
      if (val<0) {analogWrite(D5,abs(val));} else {analogWrite(D6,abs(val));}
    } else if (strstr(line,":")) {
      line[len]='\n'; line[len+1]='\0';
      Serial.println(line);
    } else if (strstr(line,"D0 ")) {
      int val=(int)atof(&line[3]);
      if (val==0) {
        digitalWrite(LED_BUILTIN,HIGH);
      } else {
        digitalWrite(LED_BUILTIN,LOW);
      }
    } else if (strstr(line,"SERVO ")) {
      int val=(int)atof(&line[6]);
//    Serial.print("SERVO val= "); Serial.println(val);
      myServo.write(val);  
    } else if (strstr(line,"BEEP ")) {
      int val=(int)atof(&line[5]);
      Serial.print("BEEP val= "); Serial.println(val);
      tone(D8,440,val);   
    } else if (strstr(line,"A0?")) {
      int val=analogRead(A0);
      Serial.print("A0 "); Serial.println(val);
      sprintf(line,"A0 %d",val); send_string(line);
    } else if (strstr(line,"RANGE?")) {
      int val=range();
      sprintf(line,"RANGE %d",val); send_string(line);
    } else if (strstr(line,"SCANRANGE ")) {
      int val=(int)atof(&line[10]);
      int minval=2000,minpos=-1;
      if (val>0) {
        myServo.write(10);
        delay(1000);
        for (int k=10;k<170;k+=5) {
          myServo.write(k); delay(200);
          val=range();
          if (val<minval) { minval=val; minpos=k;}
          sprintf(line,"SCANRANGE %d %d",k,val); send_string(line);
        }
        myServo.write(minpos);
        sprintf(line,"MINIMUM at %d",minpos); send_string(line);
      } else {
        myServo.write(90);
      }
    } else if (strstr(line,"FINDFIRE ")) {
      int val=(int)atof(&line[9]);
      int minval=2000,minpos=-1;
      if (val>0) {
        myServo.write(10);
        delay(1000);
        for (int k=10;k<170;k+=5) {
          myServo.write(k); delay(200);
          val=analogRead(A0);
          if (val<minval) { minval=val; minpos=k;}
          sprintf(line,"FINDFIRE %d %d",k,val); send_string(line);
        }  
        myServo.write(minpos);
        sprintf(line,"MINIMUM at %d",minpos); send_string(line);
      } else {
        myServo.write(90);
      }
    } else if (strstr(line,"NEXTEVENT ")) {
      nextevent=(int)atof(&line[10]);
    } else if (strstr(line,"SLEEPTIME ")) {
      sleeptime=(int)atof(&line[10]);
    } else {
      Serial.println("unknown");
    }  
  }
  yield();
  if (millis()>lasttime+sleeptime) {  // next scheduled event
    switch (nextevent) {
      case 1:  // determine range
        sprintf(line,"RANGE %d",range()); send_string(line);
        sprintf(line,"A0 %d",analogRead(A0)); send_string(line);
        nextevent=1;
        break;  
      case 2:  // scan with servo
        servo_pos+=servo_inc;
        if (servo_pos>170) {servo_inc=-servo_inc;}
        if (servo_pos<10) {servo_inc=-servo_inc;}
        myServo.write(servo_pos);
        nextevent=2;
      case 3:  // request direction sensors
        if (range()>10) {
        Serial.println(":A0?\n:A1?");
        sleeptime=50; nextevent=4;
        } else {
          motor_stop();
          nextevent=1;
        }
        break;
      case 4:  // read direction sensors and take action
        if ((right_sensor>0) && (left_sensor>0)) { // new data
          if ((left_sensor<250) || (right_sensor<250)) {
            sprintf(line,"SENSORS %d %d",left_sensor,right_sensor);
            send_string(line);
            int val0=(int)(600+0.2*(right_sensor-left_sensor));
            val0=Max(-1023,Min(1023,val0));
            int val1=(int)(600-0.2*(right_sensor-left_sensor));
            val1=Max(-1023,Min(1023,val1));
            motor_speed(val0,val1);
          } else {
            motor_stop();
          }
          right_sensor=-1; left_sensor=-1;
          sleeptime=1000; nextevent=3;
        }
      default:
        break;
    }
    lasttime=millis();
  }
  yield();
  if (Serial.available()) {
    Serial.readStringUntil('\n').toCharArray(line,30);
    Serial.println(line);
    send_string(line);
    if (strstr(line,".A0 ")==line) {
      right_sensor=(int)atof(&line[3]); 
    } else if (strstr(line,".A1 ")==line) {  
      left_sensor=(int)atof(&line[3]); 
    }
  }
}
