// AD9850 frequency generator, V. Ziemann, 221024

double freq=100000;
#define AD9850_CLOCK 125000000
const int W_CLK=2,FQ_UD=3,DATA=4,RESET=5;  // Nano
//const int W_CLK=5,FQ_UD=4,DATA=3,RESET=2;  // UNO

#define pulseHigh(pin) {digitalWrite(pin,HIGH); digitalWrite(pin,LOW);}

void set_frequency(double frequency) {
  int32_t freq1=frequency * 4294967295/AD9850_CLOCK;
  pulseHigh(RESET);        // initialize AD9850
  pulseHigh(W_CLK); 
  pulseHigh(FQ_UD);
  for (int i=0; i<40; i++){
    digitalWrite(DATA,(freq1 >> i) & 0x01);
    pulseHigh(W_CLK); 
  }
  pulseHigh(FQ_UD);
}

void setup() {
  Serial.begin(9600);
  pinMode(FQ_UD,OUTPUT); pinMode(W_CLK,OUTPUT);
  pinMode(DATA,OUTPUT); pinMode(RESET,OUTPUT);
  set_frequency(freq);
}

void loop() {
  if (Serial.available()) {
    char line[30];   
    Serial.readStringUntil('\n').toCharArray(line,30);
    if (strstr(line,"FREQ ")) {
      freq=atof(&line[5]);
      set_frequency(freq);
    } else if (strstr(line,"FREQ?")) {
      Serial.print("FREQ "); Serial.println(freq);
    } else {
      Serial.println("unknown command, disconnecting");
    }
  }
}
