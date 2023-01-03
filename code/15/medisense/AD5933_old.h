// AD5933.h, V. Ziemann, 221118
const int AD5933=0x0D;  // I2C address
float freq=5000.0, finc=500, MCLK=16.776E6;
int npts=200;
byte mode=0,pga=1;

void AD5933_config(float freq, float finc, int npts, byte mode, byte pga) {
  I2Cwrite(AD5933,0x80,0x00);    // Control registers, reset
  I2Cwrite(AD5933,0x81,0x10);  
//I2Cwrite(AD5933,0x80,0x01);    // Range=1, PGA=1  
  I2Cwrite(AD5933,0x80,((mode & 0x03)<<1) | pga); 
//  I2Cwrite(AD5933,0x8A,0x07);  // Settling time, as long as possible
  I2Cwrite(AD5933,0x8A,0x00);    // four times faster
  I2Cwrite(AD5933,0x8B,0xFF);     
  long n=(long)(freq/(MCLK/4)*pow(2,27));
//  Serial.print("n = 0x"); Serial.println(n,HEX);
  I2Cwrite(AD5933,0x82,(n>>16) & 0xFF);              // Starting frequency
  I2Cwrite(AD5933,0x83,(n>>8) & 0xFF);
  I2Cwrite(AD5933,0x84,n & 0xFF);
  long m=(long)(finc/(MCLK/4)*pow(2,27));
//  Serial.print("m = 0x"); Serial.println(m,HEX);
  I2Cwrite(AD5933,0x85,(m>>16) & 0xFF);              // Frequency increment
  I2Cwrite(AD5933,0x86,(m>>8) & 0xFF);
  I2Cwrite(AD5933,0x87,m & 0xFF);
//  Serial.print("npts = "); Serial.println(npts);
  I2Cwrite(AD5933,0x88,(npts>>8) & 0xFF);            // Number of increments 
  I2Cwrite(AD5933,0x89,npts & 0xFF); 
}

// cmd: 0x10=init, 0x20=start, 0x30=increment, 0x40=repeat
//      0x90=temp, 0xA0=Pwdn, 0xB0=Stdby
void AD5933_command(byte cmd) {
  uint8_t b=I2Cread(AD5933,0x80); 
  I2Cwrite(AD5933,0x80,(b & 0x0F) | cmd);
}

void AD5933_standby() {AD5933_command(0xB0);}
void AD5933_init() {AD5933_command(0x10);}
void AD5933_increment() {AD5933_command(0x30);}
void AD5933_power_down() {AD5933_command(0xA0);}
int valid_data() {return ((I2Cread(AD5933,0x8F) & 0x02) == 0x02);}
int sweep_done() {return ((I2Cread(AD5933,0x8F) & 0x04) == 0x04);} 

void AD5933_sweep(float freq, float finc, int npts, byte mode, byte pga) {
  AD5933_config(freq,finc,npts,mode,pga);
  AD5933_standby();  // Standby
  AD5933_init();  // Init
  delay(1000);          // Initial settle time
  AD5933_command(0x20);  // Start sweep
  while (!sweep_done()) {
    while (!valid_data()) {delay(10);}  // wait for data ready
    int16_t Ival=(I2Cread(AD5933,0x94) << 8) | I2Cread(AD5933,0x95); // real
    int16_t Qval=(I2Cread(AD5933,0x96) << 8) | I2Cread(AD5933,0x97); // imag
    float mag=sqrt(Ival*Ival+Qval*Qval);
    Serial.print(Ival); Serial.print("\t"); Serial.print(Qval); Serial.print("\t");
    Serial.println(mag); // Serial.print("\t"); Serial.println(82e3*3930/mag);
    AD5933_increment();   // increment frequency
  }
//  AD5933_command(0xA0);  // Power down
}

void AD5933_sweep2(float freq, float finc, int npts, byte mode, byte pga, 
                   int16_t *Ival, int16_t *Qval) {
  AD5933_config(freq,finc,npts,mode,pga);
  AD5933_standby();      // Standby
  AD5933_init();         // Init
  delay(200);            // Initial settle time
  AD5933_command(0x20);  // Start sweep
  int ic=0;
  while (!sweep_done()) {
    while (!valid_data()) {delay(10);}  // wait for data ready
    Ival[ic]=(I2Cread(AD5933,0x94) << 8) | I2Cread(AD5933,0x95); // real
    Qval[ic]=(I2Cread(AD5933,0x96) << 8) | I2Cread(AD5933,0x97); // imag
    ic++;
    AD5933_increment();   // increment frequency
        
  }
//  AD5933_command(0xA0);  // Power down
}
