// Weathernode-BME680, V. Ziemann, 221028
const char* ssid     = "messnetz";
const char* password = "zxcvZXCV";
const int port=1137;
#include <ESP8266WiFi.h>
WiFiServer server(port);
#include <Adafruit_BME680.h>
Adafruit_BME680 bme; 
char status[30] = "OK";
void setup() {   //.........................................setup
  Serial.begin(115200); delay(1000);
  IPAddress ip(192, 168, 20, 56);         // define static IP
  IPAddress gw(192, 168, 20, 1);          // gateway
  IPAddress subnet(255, 255, 255, 0);     // netmask
  WiFi.config(ip,gw,subnet);              // configure static IP
  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED) {delay(500); Serial.print(".");}
  Serial.print("\nConnected to ");  Serial.print(ssid);
  Serial.print(" with IP address: "); Serial.println(WiFi.localIP());
  server.begin();
  // Wire.begin(0,2);  // Wire.begin(4,5) is default on NodeMCU
  if (!bme.begin()) {  // default address
     strcpy(status,"Error: BME680 not found");
     Serial.println(status);
  }
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
}
void loop() { //............................................loop
  char line[30];
  WiFiClient client = server.available();
  while (client) {
    while(!client.available()) {
      delay(5);
      if (!client.connected()) {client.stop(); break;}
    }
    if (!client.connected()) {client.stop(); break;}
    client.readStringUntil('\n').toCharArray(line,30);
    bme.performReading();  
    float T=bme.temperature;              // Celsius
    float P=bme.pressure/100.0;           // hPa=mbar
    float H=bme.humidity;                 // percent
    float VOC=bme.gas_resistance/1000.0;  // kOhm 
    if (strstr(line,"V?")==line) {
      client.print("WeatherNode-BME680, Status ");
      client.println(status);
    } else if (strstr(line,"T?")==line) {
      client.print("T "); client.println(T,2);
//   sprintf(line,"T %.2f",T); client.println(line);
    } else if (strstr(line,"P?")==line) {
      client.print("P "); client.println(P,2);
//    sprintf(line,"P %.2f",P); client.println(line);
    } else if (strstr(line,"H?")==line) {
      client.print("H "); client.println(H,2);
//    sprintf(line,"H %.1f",H); client.println(line);
    } else if (strstr(line,"VOC?")==line) {
      client.print("VOC "); client.println(VOC,2);     
//      sprintf(line,"VOC %.2f",VOC); client.println(line);
    } else if (strstr(line,"ALL?")==line) {
      sprintf(line,"%.2f:%.2f:%.1f:%.2f",T,P,H,VOC); client.print(line);
      client.flush(); client.stop();
    } else {
      client.println("Unknown command, disconnecting!");
      client.flush(); client.stop();
    }    
    yield();
  } 
}
