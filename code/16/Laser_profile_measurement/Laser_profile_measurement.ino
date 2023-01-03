// Laser_profile_measurement, V. Ziemann, 170628

char line[30];
int settle_time=2;
bool full_step=0;
int stepcounter=0;
int laser_power=10;

const int PA=2,PB=3,PC=4,PD=5,ENABLE=6;
//const int PB=3;
//const int PC=4;
//const int PD=5;
//const int ENABLE=6;
const int LASER=9;

void set_coils(int istep) {
  bool patA[]={1,1,0,0};
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

void setup() {
  Serial.begin (9600);
  while (!Serial) {;} 
  Serial.println("starting");
  pinMode(PA,OUTPUT);
  pinMode(PB,OUTPUT);
  pinMode(PC,OUTPUT);
  pinMode(PD,OUTPUT);
  pinMode(ENABLE,OUTPUT);
  digitalWrite(ENABLE,HIGH);
  analogWrite(LASER,laser_power);
}

void loop() {
  if (Serial.available()) {
    Serial.readStringUntil('\n').toCharArray(line,30);
    if (strstr(line,"MOVE ")) {
      int steps=(int)atof(&line[5]);
      Serial.print("MOVE = "); Serial.println(steps);
      digitalWrite(ENABLE,HIGH);
      if (steps > 0) {
        for (int i=0;i<steps;i++) set_coils(stepcounter++);
      } else {
        for (int i=0;i<abs(steps);i++) set_coils(stepcounter--);
      }
    } else if (strstr(line,"STEPS?")) {
      Serial.print("STEPS "); Serial.println(stepcounter);
    } else if (strstr(line,"STEPS ")) {
      stepcounter=(int)atof(&line[6]); 
    } else if (strstr(line,"WAIT?")) { 
      Serial.print("WAIT "); Serial.println(settle_time);     
    } else if (strstr(line,"WAIT ")) {
      settle_time=(int)atof(&line[5]); 
    } else if (strstr(line,"DISABLE")) {   
      digitalWrite(ENABLE,LOW);
    } else if (strstr(line,"ENABLE")) {   
      digitalWrite(ENABLE,HIGH);  
    } else if (strstr(line,"LDR?")) {   
      Serial.print("LDR "); Serial.println(analogRead(A0));     
    } else if (strstr(line,"POWER?")) {
      Serial.print("POWER "); Serial.println(laser_power);      
    } else if (strstr(line,"POWER ")) {
      laser_power=(int)atof(&line[6]);
      Serial.print("POWER "); Serial.println(laser_power);
      analogWrite(LASER,laser_power); 
    } else if (strstr(line,"SCAN ")) {
      int steps=(int)atof(&line[5]);       
      Serial.print("SCAN "); Serial.println(steps);
      digitalWrite(ENABLE,HIGH);
      delay(100);
      for (int i=0;i<abs(steps);i++) set_coils(stepcounter++);
      delay(100);
      for (int i=0;i<abs(steps);i++) {
        set_coils(stepcounter--);
        delay(50);
        unsigned long sum=0;
        for (int k=1; k<10; k++) {sum+=analogRead(A0); delay(10);}
        Serial.print(i); Serial.print(",\t"); Serial.println(sum);
      } 
      digitalWrite(ENABLE,LOW);
    } else if (strstr(line,"CALIBRATE")) {
        for (int power=0; power<256; power++) {
          analogWrite(LASER,power);
          delay(100);
          unsigned long sum=0;
          for (int k=1; k<10; k++) {sum+=analogRead(A0); delay(10);}
          Serial.print(power); Serial.print(",\t"); Serial.println(sum);
        }
        analogWrite(LASER,10);
    } else {
      Serial.println("unknown");
    }
  }
}
