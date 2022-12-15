// AD5933Serial, V. Ziemann, 221118
#include <Wire.h>
#include "I2Crw.h"
#include "AD5933.h"    

void setup() {
  Serial.begin(115200); delay(1000);
  Wire.begin(); 
}

void loop() {
  char line[30];
  if (Serial.available()) {
    Serial.readStringUntil('\n').toCharArray(line,30);
    if (strstr(line,"SWEEP")) {
      AD5933_sweep(freq,finc,npts,mode,pga,NULL,NULL);      
    } else if (strstr(line,"OUTON")) {  
      AD5933_config(freq,finc,npts,mode,pga);
      AD5933_standby();
      AD5933_init();
    } else if (strstr(line,"OUTOFF")) { 
      AD5933_power_down();
    } else if (strstr(line,"FREQ ")) {
      freq=atof(&line[5]);
    } else if (strstr(line,"FINC ")) {
      finc=atof(&line[5]);
    } else if (strstr(line,"NPTS ")) {
      npts=atoi(&line[5]);
    } else if (strstr(line,"MODE ")) {
      mode=atoi(&line[5]);
    } else if (strstr(line,"PGA ")) {
      pga=atoi(&line[4]); 
    } else if (strstr(line,"STATE?")) {
      char msg[80];     
      sprintf(msg,"STATE %ld %ld %d %d %d",(long)freq,(long)finc,npts,mode,pga);
      Serial.println(msg);
    }
  }
}
