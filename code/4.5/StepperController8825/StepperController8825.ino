// Stepper controller with DRV8825, V. Ziemann, 170626
char line[30];
int settle_time=30,stepcounter=0;
const int DIR=2;  // direction pin
const int STEP=3; // step pin
const int MODE=4; // mode pin, LOW=FULLSTEP, HIGH=MICROSTEP
void setup() {  //................................setup
  Serial.begin (9600);
  while (!Serial) {;} 
  Serial.println("starting");
  pinMode(DIR,OUTPUT); digitalWrite(DIR,LOW);
  pinMode(STEP,OUTPUT); digitalWrite(STEP,LOW);
  pinMode(MODE,OUTPUT); digitalWrite(MODE,HIGH);
}
void loop() {  //..................................loop
  if (Serial.available()) {
    Serial.readStringUntil('\n').toCharArray(line,30);
    if (strstr(line,"MOVE ")) {
      int steps=(int)atof(&line[5]);
      Serial.print("MOVE = "); Serial.println(steps);
      if (steps > 0) {
        digitalWrite(DIR,LOW);
        for (int i=0;i<steps;i++) { 
          stepcounter++;
          digitalWrite(STEP,HIGH);
          delayMicroseconds(settle_time);
          digitalWrite(STEP,LOW);
          delayMicroseconds(settle_time);
        }
      } else {
        digitalWrite(DIR,HIGH);
        for (int i=0;i<abs(steps);i++) {
          stepcounter--;
          digitalWrite(STEP,HIGH);
          delayMicroseconds(settle_time);
          digitalWrite(STEP,LOW);
          delayMicroseconds(settle_time);
        }
      }
    } else if (strstr(line,"STEPS?")) {
      Serial.print("STEPS "); Serial.println(stepcounter);
    } else if (strstr(line,"STEPS ")) {
      stepcounter=(int)atof(&line[6]); 
    } else if (strstr(line,"WAIT?")) { 
      Serial.print("WAIT "); Serial.println(settle_time);     
    } else if (strstr(line,"WAIT ")) {
      settle_time=(int)atof(&line[5]); 
    } else if (strstr(line,"MICROSTEP")) {  
      settle_time=30;
      digitalWrite(MODE,HIGH);  
    } else if (strstr(line,"FULLSTEP")) {
      settle_time=2000;  
      digitalWrite(MODE,LOW);           
    } else { 
      Serial.println("unknown");
    }
  }
}
