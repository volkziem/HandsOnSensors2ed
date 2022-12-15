// H-bridge DC-motor Controller, V. Ziemann, 170614
char line[30];
void setup() {
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  Serial.begin (9600);
  while (!Serial) {;} 
  Serial.println("starting");
}

void loop() {
  if (Serial.available()) {
    Serial.readStringUntil('\n').toCharArray(line,30);
    if (strstr(line,"FW ")) {
      digitalWrite(2,LOW);
      digitalWrite(3,LOW);
      digitalWrite(3,HIGH);
      float val=atof(&line[3]);
      Serial.print("FW = "); Serial.println(val);
      analogWrite(9,(int)val);  
    } else if (strstr(line,"BW ")) {    
      digitalWrite(2,LOW);
      digitalWrite(3,LOW);
      digitalWrite(2,HIGH);
      float val=atof(&line[3]);
      Serial.print("BW = "); Serial.println(val);
      analogWrite(9,(int)val); 
    } else if (strstr(line,"STOP")) {
      digitalWrite(2,LOW);
      digitalWrite(3,LOW); 
      analogWrite(9,(int)0);  
    } else {   // STOP in all other cases
      digitalWrite(2,LOW);
      digitalWrite(3,LOW); 
      analogWrite(9,(int)0); 
      Serial.println("unknown");
    }
  }
}
