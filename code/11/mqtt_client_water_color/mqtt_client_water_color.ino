// MQTT_client_water_color, V. Ziemann, 170816
const char* ssid = "messnetz";
const char* password = "zxcvZXCV";
const char* broker = "192.168.20.1";
#include <Ticker.h>
volatile uint8_t do_something=0;
Ticker tick;
void tick_action() {do_something=1;}  // executed regularly
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
WiFiClient espClient;
PubSubClient client(espClient);
float measure(int pin, int repeat) {  //..............measure
  int hi,lo;
  float sum=0;
  digitalWrite(D2,HIGH);
  digitalWrite(D3,HIGH);
  digitalWrite(D4,HIGH);
  delay(10);
  for (int i=0;i<repeat;i++) {
    digitalWrite(pin,LOW);  
    delay(1); 
    lo=analogRead(A0);
    digitalWrite(pin,HIGH);
    delay(1); 
    hi=analogRead(A0);
    sum+=(hi-lo);
  }
  return sum/repeat;
}
void setup() { //.................................setup
  pinMode(D2,OUTPUT);
  pinMode(D3,OUTPUT);
  pinMode(D4,OUTPUT);
  Serial.begin(115200);
  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.print("\nWifi connected to "); Serial.println(ssid);
  Serial.print("with IP address: "); Serial.println(WiFi.localIP()); 
  client.setServer(broker, 1883);  // 1883 = default MQTT port
//  client.connect("PubSub1");
  tick.attach(10,tick_action); // execute tick_action every 10 seconds
}
void loop() {  //..................................loop
  while (!client.connected()) {  // try to connect to broker
    if (!client.connect("PubSub1")) {delay(5000);}
  }
  client.loop();
  if (do_something) {
    do_something=0;
    char message[30];
    float red=measure(D2,100);    // red
    float green=measure(D3,100);  // green
    float blue=measure(D4,100);   // blue
    sprintf(message,"%d %d %d",(int)red,(int)green,(int)blue);
    Serial.println(message);
    client.publish("node1/color",message);
  }
}
