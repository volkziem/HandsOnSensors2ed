// HC-SR04 distance logger, V. Ziemann, 161204
const int trig=2, echo=3;
void setup() {
  Serial.begin(9600); while (!Serial) {;}
  pinMode(trig,OUTPUT);
  pinMode(echo,INPUT);
}
void loop() {
  float duration,distance;
  digitalWrite(trig,LOW); // make a 10 us trigger pulse
  delayMicroseconds(2);
  digitalWrite(trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(trig,LOW);
  duration=pulseIn(echo,HIGH);      // wait for echo
  distance=100*duration*340e-6/2;   // in cm
  Serial.print(duration); Serial.print(" ");
  Serial.println(distance);
  delay(1000);
}
