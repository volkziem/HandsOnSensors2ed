// Stepper controller, V. Ziemann, 170616
#include <Stepper.h>
Stepper myStepper(200, 2, 4, 3, 5);
char line[30];
void setup() {
  myStepper.setSpeed(60);
  Serial.begin (9600);
  while (!Serial) {;} 
}
void loop() {
  if (Serial.available()) {
    Serial.readStringUntil('\n').toCharArray(line,30);
    if (strstr(line,"MOVE ")==line) {
      int val=(int)atof(&line[5]);
      myStepper.step(val);
    } else {  
      Serial.println("unknown");
    }
  }
}
