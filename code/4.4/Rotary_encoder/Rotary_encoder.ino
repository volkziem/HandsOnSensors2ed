// Rotary encoder, V. Ziemann, 161205
const int pinA=2,pinB=4;
volatile int pos=0;
void setup() {
  Serial.begin(9600); while (!Serial) {;}
  pinMode(pinA,INPUT_PULLUP); 
  pinMode(pinB,INPUT_PULLUP); 
  attachInterrupt(0,interrupt_handler,FALLING); // 0=pin2, 1=pin3
}
void loop() {
  Serial.println(pos);  
  delay(1000); 
}
void interrupt_handler() {
  if (digitalRead(pinA)==digitalRead(pinB)) {pos++;} else {pos--;}
}
