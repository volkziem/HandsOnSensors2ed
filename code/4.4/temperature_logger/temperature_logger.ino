// ultra-simple LM35 temperature logger, V. Ziemann, 161013

float temp;
void setup() {
  analogReference(INTERNAL);  // 1.1V internal ADC reference
  Serial.begin (9600);
  while (!Serial) {;}
}
void loop() {
  temp=100*1.1*analogRead(0)/1023.0;  // 0.01 Volt/Celsius
  Serial.println(temp);
  delay(1000);
}
