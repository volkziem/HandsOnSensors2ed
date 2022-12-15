// Water_color_monitor, V. Ziemann, 170823
int repeat=20,wait=5;
void alloff() {  //.......................alloff
  digitalWrite(2,HIGH);
  digitalWrite(3,HIGH);
  digitalWrite(4,HIGH);
}
float measure(int pin, int repeat) {  //..............measure
  int hi,lo;
  float sum=0;
//  if (wait>1) {Serial.print("Pin = "); Serial.println(pin);}
  alloff();
  delay(10);
  for (int i=0;i<repeat;i++) {
    digitalWrite(pin,LOW);  
    delay(wait); 
    lo=analogRead(0);
    digitalWrite(pin,HIGH);
    delay(wait); 
    hi=analogRead(0);
    sum+=(hi-lo);
//    if (wait>1) {Serial.print(lo); Serial.print("\t"); Serial.println(hi);}
  }
  return sum/repeat;
}
void setup() {  //.........................setup
  Serial.begin(115200); while (!Serial) {;}
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  alloff();
  digitalWrite(2,LOW); 
}
void loop() {  //...........................loop
  if (Serial.available()) {
    char line[30]; 
    Serial.readStringUntil('\n').toCharArray(line,30);   
    if (strstr(line,"OFF")==line) {
      alloff();       
    } else if (strstr(line,"RED")==line) {
      alloff();  
      digitalWrite(2,LOW); 
    } else if (strstr(line,"GREEN")==line) {
      alloff();  
      digitalWrite(3,LOW); 
    } else if (strstr(line,"BLUE")==line) {
      alloff();  
      digitalWrite(4,LOW); 
    } else if (strstr(line,"A0?")==line) {
      Serial.print("A0 "); Serial.println(analogRead(A0));      
    } else if (strstr(line,"COLOR?")==line) {
      float red=measure(2,repeat);    // red
      float green=measure(3,repeat);  // green
      float blue=measure(4,repeat);   // blue
      Serial.print("COLOR "); Serial.print(red); 
      Serial.print("\t");Serial.print(green); 
      Serial.print("\t");Serial.println(blue); 
    } else if (strstr(line,"WAIT?")==line) { 
      Serial.print("WAIT "); Serial.println(wait); 
    } else if (strstr(line,"WAIT ")==line) {
       wait=(int)atof(&line[5]);
    } else if (strstr(line,"REPEAT?")==line) { 
      Serial.print("REPEAT "); Serial.println(repeat); 
    } else if (strstr(line,"REPEAT ")==line) { 
      repeat=(int)atof(&line[6]);
    }
  }
  delay(10);
}
