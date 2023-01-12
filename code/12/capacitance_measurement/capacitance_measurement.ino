// Capacitance measurement, V. Ziemann, 170629
const int npts=100;
volatile int isamp=0,sample_buffer_ready=0;
uint16_t sample_buffer[npts],nsamp=npts,timestep=5;
float R=33e3;  // 33 kOhm
#include <MsTimer2.h>
void timer_action() {  //..................timer_action
  sample_buffer[isamp]=analogRead(A0);
//  sample_buffer[isamp]=max(1,sample_buffer[isamp]);
  isamp++;
  if (nsamp == isamp) {   
    MsTimer2::stop();
    isamp=0;
    sample_buffer_ready=1;
  }
}
double linfit(int n, uint16_t y[]) {  //.........linfit
  double ay0=0,ay1=0;
  double S0=n; 
  double S1=0.5*n*(n+1);
  double S2=n*(n+1.0)*(2.0*n+1)/6.0;
  for (int k=0;k<n;k++) {
     ay0+=(k+1)*log(y[k]);
     ay1+=log(y[k]);
  }
  return (S0*ay0-S1*ay1)/(S2*S0-S1*S1);
}
void setup() {  //...............................setup
  Serial.begin(9600);
  while (!Serial) {delay(10);}
  pinMode(2,OUTPUT);
  digitalWrite(2,HIGH);
//  Serial.println("Starting"); 
}
void loop() {  //................................loop
  if (Serial.available()) {
    char line[30];
    Serial.readStringUntil('\n').toCharArray(line,30);
    if (strstr(line,"CAP?")) {
      nsamp=(int)atof(&line[5]);
      nsamp=min(nsamp,npts);
      if (nsamp==0) nsamp=npts;
//    Serial.print("CAP "); Serial.println(nsamp); 
      pinMode(2,INPUT); 
      digitalWrite(2,LOW); // disables internal pullup
      MsTimer2::set(timestep, timer_action); 
      MsTimer2::start(); 
      sample_buffer[isamp]=analogRead(A0);
      isamp++;
   } else if (strstr(line,"WF?")) {
      Serial.print("WF "); Serial.println(nsamp);
      for (int i=0; i<nsamp; i++) Serial.println(sample_buffer[i]);  
   } else if (strstr(line,"TIMESTEP ")) {
      timestep=(int)atof(&line[9]);
      Serial.print("TIMESTEP "); Serial.println(timestep); 
   } else if (strstr(line,"TIMESTEP?")) {  
      Serial.print("TIMESTEP "); Serial.println(timestep);      
   } else if (strstr(line,"RESISTOR ")) {
      R=atof(&line[9]);
      Serial.print("RESISTOR "); Serial.println(R); 
   } else if (strstr(line,"RESISTOR?")) {
      Serial.print("RESISTOR "); Serial.println(R);     
    } else {
      Serial.print("Unknown: "); Serial.println(line);
    }
  }
  if (sample_buffer_ready==1) {
    sample_buffer_ready=0; 
//  for (int i=0; i<nsamp; i++) Serial.println(sample_buffer[i]);
    pinMode(2,OUTPUT);  // start charging capacitor
    digitalWrite(2,HIGH);
    delay(100);
    double slope=linfit(nsamp,(uint16_t)sample_buffer);
//    Serial.print("slope= "); Serial.println(slope,5);
    double capacitance=-1e6*timestep*1e-3/(slope*R);
    Serial.print("CAP "); Serial.println(capacitance,4);
    if (sample_buffer[0] < 250*sample_buffer[nsamp-1]) {
      Serial.println("Time too short, double TIMESTEP");
    }
  }
  delay(1);
}
