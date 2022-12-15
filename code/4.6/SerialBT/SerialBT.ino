// SerialBT, V. Ziemann, 221101
#include <BluetoothSerial.h>
BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200); delay(1000);  Serial.println("Pairing");
  SerialBT.begin("ESP32BT2"); 
//  SerialBT.setPin("2345");
}

void loop() {
  if (SerialBT.available()) {
    char line[30];
    SerialBT.readStringUntil('\n').toCharArray(line,30);
    if (strstr(line,"A0?")) {
      int val=analogRead(33); 
      SerialBT.print("A0 "); SerialBT.println(3.3*val/4095);
    } else {
      SerialBT.println("unknown");
    }
  }
}
