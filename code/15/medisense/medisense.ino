// MediSense, V. ziemann, 221106
const char* ssid     = "messnetz";
const char* password = "zxcvZXCV";
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <SPI.h>
ESP8266WebServer server2(80);                       // port 80
WebSocketsServer webSocket = WebSocketsServer(81);  // port 81
#include <Ticker.h>
Ticker SampleSlow,ECGticker;
#include <DFRobot_MAX30102.h>
DFRobot_MAX30102 Sensor;
#include "I2Crw.h"
#include "AD5933.h"
int16_t Ival[512],Qval[512]; float mag50=1, phase50=0;
volatile uint8_t websock_num=0,info_available=0,output_ready=0;
int mmode=0,sample_period=100,samples[3],icounter=0,recording=0;
int ringbuf[50],ibuf=0,ped=0;  // pedestal subtraction for oximeter
char info_buffer[80];          // for websock messages
char out[300]; DynamicJsonDocument doc(300);
float Rcal=10.0;               // calibration resistor
uint16_t ecgtrace[200],ecg_counter=0;;
#define CS D8                  // chip select for SPI-DAC
void handle_notfound() {  //.....................http server error handling
  server2.send(404,"text/plain","404 not found, just use http://ip-address/");
}

void cardio_action() {  //....................sampleslow_action
  samples[0]=analogRead(0)/2;  // adjust scale
  samples[1]=100*digitalRead(D3)+50*digitalRead(D4);
  samples[2]=0;
  output_ready=1; 
}
void oximeter_action() { //.........................oximeter_action
  float ir=0;
  if (mmode==3) {ir=(float)Sensor.getIR();
  } else if (mmode==4) {ir=(float)Sensor.getRed();}
  ringbuf[ibuf]=ir; ibuf++; if (ibuf>=50) {ibuf=0;} 
  ped=ringbuf[0]; for (int k=1;k<50;k++) {ped=min(ped,ringbuf[k]);}
  samples[0]=100+(ir-ped)/4.0;  // adjust scale
  icounter=icounter+1;
  if (icounter==200) {samples[1]=30000; icounter=0;} else {samples[1]=0;}
  samples[2]=0;
  output_ready=1;  
}
void impedance_action() {  //.....................impedance_action
  AD5933_sweep(50e3,0.01,0,mode,pga,Ival,Qval); 
  float mag=sqrt(Ival[0]*Ival[0]+Qval[0]*Qval[0]);
  float phase=atan2(Qval[0],Ival[0])*180/3.1415926;
  samples[0]=Rcal*(mag50/mag-1.0)*512.0;
//  samples[0]=128+128*log10(mag50/mag);
  samples[1]=256+25.6*(phase-phase50); 
  samples[2]=0;
  output_ready=1;
}
void set_dac(uint16_t val) {
  val|=(B0011 << 12);
  digitalWrite(CS,LOW);
  SPI.transfer(highByte(val));
  SPI.transfer(lowByte(val));
  digitalWrite(CS,HIGH);
}
void ecg_action() {   //...............................ecg_action
  set_dac(ecgtrace[ecg_counter]);
  ecg_counter++;
  if (ecg_counter==200) {ecg_counter=0;} 
}
void send_samples(int samples[]) {  //................send_samples
  for (int k=0;k<3;k++) {doc["ADC"][k]=samples[k];}
  serializeJson(doc,out); webSocket.sendTXT(websock_num,out,strlen(out));
}
void sendMSG(char *nam, const char *msg) { //..............sendMSG
  (void) sprintf(info_buffer,"{\"%s\":\"%s\"}",nam,msg);
  if (strstr(msg,"!")==msg) {
    webSocket.sendTXT(websock_num,info_buffer,strlen(info_buffer));
  } else {
    info_available=1;
  }
}
//...................................................................
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  Serial.printf("webSocketEvent(%d, %d, ...)\r\n", num, type);
  websock_num=num;
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\r\n", num);
      break;
    case WStype_CONNECTED: 
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\r\n", 
          num, ip[0], ip[1], ip[2], ip[3], payload);
      }
      sendMSG("INFO","ESP: Successfully connected");
      break;
    case WStype_TEXT:
    {
      Serial.printf("[%u] get Text: %s\r\n", num, payload);
      //........................................parse JSON
      DynamicJsonDocument root(300);
      deserializeJson(root,payload);
      const char *cmd = root["cmd"];
      const int val = root["val"];
      if (strstr(cmd,"START")) { 
        sendMSG("INFO","ESP: Received Start command");
        sample_period=val;
        Serial.print("sample_period = "); Serial.println(sample_period);
        if (mmode==0) {
          SampleSlow.attach_ms(sample_period,cardio_action);
        } else if (mmode==3 || mmode==4) {
          sample_period=max(50,sample_period); 
          SampleSlow.attach_ms(sample_period,oximeter_action);
        } else if (mmode==8) {
          SampleSlow.attach_ms(sample_period,impedance_action); 
        }
      } else if (strstr(cmd,"STOP")) {
         sendMSG("INFO","ESP: Received Stop command");
         SampleSlow.detach();
      } else if (strstr(cmd,"MMODE")) {      
        mmode=val;
        if (mmode==3 & mmode==4) {
           if (!Sensor.begin()) {sendMSG("INFO","ESP: Cannot find MAX30102"); mmode=-1;
          } else {
            Sensor.sensorConfiguration(60,SAMPLEAVG_8,MODE_MULTILED,SAMPLERATE_400,
              PULSEWIDTH_411,ADCRANGE_16384); 
          }
        } else if (mmode==5) {
          if (!Sensor.begin()) {sendMSG("INFO","ESP: Cannot find MAX30102"); mmode=-1;
          } else {
            sendMSG("INFO","ESP: MAX30102 is up and running"); 
            Sensor.sensorConfiguration(50,SAMPLEAVG_4,MODE_MULTILED,SAMPLERATE_100,
              PULSEWIDTH_411,ADCRANGE_16384); 
          }
        } else if (mmode==200) {
          recording=1; sendMSG("INFO","ESP: Recording ON");
        } else if (mmode==201) {
          recording=0; sendMSG("INFO","ESP: Recording OFF");
        } else if (mmode==300) { // Start ECG output
          ECGticker.attach_ms(5,ecg_action);
          sendMSG("INFO","ESP: Starting ECG output");
        } else if (mmode==301) { // Stop ECG output
          ECGticker.detach();
          set_dac(0);
          sendMSG("INFO","ESP: Stopping ECG output");
        }
      } else {
        Serial.println("Unknown command");
        sendMSG("INFO","ESP: Unknown command received");
      }
    }
  }
}
void listdir() {   //......................Directory listing
  Serial.println("\nFile list:");
  sendMSG("INFO","ESP: File listing sent to serial line"); 
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    Serial.print(dir.fileName()); Serial.print(" ");
    if(dir.fileSize()) {
      File f = dir.openFile("r");
      Serial.println(f.size());
    }
  }
}  
void setup() {   //........................................setup
  pinMode(D3,INPUT); 
  pinMode(D4,INPUT); 
  pinMode(D0,OUTPUT);  digitalWrite(D0,HIGH); // LED on NodeMCU 
  Serial.begin(115200);
  delay(1000);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {delay(500); Serial.print(".");}
  Serial.print("\nConnected to ");  Serial.print(ssid);
  Serial.print(" with IP address: "); Serial.println(WiFi.localIP());
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  if (!SPIFFS.begin()) {
    Serial.println("ERROR: no SPIFFS filesystem found");
  } else {
    server2.begin();          
    server2.serveStatic("/", SPIFFS, "/medisense.html");
    server2.onNotFound(handle_notfound);
    Serial.print("SPIFFS file system found and server started on port 80");   
    listdir();  
  }
  while (!Sensor.begin()) {Serial.println("Cannot find MAX30102"); delay(1000);}
  Sensor.sensorConfiguration(60,SAMPLEAVG_8,MODE_MULTILED,SAMPLERATE_200,
    PULSEWIDTH_411,ADCRANGE_16384); 
  freq=1e4; finc=200; npts=511;                // AD5933 frequency range
  File f=SPIFFS.open("/calib50kHz.dat","r");   // 50kHz calibration data
  if (f) {
    float re,im; char line [80];
    f.readStringUntil('\n').toCharArray(line,80); 
    sscanf(line," %g %g %g %g",&re,&im,&mag50,&phase50); 
//  Serial.println(line);
//  Serial.println(mag50); Serial.println(phase50);
    f.close();
  }
  f=SPIFFS.open("/ecg.dat","r");
  if (f) {
    uint16_t v; char line[20];
    for (int k=0;k<200;k++) {
      f.readStringUntil('\n').toCharArray(line,80); 
//    Serial.print(k); Serial.print("\t"); Serial.println(line);
      sscanf(line,"%d",&v);// Serial.println(v);
      ecgtrace[k]=v;
//    Serial.println(ecgtrace[k]);
    }
    f.close();
  }
  pinMode(CS,OUTPUT); digitalWrite(CS,HIGH);  // init SPI-CS
  SPI.begin(); SPI.setBitOrder(MSBFIRST);
}
//.................................................................loop
void loop() { 
  server2.handleClient();    // http server
  webSocket.loop();          // websocket
  if (info_available==1) {
    info_available=0;
    webSocket.sendTXT(websock_num,info_buffer,strlen(info_buffer));
  }
  if (output_ready==1) {
    output_ready=0;
    if (recording==0) {
      send_samples(samples);
//    Serial.print(samples[1]);  Serial.print("\t");  Serial.println(samples[2]); 
    } else if (recording==1) {      // record data on local file system
      File f=SPIFFS.open("/data.txt","a");   // open for append
      if (!f) {
        Serial.println("Unable To Open file");
      } else {
        f.print(samples[0]); f.print("\t"); f.println(samples[1]);
        f.close();
      }  
      sendMSG("INFO","ESP: Recording..."); 
    } 
  } 
  if (mmode==5) {   // SPO2 rate displayed on status line
    char msg[50];
    int32_t SPO2,HR;       // values
    int8_t SPO2_OK,HR_OK;  // data OK?
    Sensor.heartrateAndOxygenSaturation(&SPO2,&SPO2_OK,&HR,&HR_OK);
    sprintf(msg,"!ESP: SPO2,HR = %d, %d  (%d,%d)",SPO2,HR,SPO2_OK,HR_OK);
    sendMSG("INFO",msg);
  } else if (mmode==6) {     // AD5933 calibrate
    digitalWrite(D0,LOW);  //turns LED on
    sendMSG("INFO","!ESP calibrating..."); 
    AD5933_sweep(freq,finc,npts,mode,pga,Ival,Qval);
    sendMSG("INFO","ESP: AD5933 Calibration complete"); 
    digitalWrite(D0,HIGH);  //turns LED off
//  for (int k=0;k<npts+1;k++) {Serial.print(Ival[k]); Serial.print("\t"); Serial.println(Qval[k]);}
    File f=SPIFFS.open("/calib.dat","w");  
    for (int k=0;k<npts+1;k++) {
      f.print(Ival[k]); f.print("\t"); f.print(Qval[k]);
      f.print("\t"); f.print(sqrt(Ival[k]*Ival[k]+Qval[k]*Qval[k]));
      f.print("\t"); f.println(atan2(Qval[k],Ival[k])*180/3.1415926);
    } 
    f.close();
    AD5933_sweep(50e3,0.01,0,mode,pga,Ival,Qval);  // calibrate at 50 kHz
    f=SPIFFS.open("/calib50kHz.dat","w");
    f.print(Ival[0]); f.print("\t"); f.print(Qval[0]);
    f.print("\t"); f.print(sqrt(Ival[0]*Ival[0]+Qval[0]*Qval[0]));
    f.print("\t"); f.println(atan2(Qval[0],Ival[0])*180/3.1415926);
    f.close();
    mmode=-1;
  } else if (mmode==7) {     // AD5933 frequency sweep 
    char line[80]; 
    float re,im,ab,ph;  
    sendMSG("INFO","!ESP sweeping...");
    digitalWrite(D0,LOW);  //turns LED on
    AD5933_sweep(freq,finc,npts,mode,pga,Ival,Qval); 
    digitalWrite(D0,HIGH);  //turns LED on
    sendMSG("INFO","!ESP: AD5933 Sweep complete");
    File f=SPIFFS.open("/calib.dat","r");  
    for (int k=0;k<npts+1;k++) {
      float mag=sqrt(Ival[k]*Ival[k]+Qval[k]*Qval[k]);
      float phase=atan2(Qval[k],Ival[k])*180/3.1415926;
      f.readStringUntil('\n').toCharArray(line,80); 
      sscanf(line," %g %g %g %g",&re,&im,&ab,&ph); 
      samples[0]=Rcal*(ab/mag-1.0)*512.0;
      // samples[0]=128+128*log10(ab/mag); 
      samples[1]=256+25.6*(phase-ph); 
      samples[2]=0;
      send_samples(samples); send_samples(samples);
      Serial.print(ab/mag); Serial.print("\t"); Serial.print(phase-ph);
      Serial.print("\t"); Serial.println(samples[0]);
    }
    f.close();  
    mmode=-1; 
  } else if (mmode==100) {   // Directory listing
    mmode=-1;
    listdir();
  } else if (mmode==101) {   // Remove data file 
    SPIFFS.remove("/data.txt");
    sendMSG("INFO","ESP: Removing file /data.txt");  
    mmode=-1;
  } else if (mmode==202) {              // replay data from local file
    Serial.println("Replaying data");
    File f=SPIFFS.open("/data.txt","r");   // open for read
    if (!f) {
      Serial.println("Cannot open file /data.txt");
      sendMSG("INFO","ESP: cannot open file /data.txt"); 
    } else { 
      Serial.print("File size = "); Serial.println(f.size());   
      char msg[80]; sprintf(msg,"ESP: File size  of /data.txt = %d",f.size());
      sendMSG("INFO",msg); 
      char line[80]; 
      samples[2]=0;
      while (f.position()<f.size()) {
        f.readStringUntil('\n').toCharArray(line,80);
        sscanf(line," %d %d",&samples[0],&samples[1]); 
        send_samples(samples);
      }   
    }
    f.close();
    mmode=-1;
  } else if (mmode==999) { // Test something   
    Serial.println("MMODE=999 received");
    uint16_t val=(B0011 << 12) | random(4095);
    digitalWrite(CS,LOW);
    SPI.transfer(highByte(val));
    SPI.transfer(lowByte(val));
    digitalWrite(CS,HIGH);
    delay(1000);
//    mmode=-1;
  }
  yield();
}
