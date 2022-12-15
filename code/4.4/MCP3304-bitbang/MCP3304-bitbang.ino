// MCP3304 bit-banged, V. Ziemann, 170726

#define CS   15       // ChipSelect
#define MOSI 13       // MasterOutSlaveIn
#define MISO 12       // MasterInSlaveOut
#define CLK  14       // Clock

void mcp3304_init_bb() {
  pinMode(CS,OUTPUT); 
  pinMode(MOSI,OUTPUT);
  pinMode(MISO,INPUT); 
  pinMode(CLK,OUTPUT); 
  digitalWrite(CS,HIGH); 
  digitalWrite(MOSI,LOW); 
  digitalWrite(CLK,LOW);  
}

int mcp3304_read_bb(int channel) {  // bit-bang version
  int adcvalue=0, sign=0;
  byte commandbits = B10000000;
  commandbits|=(channel & 0x03) << 4; // 5 config bits, MSB first
  digitalWrite(CS,LOW);  // chip select
  for (int i=7; i>0; i--){  // clock bits to device
    digitalWrite(MOSI,commandbits&1<<i);
    digitalWrite(CLK,HIGH); // including two null bits
    digitalWrite(CLK,LOW);    
  } 
  sign=digitalRead(MISO);    // first read the sign bit
  digitalWrite(CLK,HIGH);
  digitalWrite(CLK,LOW);  
  for (int i=11; i>=0; i--){
    adcvalue+=digitalRead(MISO)<<i;
    digitalWrite(CLK,HIGH);
    digitalWrite(CLK,LOW); 
  }
  digitalWrite(CS, HIGH); 
  if (sign) {adcvalue = adcvalue-4096;  } 
  return adcvalue;
}
  
void setup() {
  mcp3304_init_bb();
  Serial.begin(115200);
  while (!Serial) { delay(10);} 
}

void loop() {
  int val = mcp3304_read_bb(0); 
  Serial.print("  CH0="); Serial.println(val); 
  delay(1000); 
}
