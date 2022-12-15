//Neopixel thermometer, V. Ziemann, 221115
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 2  // pin 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
//...................neopixel
#include <Adafruit_NeoPixel.h>
#define NUMPIXELS 8
#define PIN 6 
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
void setup() {
//  Serial.begin(115200); delay(2000);
  sensors.begin();
  pixels.begin();  
}
void loop() {
  sensors.requestTemperatures();
  float temp=sensors.getTempCByIndex(0);
  int imax=(int)temp-20;
//  Serial.print(temp); Serial.print('\t'); Serial.println(imax);
  pixels.clear();
  for (int i=0;i<imax;i++) {
    pixels.setPixelColor(i, pixels.Color(20+10*i,20, 100-10*i));
  }
  pixels.show();   
  delay(1000);
}
