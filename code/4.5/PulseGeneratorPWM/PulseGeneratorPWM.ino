//Pulse Generator using PWM, V. Ziemann, 221021
#include <TimerOne.h>
const int outpin = 9;
float period=100, width=10; // in microseconds

void setup() {
  Serial.begin(9600); delay(2000);
  Timer1.initialize((long)period);
  Timer1.pwm(outpin, (long)(width*1023/period));
}

void loop() {
  if (Serial.available()) {
    char line[30];
    Serial.readStringUntil('\n').toCharArray(line,30);
    if (strstr(line,"PERIOD ")) {
      period=atof(&line[7]);
      Timer1.setPeriod((long)period);
      Timer1.pwm(outpin, (long)(width*1023/period));
    } else if (strstr(line,"PERIOD?")) {
      Serial.print("PERIOD "); Serial.println(period);
    } else if (strstr(line,"WIDTH ")) {
      width=max(1,min(period,atof(&line[6])));
      Timer1.pwm(outpin, (long)(width*1023/period));
    } else if (strstr(line,"WIDTH?")) {
      Serial.print("WIDTH "); Serial.println(width);
    } else if (strstr(line,"START")) {
      Timer1.start();
    } else if (strstr(line,"STOP")) {
      Timer1.stop();
    }
  } 
  delay(10);
}
