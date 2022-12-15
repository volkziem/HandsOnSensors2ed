// Servo controller, V. Ziemann, 170614
#include <Servo.h>
Servo myServo;
char line[30];
void setup() {
  myServo.attach(9);   // pin D9
  Serial.begin (9600);
  while (!Serial) {;} 
}
void loop() {
  if (Serial.available()) {
    Serial.readStringUntil('\n').toCharArray(line,30);
    if (strstr(line,"SERVO ")==line) {
      float val=atof(&line[6]);
      myServo.write((int) val);  // 0 to 180    
    }
  }
}
