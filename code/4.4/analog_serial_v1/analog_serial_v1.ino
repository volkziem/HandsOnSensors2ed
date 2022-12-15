// Analog and Serial comm
int inp,val;
void setup() {
  Serial.begin(9600);  // baud rate
}
void loop() {
  if (Serial.available()) {    
     inp=Serial.read();          // read serial line
     val=analogRead(0);          // read analog 
     Serial.print("Value is ");  // and report back   
     Serial.println(val);
  }
  delay(50);                     // wait a short while
}
