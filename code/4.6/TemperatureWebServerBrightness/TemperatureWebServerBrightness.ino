// TemperatureWebServerBrightness, V. Ziemann, 221214
#include <ESP8266WiFi.h>
const char* ssid     = "MyHomeNet";
const char* password = ".........";
WiFiServer server(80);  // server listens on html port 80
void setup() {
  pinMode(D0,OUTPUT);    // D0=GPIO16=LED_BUILTIN on NodeMCU
  Serial.begin(115200); delay(10);
  Serial.print("Connecting to "); Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.print("\nWiFi connected and ");
  server.begin();
  Serial.print("server started at ");
  Serial.println(WiFi.localIP());
}
void loop() {
  WiFiClient client = server.available();
  while (client) {
    Serial.println("new client");
    while(!client.available()){delay(1);}
    String req = client.readStringUntil('\r');
    Serial.println(req);
    client.flush();
    client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html");
    client.print("\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n");
    if (req.indexOf("/temperature") != -1) {
      float temp=100*3.3*analogRead(0)/1023;
      client.print("Temperature="); client.print(temp,2);
    } else if (req.indexOf("/led") != -1) {
      int i1=req.indexOf("?b="); int i2=req.indexOf("HTTP"); 
      String payload=req.substring(i1+3,i2-1);
      Serial.println(payload);
      if (i1>0) analogWrite(D4,1023-payload.toInt());  
      client.print("Setting LED brightness to "); client.print(payload); 
    }
    client.println("</html>"); delay(1);
    client.stop();
  }
}
