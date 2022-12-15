// DHT11, V. Ziemann, 170804
#define DHT 2   // sensor pin
float read_dht11(float *T) {
  bool p[500]= { 0 }; 
  uint8_t data[5]={0},checksum;
  int ic=0,goes_up=0;
  pinMode(DHT,OUTPUT); 
  digitalWrite(DHT,LOW);   // 20 ms low pulse
  delay(20);
  digitalWrite(DHT,HIGH);
  noInterrupts();  // interrupts off for accurate timing
  delayMicroseconds(20);  // make sure we start at low
  pinMode(DHT,INPUT);
  for (int i=0;i<500;i++) {  // read 5 ms worth of data
    p[i]=digitalRead(DHT);
    delayMicroseconds(10);   // one every 10 us
  }
  interrupts();    // interrupts on again
  while (p[ic++] == 0); // next HIGH, acknowledge bit
  while (p[ic++] == 1); // next LOW, first data bit 
  for (int i=0;i<5;i++) {        // loop over the 
    for (int j=7;j>=0;j--) {     // 40 data bits
     while (p[ic++] == 0) {goes_up=ic;}
     while (p[ic++] == 1) ;
     (ic-goes_up > 4) ? data[i] |= (1<<j) : 0; 
    } 
  }
  checksum=((data[0]+data[1]+data[2]+data[3]) & 0xFF);
  if (checksum==data[4]) {
    *T=(float) data[2];      // temperature
    return (float) data[0];  // humidity
  } 
  *T=-100;   // only get here if bad reading
  return -1;  
}
void setup() {  //.............................setup
  Serial.begin(9600); 
  while (!Serial) {;}
  pinMode(DHT,INPUT);
  digitalWrite(DHT,HIGH);
  delay(2000);     // wait for DHT to wake up
}
void loop() {  //...............................loop
  float temperature,humidity;
  humidity=read_dht11(&temperature);
  Serial.print(humidity); Serial.print("\t"); 
  Serial.println(temperature);
  delay(2000);
}
