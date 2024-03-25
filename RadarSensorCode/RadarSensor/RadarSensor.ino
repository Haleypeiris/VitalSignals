// Libraries //
#include "Arduino.h"
#include "60ghzbreathheart.h"  
#include "WiFiS3.h"            
#include <SoftwareSerial.h>
#include "Arduino_LED_Matrix.h"                   

// Marcos //
#define TOLERANCE 1.1f                                    // Multipler, to see how much variablibity in the value before we say the bed is missing
#define VALIDINPUT(x) (x > 40.f && x < 200.f) ? x : 0.f  // Checks if the input is between 40cm and 200cm, outside the range is invalid, and 0cm is return
#define RX_PIN A2
#define TX_PIN A3
#define PERIOD 7000
#define LENOFHRDATA 11
#define HUMANPRESENSE 0x80
#define DISTANCECMD 0x04
#define HEARTRATE 0x85
#define HEARTRATECMD 0x02
#define RESPIRATORY 0x81
#define BREATHINGCMD 0x02

// Setting Debug mode
#ifndef DEBUG_MODE
//#define DEBUG_MODE
#endif

// Setting WIFI mode
#ifndef WIFI_MODE
//#define WIFI_MODE
#endif

// Heart Rate and Breathing functions
#ifndef OTHER_MODES
//#define OTHER_MODES
#endif

ArduinoLEDMatrix matrix;
const uint32_t heart[] = {
  0x3184a444,
  0x44042081,
  0x100a0040
};
const uint32_t clear[] = {
  0x00000000,
  0x00000000,
  0x00000000
};

// WIFI Variables //
#ifdef WIFI_MODE
  const char ssid[] = "NatalieiPhone";  // change your network SSID (name) //specific to Natalie's set up
  const char pass[] = "nataliehaw";     // change your network password (use for WPA, or use as key for WEP) //specific to Natalie's set up

  WiFiClient client;
  int status = WL_IDLE_STATUS;

  int HTTP_PORT = 80;
  String HTTP_METHOD = "GET";
  char HOST_NAME[] = "172.20.10.2";                                            //change to computer's IP address //specific to Natalie's set up
  String PATH_INSERT = "/capstone/bcgparsing/public/staff/insert_values.php";  //specific to Natalie's set up

  String queryString = "?measure=2";
  String float_distance = "empty";
  String float_hr = "empty";
  String float_rr = "empty";
#endif //WIFI_MODE


// Body Detection Variables //
BreathHeart_60GHz radar = BreathHeart_60GHz(&Serial1);
float BedDistance = 0.f;
float MaxBodyThickness = 15.f;         //cm
static float BodyDistance = 300.f;  //cm
bool BedOccupancy = false;
int FalseCounter = 0;
unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
int SSHeader;

#ifdef OTHER_MODES
//Todo: change into structu
float SendData[4] = { 0 };           //0: Distance, 1: Average HeartRate, 2: Breathing Rate
int HeartRateStorage[2] = { 0, 0 };  //0: Length of Data, 2-10: HeartRate Data
static unsigned int HeartRate = 0;
static unsigned int BreathingRate = 0;
#endif //OTHER_MODES

#ifdef WIFI_MODE
void sendGET(char HOST_NAME[])  //client function to send/receive GET request data.
{
  //if (client.connect(myserver, 80)) {  //starts client connection, checks for connection

  //if (client.connect(HOST_NAME, HTTP_PORT))
  if (client.connect(HOST_NAME, 80)) {
    //Serial.println("connected");
    //client.println(HTTP_METHOD + " " + PATH_INSERT + queryString + " HTTP/1.1"); // download text - did not work with http part
    client.println(HTTP_METHOD + " " + PATH_INSERT + queryString);
    //client.println("GET /~shb/arduino.txt HTTP/1.0"); //download text
    //Serial.println(HTTP_METHOD + " " + PATH_INSERT + queryString + " HTTP/1.1");
    Serial.println(HTTP_METHOD + " " + PATH_INSERT + queryString);  //how to print to see in string
    client.println();                                               //end of get request
  } else {
    Serial.println("connection failed");  //error message if no client connect
    Serial.println();
  }

  while (client.connected() && !client.available()) delay(1);  //waits for data
  while (client.connected() || client.available()) {           //connected or data available
    char c = client.read();                                    //gets byte from ethernet buffer
    //Serial.print(c); //prints byte to serial monitor
  }
  //Serial.println("disconnecting.");
  //Serial.println("==================");
  Serial.println();
  client.stop();  //stop client
}  // end of sendget
#endif

void setup() 
{

  // Serial Set up //
  Serial.begin(115200);
  Serial1.begin(115200);
  startMillis = millis();  //initial start time

  while (!Serial)
    Serial.println("Seria1 Error");  //When the serial port is opened, the program starts to execute.
  while (!Serial1)
    Serial.println("Serial1 Error");
  
  radar.reset_val();
  radar.reset_func();

  // Sets up the array of LEDs
  matrix.begin();

  // WIFI Stuff //
#ifdef WIFI_MODE
  if (WiFi.status() == WL_NO_MODULE) 
  {          // check for the WiFi module:
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) 
  {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) 
  {
    //Serial.print("Attempting to connect to SSID: ");
    //Serial.println(ssid); // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    //delay(10000);
  }

  //Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("wifi worked");

#endif //WIFI_MODE
}

void loop() 
{
  // put your main code here, to run repeatedly:
  while (BedDistance == 0.f) 
  {
#ifdef DEBUG_MODE
    Serial.println("distance from the bed - debug mode");
    BedDistance = 95.0f;  //VALIDINPUT(Serial.parseFloat());
#else
    Serial.println("distance from the bed");
    BedDistance = 95.0f;  //VALIDINPUT(Serial.parseFloat());
#endif
  }

  if (Serial1.available() && (BedDistance != 0.f)) 
  {
#ifdef WIFI_MODE
    float_distance = String(BodyDistance);
    //Serial.println(float_distance); //Need to create separate request or separate ?measure for it
    //queryString = "?measure="+float_distance;
    //sendGET(HOST_NAME);
#endif

    radar.recvRadarBytes();  //Receive radar data and start processing
    //radar.showData();                 //Serial port prints a set of received data frames
    SSHeader = int(radar.Msg[0]);
    switch (SSHeader) 
    {
      case HUMANPRESENSE:
        if (int(radar.Msg[1]) == DISTANCE) 
        {
          radar.distance = radar.Msg[4] << 8 | radar.Msg[5];  //Msg[4] -> MSB, Msg[5] -> LSB
          if(VALIDINPUT(radar.distance)>(BedDistance-MaxBodyThickness) && VALIDINPUT(radar.distance)<BedDistance)
          {
            //someone is in bed
          }
          else
          {
            FalseCounter += 1;
          }
          
#ifdef DEBUG_MODE
          Serial.print("Distance: ");
          Serial.println(radar.distance);
          Serial.print("False Counter");
          Serial.println(FalseCounter);
#endif //DEBUG_MODE
        }
        break;

#ifdef OTHER_MODES
      case HEARTRATE:
        if (int(radar.Msg[1]) == HEARTRATECMD) 
        {
          radar.heart_rate = radar.Msg[4];

  #ifdef WIFI_MODE      
        float_hr = String(radar.heart_rate);
        queryString = "?measure="+float_hr;
        sendGET(HOST_NAME);
  #endif //WIFI_MODE

  #ifdef DEBUG_MODE
          Serial.print("Heart Rate: ");
          Serial.println(radar.heart_rate);
  #endif //DEBUG_MODE

          HeartRateStorage[0] += radar.heart_rate;
          HeartRateStorage[1] += 1;
        }
        break;

      case RESPIRATORY:
        if (int(radar.Msg[1]) == BREATHINGCMD) 
        {
          radar.breath_rate = radar.Msg[4];

  #ifdef DEBUG_MODE
          Serial.print("Breathing Rate: ");
          Serial.println(radar.breath_rate);
  #endif //DEBUG_MODE
        }
        break;
#endif //OTHER_MODES
    }
  
#ifdef OTHER_MODES
  #ifdef WIFI_MODE
    float_hr = String(radar.heart_rate);
    float_rr = String(radar.breath_rate);
    queryString = "?measure="+float_hr+"&rr="+float_rr;
    Serial.println(queryString);
    sendGET(HOST_NAME);
  #endif //WIFI_MODE
#endif //OTHER_MODES


    currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
    // Sending Data every X seconds
    if (currentMillis - startMillis >= PERIOD)  //test whether the period has elapsed
    {
      if(FalseCounter > 10)
      {
        //if there were 10 counts that the user wasn't in bed, return false for this minute
        BedOccupancy = false;
      }
      else
      {
        BedOccupancy = true;
      }
#ifdef OTHER_MODES
      float AvgHeartRate = 0.00;
      //Calculate average HeartRate
      if (HeartRateStorage[1] != 0) 
      {
        AvgHeartRate = HeartRateStorage[0] / HeartRateStorage[1];
      } 
      else
      {
        HeartRateStorage[0] = 0;
      }    
      HeartRateStorage[1] = 0;

  #ifdef DEBUG_MODE
    Serial.print("Average Heart Rate: ");
    Serial.println(AvgHeartRate);
    SendData[2] =float(BreathingRate);
    SendData[3] = 1;
    Serial.println("********************************************");
    Serial.print("Distance: ");
    Serial.print(SendData[0]);
    Serial.print(" ");
    Serial.print("HeartRate: ");
    Serial.print(SendData[1]);
    Serial.print(" ");
    Serial.print("Breathing Rate: ");
    Serial.print(SendData[2]);
    Serial.print(" ");
    Serial.print("Time: ");
    Serial.println(SendData[3]);
    Serial.println("********************************************");   
  #endif //DEBUG_MODE
#endif //OTHER_MODES

      matrix.loadFrame(heart);
      startMillis = currentMillis;
    }

    delay(400);  //Add time delay to avoid program jam
  }  
  matrix.loadFrame(clear);
}
