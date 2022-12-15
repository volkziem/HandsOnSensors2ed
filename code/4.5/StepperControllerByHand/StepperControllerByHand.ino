// Stepper controller by hand, V. Ziemann, 170624
char line[30];
int settle_time=2; // milli-seconds
int stepcounter=0;
const int PA=2;
const int PB=3;
const int PC=4;
const int PD=5;
const int ENABLE=6;
void set_coils(int istep) {  //........full-step mode
  bool patA[]={1,1,0,0};  // or {1,0,0,0}
  int pat_length=4;
  int ii;
  istep=istep % pat_length;
  if (istep < 0) istep+=pat_length;  
  digitalWrite(PA,patA[istep]);
  ii=(istep+2) % pat_length;
  digitalWrite(PB,patA[ii]);
  ii=(istep+3) % pat_length;
  digitalWrite(PC,patA[ii]);
  ii=(istep+1) % pat_length;
  digitalWrite(PD,patA[ii]);        
  delay(settle_time);   
}
void setup() {    //...........................setup
  Serial.begin (9600);
  while (!Serial) {;} 
  pinMode(PA,OUTPUT);
  pinMode(PB,OUTPUT);
  pinMode(PC,OUTPUT);
  pinMode(PD,OUTPUT);
  pinMode(ENABLE,OUTPUT);
  digitalWrite(ENABLE,HIGH);
}
void loop() {  //...............................loop
  if (Serial.available()) {
    Serial.readStringUntil('\n').toCharArray(line,30);
    if (strstr(line,"MOVE ")==line) {
      int steps=(int)atof(&line[5]);
      if (steps > 0) {
        for (int i=0;i<steps;i++) set_coils(stepcounter++);
      } else {
        for (int i=0;i<abs(steps);i++) set_coils(stepcounter--);
      }
    } else if (strstr(line,"STEPS?")==line) {
      Serial.print("STEPS "); Serial.println(stepcounter);
    } else if (strstr(line,"STEPS ")==line) {
      stepcounter=(int)atof(&line[6]); 
    } else if (strstr(line,"DISABLE")==line) {   
      digitalWrite(ENABLE,LOW);
    } else if (strstr(line,"ENABLE")==line) {   
      digitalWrite(ENABLE,HIGH);              
    }
  }
}
/*
void set_coils(int istep) {   // half-step mode
  bool patA[]={1,1,1,0,0,0,0,0};
  int pat_length=8;
  int ii;
  istep=istep % pat_length;
  if (istep < 0) istep+=pat_length;  
  digitalWrite(PA,patA[istep]);
  ii=(istep+4) % pat_length;
  digitalWrite(PB,patA[ii]);
  ii=(istep+6) % pat_length;
  digitalWrite(PC,patA[ii]);
  ii=(istep+2) % pat_length;
  digitalWrite(PD,patA[ii]);        
  delay(settle_time);   
}
*/
