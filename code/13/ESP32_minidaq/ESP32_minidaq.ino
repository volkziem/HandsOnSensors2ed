// ESP32 DAQ, V. ziemann, 221018
const char* ssid     = "messnetz";
const char* password = "zxcvZXCV";
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
WebServer server2(80);                              // port 80
WebSocketsServer webSocket = WebSocketsServer(81);  // port 81
#include <Ticker.h>
Ticker SampleSlow,Ramp25Ticker,Pulse18Ticker;
volatile uint8_t websock_num=0,info_available=0,output_ready=0;
int sample_period=100,samples[3];
uint8_t dac25val=0,do18=0;
char info_buffer[80];
char out[300]; DynamicJsonDocument doc(300);
//......................................................sampleslow_action
void sampleslow_action() {
  samples[0]=analogRead(32)/8;
  samples[1]=analogRead(33)/8;
  samples[2]=digitalRead(21);
  output_ready=1;
}
//..................................................................
void ramp25_action() {
  dac25val++; dacWrite(25,dac25val);
}
//..................................................................
void pulse18_action() {
  do18=!do18; digitalWrite(18,do18);
}
//..............................................................http server
void handle_notfound() {
  server2.send(404,"text/plain","not found, use http://ip-address/");
}
//..............................................................websocket
void sendMSG(char *nam, const char *msg) {
  (void) sprintf(info_buffer,"{\"%s\":\"%s\"}",nam,msg);
  info_available=1;
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
  //      char msg[100]="Connected to sensor";
  //      char buf[20],out[100];  
      }
      sendMSG("INFO","ESP32: Successfully connected");
      break;
    case WStype_TEXT:
    {
      Serial.printf("[%u] get Text: %s\r\n", num, payload);
      //........................................parse JSON
      DynamicJsonDocument root(300);
      deserializeJson(root,payload);
      const char *cmd = root["cmd"];
      const long val = root["val"];
      if (strstr(cmd,"START")) { 
        sendMSG("INFO","ESP32: Received Start command");
        sample_period=val;
        SampleSlow.attach_ms(sample_period,sampleslow_action);
        Serial.print("sample_period = "); Serial.println(sample_period);
      } else if (strstr(cmd,"STOP")) {
         sendMSG("INFO","ESP32: Received Stop command");
         SampleSlow.detach();
      } else if (strstr(cmd,"DAC25")) {
        dacWrite(25,val);
        sendMSG("INFO","ESP32: set DAC25 to requested value");      
      } else if (strstr(cmd,"DO18")) { digitalWrite(18,val); 
      } else if (strstr(cmd,"RAMP25")) {
        if (val > 0) {Ramp25Ticker.attach_ms(val,ramp25_action);
        } else {Ramp25Ticker.detach();}
       } else if (strstr(cmd,"PULSE18")) {
        if (val > 0) {Pulse18Ticker.attach_ms(val,pulse18_action);
        } else {Pulse18Ticker.detach();}
      } else {
        Serial.println("Unknown command");
        sendMSG("INFO","ESP32: Unknown command received");
      }
//      break;
    }
  }
}

//..................................................................setup
void setup() {
  pinMode(21,INPUT_PULLUP); 
  pinMode(18,OUTPUT); 
  dacWrite(25,0); // initialize DAC
  Serial.begin(115200);
  delay(1000);
  //..............................................client mode
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {delay(500); Serial.print(".");}
  Serial.print("\nConnected to ");  Serial.print(ssid);
  Serial.print(" with IP address: "); Serial.println(WiFi.localIP());
  //........................................access point mode
/*
  WiFi.softAP("ESP32DAQ",password);
  Serial.print("Access point ESP32-DAQ started at IP ");
  Serial.println(WiFi.softAPIP());
*/
  //.........................................................
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  //.....................................................http server
  if (!SPIFFS.begin()) {
    Serial.println("ERROR: no SPIFFS filesystem found");
    return;
  } else {
    server2.begin();          
    server2.serveStatic("/", SPIFFS, "/esp32-daq.html");
    server2.onNotFound(handle_notfound);
    Serial.print("SPIFFS file system found and server started on port 80");     
  }
  //delay(1000);
  //dacWrite(25,0); // initialize DAC
}
//.................................................................loop
void loop() { 
  server2.handleClient();    //..........................http server
  webSocket.loop();
  if (info_available==1) {
    info_available=0;
    webSocket.sendTXT(websock_num,info_buffer,strlen(info_buffer));
  }
  if (output_ready==1) {
    output_ready=0;
    doc.to<JsonObject>();  // clear doc
    for (int k=0;k<3;k++) {doc["ADC"][k]=samples[k];}
    serializeJson(doc,out);
    webSocket.sendTXT(websock_num,out,strlen(out));
  }
}
