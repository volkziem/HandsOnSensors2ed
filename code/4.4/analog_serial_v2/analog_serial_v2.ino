// Analog and Serial communication, v2, 161130

int inp,val;
void setup() {
  Serial.begin(9600);  // baud rate
}

void loop() {
  if (Serial.available()) {    
     inp=Serial.read();       // read serial line
     if (inp=='t') {
       val=analogRead(0);     // read analog 
       Serial.print("A0= ");  // and report back   
       Serial.println(val);
     } else if (inp=='s') {
       val=analogRead(1); 
       Serial.print("A1= "); Serial.println(val); 
     } else {
       Serial.println("unknown command");
     }
  }
  delay(50);                 // wait a short while
}
