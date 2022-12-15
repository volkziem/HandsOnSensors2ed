// Minimal time-series-server, V. Ziemann, 170324

const char* ssid = "messnetz";
const char* password = "zxcvZXCV";
const int port = 1137;
#include <ESP8266WiFi.h>
WiFiServer server(port);

const uint16_t npts=1024;  // number of samples
const int sample_period=1; // ms

#include <Ticker.h>
Ticker SampleFast;
uint16_t sample_buffer[npts];
volatile uint16_t isamp=0,sample_buffer_ready=0;
char line[30];

void samplefast_action() { //.....................samplefast_action
  sample_buffer[isamp]=analogRead(0);
  isamp++;
  if (npts == isamp) {   
     SampleFast.detach();
     isamp=0;
     sample_buffer_ready=1;
  }
}

void setup() {  //...........................................setup
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);
  Serial.begin(115200);
  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("");
  Serial.print("Wifi connected to "); Serial.println(ssid);
  Serial.print("Server IP address: "); Serial.println(WiFi.localIP());
  server.begin();
  Serial.print("Server started on port "); Serial.println(port);
  digitalWrite(LED_BUILTIN,HIGH);
}

void loop() { //.............................................loop
  WiFiClient client = server.available();
  while (client) {
    while (!client.available()) {
      delay(5);
      if (!client.connected()) break;
    }
    client.readStringUntil('\n').toCharArray(line,30);
    Serial.print("Received: "); Serial.println(line);
    if (strstr(line,"TS?")) {
      digitalWrite(LED_BUILTIN,LOW);
      sample_buffer_ready=0;
      SampleFast.attach_ms(sample_period,samplefast_action);
      while (!sample_buffer_ready) {delay(2);} // wait until acquisition finished
      for (int i=0;i<npts;i++) {client.println(sample_buffer[i]);}
      digitalWrite(LED_BUILTIN,HIGH);
    } else if (strstr(line,"WF?")) {
      digitalWrite(LED_BUILTIN,LOW);
      sample_buffer_ready=0;
      SampleFast.attach_ms(sample_period,samplefast_action);
      while (!sample_buffer_ready) {delay(2);} // wait until acquisition finished
      for (int i=0;i<npts-1;i++) {
        client.print(sample_buffer[i]); client.print(", ");
      }
      client.println(sample_buffer[npts-1]);
      digitalWrite(LED_BUILTIN,HIGH); 
    } else if (strstr(line,"A0?")) {
      client.print("A0 "); client.println(analogRead(0));
    } else {
      Serial.println("unknown command, disconnecting");
      client.stop();
    }
    client.flush();  
  }
  yield();
}
