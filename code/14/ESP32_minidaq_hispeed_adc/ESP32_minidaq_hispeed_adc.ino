// ESP32 DAQ high_speed_adc, V. ziemann, 221220
const char* ssid     = "messnetz";
const char* password = "zxcvZXCV";
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
WebServer server2(80);                              // port 80
WebSocketsServer webSocket = WebSocketsServer(81);  // port 81
#include <Ticker.h>
Ticker SampleSlow,Ramp25Ticker,Pulse18Ticker;
volatile uint8_t websock_num=0,info_available=0,output_ready=0;
int sample_period=100,samples[3];
uint8_t dac25val=0,do18=0;
char info_buffer[80];
char out[20000]; DynamicJsonDocument doc(20000);
//......................................................sampleslow_action
void sampleslow_action() {
  samples[0]=analogRead(32)/8;
  samples[1]=analogRead(33)/8;
  samples[2]=digitalRead(21);
  output_ready=1;
}
//..................................................................
void ramp25_action() {
  dac25val++; dacWrite(25,dac25val);
}
//..................................................................
void pulse18_action() {
  do18=!do18; digitalWrite(18,do18);
}
//.......................................................i2sInit
#include <driver/i2s.h>
bool i2s_adc_enabled=false;
void i2sInit(uint32_t i2s_sample_rate){
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
    .sample_rate =  i2s_sample_rate,                  // The format of the signal using ADC_BUILT_IN
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // is fixed at 12bit, stereo, MSB    
//   .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,     // requires sample reordering
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,     
    .dma_buf_len = 1024,    // 1024 is maximum length 
    .use_apll = true,   // must be true for low frequencies to work
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };
  Serial.printf("Attempting to setup I2S ADC with sampling frequency %d Hz\n", i2s_sample_rate);
  if(ESP_OK != i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL)){
    Serial.printf("Error installing I2S.");
  }
  if(ESP_OK != i2s_set_adc_mode(ADC_UNIT_1, ADC1_CHANNEL_4)){    // pin 32
    Serial.printf("Error setting up ADC.");
  }
  if(ESP_OK != adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_11)){
    Serial.printf("Error setting up ADC attenuation.");
  }
  if(ESP_OK != i2s_adc_enable(I2S_NUM_0)){
    Serial.printf("Error enabling ADC.");
  }
  Serial.printf("I2S ADC setup ok\n");
}

#include <driver/dac.h>
#include <soc/sens_reg.h>
#include "soc/rtc.h"
float dac25freq=0;
int dac25scale=0;   // output full scale
// from https://github.com/krzychb/dac-cosine
void cwDACinit(float freq, int scale, int offset) { //.........................cwDACinit
  if (abs(freq) < 1e-3) {dac_output_disable(DAC_CHANNEL_1); return;}
  int frequency_step=max(1.0,floor(0.5+0.95*65536.0*freq/RTC_FAST_CLK_FREQ_APPROX)); 
  SET_PERI_REG_MASK(SENS_SAR_DAC_CTRL1_REG, SENS_SW_TONE_EN);
  SET_PERI_REG_MASK(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_CW_EN1_M);
  SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_INV1, 2, SENS_DAC_INV1_S);
  SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL1_REG, SENS_SW_FSTEP, frequency_step, SENS_SW_FSTEP_S);
  SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_SCALE1, scale, SENS_DAC_SCALE1_S);
  SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_DC1, offset, SENS_DAC_DC1_S);
  dac_output_enable(DAC_CHANNEL_1);
  float frequency = RTC_FAST_CLK_FREQ_APPROX  * (float) frequency_step / 65536;
  Serial.printf("DAC config: clk_8m_div: 0, frequency step: %d, frequency: %.0f Hz\n", frequency_step, frequency);
}

//..............................................................http server
void handle_notfound() {
  server2.send(404,"text/plain","not found, use http://ip-address/");
}
//..............................................................websocket
void sendMSG(char *nam, const char *msg) {
  (void) sprintf(info_buffer,"{\"%s\":\"%s\"}",nam,msg);
  info_available=1;
}
//...................................................................
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  Serial.printf("webSocketEvent(%d, %d, ...)\r\n", num, type);
  websock_num=num;
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\r\n", num);
      break;
    case WStype_CONNECTED: 
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\r\n", 
          num, ip[0], ip[1], ip[2], ip[3], payload);
  //      char msg[100]="Connected to sensor";
  //      char buf[20],out[100];  
      }
      sendMSG("INFO","ESP32: Successfully connected");
      break;
    case WStype_TEXT:
    {
      Serial.printf("[%u] get Text: %s\r\n", num, payload);
      //........................................parse JSON
      DynamicJsonDocument root(300);
      deserializeJson(root,payload);
      const char *cmd = root["cmd"];
      const long val = root["val"];
      if (strstr(cmd,"START")) { 
        sendMSG("INFO","ESP32: Received Start command");
        sample_period=val;
        if (sample_period > 0) {  
          SampleSlow.attach_ms(sample_period,sampleslow_action);
          Serial.print("sample_period = "); Serial.println(sample_period);
        } else {
          if (i2s_adc_enabled) {i2s_adc_disable(I2S_NUM_0); i2s_driver_uninstall(I2S_NUM_0);}
          uint32_t i2s_sample_rate=(uint32_t) abs(sample_period);
          i2sInit(i2s_sample_rate);
          i2s_adc_enabled=true;
        }
      } else if (strstr(cmd,"STOP")) {
        if (sample_period>0) {  
          sendMSG("INFO","ESP32: Received Stop command");
          SampleSlow.detach();
        } else {
          if (ESP_OK != i2s_adc_disable(I2S_NUM_0)) {
            Serial.printf("Error disabling ADC."); //while(1);
          }
          if (ESP_OK != i2s_driver_uninstall(I2S_NUM_0)) {
            Serial.printf("Error uninstalling I2S driver."); //while(1);
          }
          i2s_adc_enabled=false;
          if (dac25freq>0) {cwDACinit(dac25freq,dac25scale,0);}
        }
      } else if (strstr(cmd,"SETDAC")) {
        Serial.printf("DAC25 frequency = %d\n",val); 
        if (val < 0) {dac25scale=-val-1;} else {dac25freq=val;}    
        cwDACinit(dac25freq,dac25scale,0);
      } else if (strstr(cmd,"DAC25")) {
        dacWrite(25,val);
        sendMSG("INFO","ESP32: set DAC25 to requested value");
      } else if (strstr(cmd,"DO18")) { digitalWrite(18,val); 
      } else if (strstr(cmd,"RAMP25")) {
        if (val > 0) {Ramp25Ticker.attach_ms(val,ramp25_action);
        } else {Ramp25Ticker.detach();}
       } else if (strstr(cmd,"PULSE18")) {
        if (val > 0) {Pulse18Ticker.attach_ms(val,pulse18_action);
        } else {Pulse18Ticker.detach();}
      } else {
        Serial.println("Unknown command");
        sendMSG("INFO","ESP32: Unknown command received");
      }
    }
  }
}

//..................................................................setup
void setup() {
  pinMode(21,INPUT); 
  pinMode(18,OUTPUT); 
  dacWrite(25,0); // initialize DAC
  Serial.begin(115200);
  delay(1000);
  //..............................................client mode
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {delay(500); Serial.print(".");}
  Serial.print("\nConnected to ");  Serial.print(ssid);
  Serial.print(" with IP address: "); Serial.println(WiFi.localIP());
  //........................................access point mode
/*
  WiFi.softAP("ESP32DAQ",password);
  Serial.print("Access point ESP32-DAQ started at IP ");
  Serial.println(WiFi.softAPIP());
*/
  //.........................................................
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  //.....................................................http server
  if (!SPIFFS.begin()) {
    Serial.println("ERROR: no SPIFFS filesystem found");
    return;
  } else {
    server2.begin();          
    server2.serveStatic("/", SPIFFS, "/esp32-daq.html");
    server2.onNotFound(handle_notfound);
    Serial.println("SPIFFS file system found and server started on port 80");     
  }
  ledcSetup(0, 1000, 2); ledcAttachPin(27, 0); ledcWrite(0, 2);
}
//.................................................................loop
void loop() { 
  server2.handleClient();    //..........................http server
  webSocket.loop();
  if (info_available==1) {
    info_available=0;
    webSocket.sendTXT(websock_num,info_buffer,strlen(info_buffer));
  }
  if (output_ready==1) {
    output_ready=0;
    doc.to<JsonObject>();  // clear doc
    for (int k=0;k<3;k++) {doc["ADC"][k]=samples[k];}
    serializeJson(doc,out);
    webSocket.sendTXT(websock_num,out,strlen(out));
  }
  if (i2s_adc_enabled) {
    size_t bytes_read;
    uint16_t buffer[1024]={0}, swap;   
    i2s_read(I2S_NUM_0, &buffer, sizeof(buffer), &bytes_read, 15);
    for (int j=0; j<bytes_read/2; j+=2) {  // left and right channel come in wrong temporal order
      swap=buffer[j+1]; buffer[j+1]=buffer[j]; buffer[j]=swap;
    }   
    doc.to<JsonObject>();  // clear doc
    for (int j=0; j<bytes_read/2; j++){doc["WF0"][j]=(buffer[j]& 0x0FFF)>>3;}
    serializeJson(doc,out);
    webSocket.sendTXT(websock_num,out,strlen(out));  
    delay(2000);
  }
  yield();
}
