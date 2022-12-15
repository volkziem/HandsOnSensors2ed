// query-response, V. Ziemann, 161013

int val;
char line[30];

void setup() {
  pinMode(13,OUTPUT);
//  analogReference(INTERNAL);  // 1.1V internal ADC reference
  Serial.begin (9600);
  while (!Serial) {;}
}

void loop() {
  if (Serial.available()) {
    Serial.readStringUntil('\n').toCharArray(line,30);
    if (strstr(line,"A0?")) {
      val=analogRead(0); 
      Serial.print("A0 "); 
      Serial.println(5.0*val/1023);
    } else if (strstr(line,"A1?")) {
      val=analogRead(1); 
      Serial.print("A1 "); 
      Serial.println(5.0*val/1023);
    } else if (strstr(line,"T?")) {
      val=analogRead(0); 
      Serial.print("T "); 
      Serial.println(100*5.0*val/1023,1);
    } else {
      Serial.println("unknown");
    }
  }
}
