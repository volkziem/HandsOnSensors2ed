// Socket-based measurement server, V. Ziemann, 161211
const char* ssid     = "MyHomeNet";
const char* password = ".........";
const int port = 1137;
#include <ESP8266WiFi.h>
WiFiServer server(port);
void setup() { //...........................................setup
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected to "); Serial.println(ssid);
  Serial.print("Server IP address: "); Serial.println(WiFi.localIP());
  server.begin();          
  Serial.print("Server started on port "); Serial.println(port);
}
void loop() {  //............................................loop
  char line[30];
  float volt,temp;
  WiFiClient client = server.available();
  while (client) {
    while(!client.available()) {delay(1);}
    client.readStringUntil('\n').toCharArray(line,30);
    Serial.print("Request: "); Serial.println(line);
    if (strstr(line,"A0?")) {
      volt=3.3*analogRead(0)/1023;
      client.print("A0 "); client.println(volt);
    } else if (strstr(line,"T?")) {
      temp=100*3.3*analogRead(0)/1023;
      client.print("T "); client.println(temp,1);
    } else {
      Serial.println("unknown command, disconnecting");
      client.stop();
    }
    client.flush();
//  client.stop();
  }
}
