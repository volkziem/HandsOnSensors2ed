// MQTT client, V. Ziemann, 170816
const char* ssid = "messnetz";
const char* password = "zxcvZXCV";
const char* broker = "192.168.20.1";
const int fan_pin=D4;
#include <Ticker.h>
volatile uint8_t do_something=0;
Ticker tick;
void tick_action() {do_something=1;}  // executed regularly
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
WiFiClient espClient;
PubSubClient client(espClient);
void on_message(char* topic, byte* msg, unsigned int length) { //......
  Serial.print("Topic["); Serial.print(topic);  Serial.print("] = ");
  for (int i = 0; i < length; i++) {Serial.print((char)msg[i]);}
  Serial.println();
  char ch[30]; memcpy(ch,msg,length); ch[length]='\0';
  if (strstr(topic,"node1/fan")==topic) {
    int val=(int)atof(ch);
    Serial.print(" Fan="); Serial.println(val);
    if (val==0) {
      digitalWrite(fan_pin,HIGH);
    } else {
      digitalWrite(fan_pin,LOW);
    }
  } else if (strstr(topic,"node2/temp")==topic) {
    Serial.print("Temperature on node2="); Serial.println(ch);
  }
}
void setup() { //.................................setup
  pinMode(fan_pin,OUTPUT);
  Serial.begin(115200);
  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.print("\nWifi connected to "); Serial.println(ssid);
  Serial.print("with IP address: "); Serial.println(WiFi.localIP()); 
  client.setServer(broker, 1883);  // 1883 = default MQTT port
  client.setCallback(on_message);  // execute when a message arrives
  tick.attach(5,tick_action); // execute tick_action every 5 seconds
}
void loop() {  //..................................loop
  while (!client.connected()) {
    if (client.connect("PubSub1")) {  // identification 
      client.subscribe("node1/fan");  // external fan control
      client.subscribe("node2/temp"); // temp on other nodemcu
    } else {
 //   Serial.println("Trying to subscribe");
      delay(5000);
    }
  }
  client.loop();
  if (do_something) {
    do_something=0;
    char message[30];
    int temperature=(int)(3.3*100*analogRead(A0)/1023.0);
    sprintf(message,"%d",temperature);
    client.publish("node1/temp",message);
  }
}
