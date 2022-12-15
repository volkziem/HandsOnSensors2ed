// BME680, V. Ziemann, 221006
//#include <Wire.h>
#include <Adafruit_BME680.h>
Adafruit_BME680 bme; 
void setup() { 
  Serial.begin(9600); while (!Serial);
  Wire.begin();
  if (!bme.begin()) {Serial.println("Error: BME680 not found");}
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
}

void loop() {
  bme.performReading();         // here the sensor is read out
  Serial.print(bme.temperature); Serial.print("\t");
  Serial.print(bme.pressure/100.0); Serial.print("\t");  // hPa=mbar
  Serial.print(bme.humidity); Serial.print("\t");
  Serial.println(bme.gas_resistance/1000.0);             // kOhm
  delay(1000);
}
