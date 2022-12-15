// Switching_and_pwm, V. Ziemann, 170614
char line[30];
void setup() {
  pinMode(2,OUTPUT);
  Serial.begin (9600);
  while (!Serial) {;} 
}
void loop() {
  if (Serial.available()) {
    Serial.readStringUntil('\n').toCharArray(line,30);
    if (strstr(line,"DW2 ")==line) {
      int val=(int)atof(&line[4]);
      digitalWrite(2,val);
    } else if (strstr(line,"AW9 ")==line) {
      int val=(int)atof(&line[4]);
      analogWrite(9,val);  
    } else {
      Serial.println("unknown");
    }
  }
}
