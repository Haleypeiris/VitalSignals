// Libraries //
#include "Arduino.h"
byte SOF = 0;
byte length = 0;
byte inBuffer[2];
byte reset_response[10] = {0};
int data;
int debug = 1;
int sent = 0;
byte reset[] = {0xFD,0x00,0x02,0x01,0x00,0xFE}; // reset command

void setup() {
  // setup code
  pinMode(LED_BUILTIN, OUTPUT);      // set LED pin as output
 
  digitalWrite(LED_BUILTIN, HIGH);    // switch on LED pin
  delay(1000); // 1s delay
  digitalWrite(LED_BUILTIN, LOW);    // switch off LED pin

  Serial.begin(230400); // serial monitor output
  Serial1.begin(230400); // RX/TX pins on Arduino

  while(!Serial);
  while(!Serial1);
  Serial.println("Ready");

}

void loop() {

 while (Serial1.available() > 0) {
    Serial1.readBytesUntil('\n',inBuffer,2); // read first 2 bits for SOF and length
    if(inBuffer[0]==0xFE){ // if SOF detected, continue
      if (debug==1){
        Serial.println("SOF detected");
        Serial.println(inBuffer[0],HEX);
        Serial.print("length = ");
        Serial.println(inBuffer[1],DEC);
      }
      
      length = inBuffer[1]+4; // length of data frame
      byte dataFrame[length];
      data = Serial1.readBytesUntil('\n',dataFrame,length); // read data frame

      for (int i=0;i<data;i++){
        Serial.print(dataFrame[i],HEX); //print BCG data to serial port
        Serial.print(" ");
      }
      Serial.print('\n');
      // define BCG payload as per binary protocol specification document
      byte type = dataFrame[0];
      uint16_t ID = dataFrame[1]<<8 | dataFrame[2];
      uint32_t time = dataFrame[6]<<24 |dataFrame[5]<<16 | dataFrame[4]<<8 | dataFrame[3];
      uint32_t HR = dataFrame[10]<<24 |dataFrame[9]<<16 | dataFrame[8]<<8 | dataFrame[7];
      uint32_t SV = dataFrame[14]<<24 |dataFrame[13]<<16 | dataFrame[12]<<8 | dataFrame[11];
      uint32_t RR = dataFrame[18]<<24 |dataFrame[17]<<16 | dataFrame[16]<<8 | dataFrame[15];
      uint32_t HRV = dataFrame[22]<<24 |dataFrame[21]<<16 | dataFrame[20]<<8 | dataFrame[19];
      uint32_t fft_output = dataFrame[26]<<24 |dataFrame[25]<<16 | dataFrame[24]<<8 | dataFrame[23];
      uint32_t status = dataFrame[30]<<24 |dataFrame[29]<<16 | dataFrame[28]<<8 | dataFrame[27];
      uint32_t B2B = dataFrame[34]<<24 |dataFrame[33]<<16 | dataFrame[32]<<8 | dataFrame[31];
      uint32_t B2B1 = dataFrame[38]<<24 |dataFrame[37]<<16 | dataFrame[36]<<8 | dataFrame[35];
      uint32_t B2B2 = dataFrame[42]<<24 |dataFrame[41]<<16 | dataFrame[40]<<8 | dataFrame[39];
      uint32_t FCS = dataFrame[43];
      Serial.print("HR="); //print heart rate
      Serial.println(HR);
      }

  }

  // the code below is an attempt for sending a reset command to the BCG
  /*
  while(Serial1.available())  {
    if(sent==0) {
      Serial1.write(reset,sizeof(reset));
      sent = 1;
      Serial.println("Reset Command Sent");
      digitalWrite(LED_BUILTIN, HIGH);
    }
      Serial1.readBytesUntil(0xFE,reset_response,sizeof(reset_response)); // read first 2 bits for SOF and length
  
    //if(reset_response[1]==0x01){ // if ID detected, continue
      if (reset_response[1]==0x01)  {
        for (int i=0;i<sizeof(reset_response);i++){
          Serial.print(reset_response[i],HEX);
          Serial.print(" ");
        }
        Serial.print('\n');
        digitalWrite(LED_BUILTIN, LOW);
    }
  //}  
  }*/
}


int resetBCG(){
    pinMode(LED_BUILTIN, OUTPUT);      // set LED pin as output

  byte reset[] = {0xFE,0x00,0x01,0x00,0x02,0xFD}; // reset command
  while (Serial1.available() > 0) {
    if(Serial.availableForWrite() > 0){
      Serial.write(reset,sizeof(reset)); //send reset command
      digitalWrite(LED_BUILTIN, HIGH); // set LED pin on
      break;
    }
  }
  while (Serial1.available() > 0) {
    //read BCG response
    Serial1.readBytesUntil('\n',reset_response,sizeof(reset_response));
    
    if (debug==1){
      for (int i=0;i<sizeof(reset_response);i++){
        Serial.print(reset_response[i],HEX);
        Serial.print(" ");
      }
      Serial.print('\n');
    }
    digitalWrite(LED_BUILTIN, LOW);
      
    if(reset_response[5]==0){break;} // indicates successful reset of BCG
  }
}
