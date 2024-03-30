/* Read Accelerometer data from Murata SCL3300 Inclinometer
 * Version 3.2.0 - September 3, 2021
 * Example3_BasicAccelerometerReading
*/

#include <SPI.h>
#include <SCL3300.h>
#include <WiFi.h>

//wifi
//const char* ssid     = "NatalieiPhone";
//const char* password = "nataliehaw";
const char* ssid     = "BELL950";
const char* password = "6DC7C2EC6411";
WiFiServer server(80);
WiFiClient client;
int HTTP_PORT = 80;
String HTTP_METHOD = "GET";
char HOST_NAME[] = "192.168.2.123"; //BELL950 one
//char HOST_NAME[] = "172.20.10.2";  //change to computer's IP address //specific to Natalie's set up
//String PATH_INSERT = "/capstone/inclinometer/public/insert_to_sql.php"; //specific to Natalie's set up
//String PATH_INSERT = "/capstone/inclinometer/public/staff/insert_values.php";
String PATH_INSERT = "/capstone/bcgparsing/public/staff/insert_values.php";
String queryString = "?yAcc=2+&rr=4"; //as a default

//scl stuff
String yAcc;
String xAcc;
SCL3300 inclinometer;
//for one minute values
double yAccArray[400];
String yAccString; 
//uint32_t period = 10000L;
int count;
uint32_t period = 20000L; //5*6000L was supposed to be 5min
//Default SPI chip/slave select pin is D10

// Need the following define for SAMD processors
#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
  #define Serial SERIAL_PORT_USBVIRTUAL
#endif

//need sendget for http
void sendGET(char HOST_NAME[]) //client function to send/receive GET request data.
{
  //if (client.connect(myserver, 80)) {  //starts client connection, checks for connection
  //if (client.connect(HOST_NAME, HTTP_PORT)) 
  if (client.connect(HOST_NAME, 80)) {
    Serial.println("connected");
    //client.println(HTTP_METHOD + " " + PATH_INSERT + queryString + " HTTP/1.1"); // download text - did not work with http part
    client.println(HTTP_METHOD + " " + PATH_INSERT + queryString);
    //client.println("GET /~shb/arduino.txt HTTP/1.0"); //download text
    //Serial.println(HTTP_METHOD + " " + PATH_INSERT + queryString + " HTTP/1.1");
    Serial.println(HTTP_METHOD + " " + PATH_INSERT + queryString); //how to print to see in string
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
  //Serial.println("==================");
  Serial.println();
  client.stop(); //stop client
}  // end of sendget

void setup() {
  Serial.begin(9600); //normally 9600 - no difference in number of points sent at 115200
  //scl
  delay(2000); //SAMD boards may need a long time to init SerialUSB
  Serial.println("Reading basic Accelerometer values from SCL3300 Inclinometer");

  if (inclinometer.begin(5) == false) { //make sure to do begin(#) with SS port pin #
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
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); //line that ACTUALLY connects  
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
  yAccString = String('"');
  count = 0;
  for( uint32_t tStart = millis();  (millis()-tStart) < period;  ){
  //for (int i=0; i<=220; i++){

    if (inclinometer.available()) { //Get next block of data from sensor
      /*Serial.print("X Accelerometer: ");
      Serial.print(inclinometer.getCalculatedAccelerometerX());
      Serial.print("\t");*/
      //Serial.print("Y Accelerometer: ");
      yAcc = double(inclinometer.getCalculatedAccelerometerY()*100000); //need as float for get request
      //Serial.print(yAcc);
      yAccString += String(yAcc);
      yAccString += ",";
      xAcc = double(inclinometer.getCalculatedAccelerometerX()*100000); //just to send a second value rn
      /*Serial.print(inclinometer.getCalculatedAccelerometerY());
      Serial.print("\t");
      Serial.print("Z Accelerometer: ");
      Serial.println(inclinometer.getCalculatedAccelerometerZ());*/
      delay(100); //Allow a little time to see the output
    } else inclinometer.reset();
    count++;
  }
  yAccString += '"';
  //yAccString.replace("-","n");
  Serial.println(yAccString);
  Serial.println(count);
  queryString = "?yAcc="+yAcc+"&rr="+xAcc+"&yAccString="+yAccString;
  //queryString = "?yAcc="+yAcc+"&rr="+xAcc;
  sendGET(HOST_NAME);
}