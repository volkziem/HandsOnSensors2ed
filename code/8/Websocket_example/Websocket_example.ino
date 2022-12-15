// websocket_example, V. Ziemann, 221017

const char* ssid     = "messnetz";
const char* password = "zxcvZXCV";

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "FS.h"
ESP8266WebServer server2(80);                       // port 80
WebSocketsServer webSocket = WebSocketsServer(81);  // port 81
//......................................................Ticker
#include <Ticker.h>
Ticker SampleSlow;
int sample_period=1,data_available=0,info_available=0;
void sampleslow_action() {data_available=1;}
char info_string[70];
volatile uint8_t websock_num=0;
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
          num, ip[0], ip[1], ip[2], ip[3],payload);
          strcpy(info_string,"Websocket opened on ESP8266"); info_available=1;
      }
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
        sample_period=val;
        SampleSlow.attach(sample_period,sampleslow_action);
        strcpy(info_string,"Acquisition started"); info_available=1;
      } else if (strstr(cmd,"STOP")) {
        SampleSlow.detach();
        strcpy(info_string,"Acquisition stopped"); info_available=1;
      } else {
        Serial.println("Unknown command");
      }
      break;
    }
//    case WStype_BIN:
//      break;
//    default:
//      break;
  }
}
//..................................................................setup
void setup() {
  Serial.begin(115200); delay(1000);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {delay(500); Serial.print(".");}
  Serial.print("\nConnected to ");  Serial.print(ssid);
  Serial.print(" with IP address: "); Serial.println(WiFi.localIP());
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  if (!SPIFFS.begin()) {
    Serial.println("ERROR: no SPIFFS filesystem found");
    return;
  } else {
    server2.begin();          
    server2.serveStatic("/", SPIFFS, "/webpage.html"); 
    Serial.println("SPIFFS file system found and server started on port 80");   
  }
}
//.................................................................loop
void loop() { 
  server2.handleClient();  // handle webserver on port 80
  webSocket.loop();        // handle websocket server on port 81
  if (data_available==1) {
    data_available=0;
    char buf[20],out[100];
    float temperature=analogRead(0)*100*3.3/1023.0;
    dtostrf(temperature,7,1,buf);
    (void) sprintf(out,"{\"%s\":\"%s\"}","TEMP",buf);
    webSocket.sendTXT(websock_num,out,strlen(out));
  }
  if (info_available==1) {
    info_available=0;
    char out[100];
    (void) sprintf(out,"{\"%s\":\"%s\"}","INFO",info_string);
    webSocket.sendTXT(websock_num,out,strlen(out));
  }
}
