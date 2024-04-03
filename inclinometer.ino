/* Read Accelerometer data from Murata SCL3300 Inclinometer
 * Version 3.2.0 - September 3, 2021
 * Example3_BasicAccelerometerReading
*/

#include <SPI.h>
#include <SCL3300.h>
#include <WiFi.h>

//more wifi stuff to add
//#include "RegisterMappings.h" //not relevant
#include "Arduino.h"
#include "Wire.h"
#include "esp_wpa2.h"
//#include <WiFi.h>
#include "arduino_secrets.h"

//wifi

//92176E77
WiFiServer server(80);
WiFiClient client;
int HTTP_PORT = 80;
String HTTP_METHOD = "GET";

//for later: transferring host_name to arduino_secrets
//char HOST_NAME[] = "172.17.97.141"; //mac wifi one thode 
char HOST_NAME[] = "192.168.2.200"; //mac wifi one thode 
String PATH_INSERT = "/capstone/bcgparsing/public/staff/insert_values.php";
String queryString = "?yAcc=2+&bed_status=0"; //default

//scl stuff
String yAcc;
String bed_status = "0"; //default
SCL3300 inclinometer;
String yAccString; 
//uint32_t period = 10000L; //10s
int count;
uint32_t period = 20000L; //5*6000L = 5min
// Need the following define for SAMD processors
#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
  #define Serial SERIAL_PORT_USBVIRTUAL
#endif

// //spi thing
// #define SCK 18
// #define MISO 23
// #define MOSI 19
// #define SS 5

//need sendget for http
void sendGET(char HOST_NAME[]) //client function to send/receive GET request data.
{
  //if (client.connect(myserver, 80)) {  //starts client connection, checks for connection
  //if (client.connect(HOST_NAME, HTTP_PORT)) 
  if (client.connect(HOST_NAME, 80)) {
    Serial.println("connected");
    //client.println(HTTP_METHOD + " " + PATH_INSERT + queryString + " HTTP/1.1"); // download text - did not work with http part
    client.println(HTTP_METHOD + " " + PATH_INSERT + queryString);
    Serial.println(HTTP_METHOD + " " + PATH_INSERT + queryString); //print to see in serial monitor
    client.println(); //end of get request
  } 
  else {
    Serial.println("connection failed"); //error message if no client connect
    Serial.println();
  }

  while(client.connected() && !client.available()) delay(1); //waits for data
  while (client.connected() || client.available()) { //connected or data available
    char c = client.read(); //gets byte from ethernet buffer
    //Serial.print(c); //prints byte to serial monitor 
  }
  //Serial.println("disconnecting.");
  Serial.println();
  client.stop(); //stop client
}  // end of sendget

void setup() {
  Serial.begin(9600); //normally 9600 - no difference in number of points sent at 115200
  delay(2000); //SAMD boards may need a long time to init SerialUSB
  Serial.println("Reading basic Accelerometer values from SCL3300 Inclinometer");
  //SPI.begin(SCK, MISO, MOSI, SS);
  //if (inclinometer.begin(SPI, 5) == false) { //make sure to do begin(#) with SS port pin #
  if (inclinometer.begin(5) == false) { //Default SPI chip/slave select pin is D10
    Serial.print("MOSI: ");
    Serial.println(MOSI);
    Serial.print("MISO: ");
    Serial.println(MISO);
    Serial.print("SCK: ");
    Serial.println(SCK);
    Serial.print("SS: ");
    Serial.println(SS);
    Serial.println("Murata SCL3300 inclinometer not connected.");
    //inclinometer.reset();
    //while(1); //Freeze
    Serial.println(inclinometer.isConnected());
  }

  //wifi
  delay(10);
  //Serial.print("Connecting to ");
  //Serial.println(ssid);

  //from e wifi
  WiFi.begin();
  WiFi.disconnect(true); //disconnect form wifi to set new wifi connection
  WiFi.mode(WIFI_STA);   //init wifi mode - might crash but part of wifi library
  //esp_wifi_set_mac(ESP_IF_WIFI_STA, &masterCustomMac[0]);
  Serial.print("MAC >> ");
  Serial.println(WiFi.macAddress());
    
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SECRET_SSID);

      // WPA2 enterprise magic starts here
  WiFi.disconnect(true);
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)SECRET_IDENTITY, strlen(SECRET_IDENTITY));      
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)SECRET_USER, strlen(SECRET_USER));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)SECRET_PASS, strlen(SECRET_PASS));
  esp_wifi_sta_wpa2_ent_enable();
      // WPA2 enterprise magic ends here
  WiFi.begin(ssid, password); //line that connects if using non arduino_secrets method 
  //WiFi.begin();
  //WiFi.begin(SECRET_SSID, WPA2_AUTH_PEAP, SECRET_IDENTITY, SECRET_USER, SECRET_PASS);

  while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();
}

void loop() {
  
  pinMode(36, INPUT); //ya for us it's output bc intention is to send bed status here
  Serial.println(digitalRead(36)); //reads input pin and either will give hi or lo (need to print to read what the value is)
  bed_status = digitalRead(36);
  yAccString = String('"');
  count = 0;
  for( uint32_t tStart = millis();  (millis()-tStart) < period;  ){ //set to 20s period
    if (inclinometer.available()) { //Get next block of data from sensor
      //Serial.print(inclinometer.getCalculatedAccelerometerX());
      yAcc = double(inclinometer.getCalculatedAccelerometerY()*100000); //need as float for get request
      //Serial.print(yAcc);
      yAccString += String(yAcc);
      yAccString += ",";
      //xAcc = double(inclinometer.getCalculatedAccelerometerX()*100000); //just to send a second value rn
      /*Serial.print(inclinometer.getCalculatedAccelerometerY());
      Serial.println(inclinometer.getCalculatedAccelerometerZ());*/
      delay(100); //Allow a time to see output
    } else inclinometer.reset();
    count++;
  }
  yAccString += '"';
  //yAccString.replace("-","n");
  //Serial.println(yAccString); //print if anything
  //Serial.println(count);

  //GPIO INPUT FROM ARDUINO
  //pinMode(36 or 39 idk, INPUT); //ya for us it's output bc intention is to send bed status here
  //bed_status = digitalRead(inPin); //reads input pin and either will give hi or lo (need to print to read what the value is)
  bed_status = String(bed_status);
  //queryString = "?yAcc="+yAcc+"&rr="+xAcc+"&yAccString="+yAccString;
  queryString = "?yAcc="+yAcc+"&bed_status="+bed_status+"&yAccString="+yAccString;

  sendGET(HOST_NAME);

  //include bed_status parameter as value from GPIO

}