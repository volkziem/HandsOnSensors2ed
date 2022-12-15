// DS18b20 1-wire temperature sensor, V. Ziemann, 170120 
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 2  // pin 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
void setup() {
  Serial.begin(115200); while (!Serial) {;}
  sensors.begin();
}
void loop() {
  sensors.requestTemperatures();
  float temp=sensors.getTempCByIndex(0);
  Serial.println(temp); 
  delay(200);
}
