
// Libraries //
#include "Arduino.h"
#include "60ghzbreathheart.h"                   // Used to process distance values
#include "RTC.h"                                // Idk, we can probably delete this
#include "WiFiS3.h"                             // Wifi module
//#include "FloatToString"                      // library aka download if you don't have it Haley
//#include <SoftwareSerial.h>                   // Double check if this is needed for additional serial ports

// Marcos //
#define TOLERANCE 1.1f                          // Multipler, to see how much variablibity in the value before we say the bed is missing
#define VALIDINPUT(x) (x>40.f && x<200.f) ? x : 0.f; // Checks if the input is between 40cm and 200cm, outside the range is invalid, and 0cm is return
//#define RX_Pin A2
//#define TX_Pin A3
#define PERIOD 5000
#define LENOFHRDATA 11

//  TODO: Digital Serial Code //
//SoftwareSerial mySerial = SoftwareSerial(RX_Pin, TX_Pin);
// we'll be using software serial
//BreathHeart_60GHz radar = BreathHeart_60GHz(&mySerial);


// WIFI Variables //
const char ssid[] = "NatalieiPhone";            // change your network SSID (name)
const char pass[] = "nataliehaw";               // change your network password (use for WPA, or use as key for WEP)

WiFiClient client;
int status = WL_IDLE_STATUS;

int HTTP_PORT = 80;
String HTTP_METHOD = "GET";
//char HOST_NAME[] = "192.168.0.26";            // change to your PC's IP address
char HOST_NAME[] = "172.17.101.34"; 
String PATH_NAME = "/capstone/insert_temp.php"; //changed
String queryString = "?temperature=35.1";       //gonna overwrite this within the loop hopefully
String float_str = "empty";

// Body Detection Variables //
BreathHeart_60GHz radar = BreathHeart_60GHz(&Serial1);
float BedDistance = 0.f;
float BodyThickness = 10.f;                     //cm
static float BodyDistance = 300.f;              //cm
static unsigned int HeartRate = 0;
static unsigned int BreathingRate = 0;
unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;

//Todo: change itno structu
float SendData[4] = {0};                        //0: Distance, 1: Average HeartRate, 2: Breathing Rate
int HeartRateStorage[2] = {1,0};                 //0: Length of Data, 2-10: HeartRate Data


void setup() {

// Serial Set up //
  Serial.begin(115200);
  Serial1.begin(115200);
  startMillis = millis();  //initial start time
  //mySerial.begin(115200);

  while(!Serial);                               //When the serial port is opened, the program starts to execute.
  while(!Serial1);

  Serial1.println("Readly1");
  Serial.println("Readly2");

  // WIFI Stuff //
  /*
  if (WiFi.status() == WL_NO_MODULE) {          // check for the WiFi module:
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // print your board's IP address:
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  //WIFI STUFF
  */
}

void loop()
{
  // put your main code here, to run repeatedly:

  while(BedDistance == 0.f) 
  {
    Serial.println("distance from the bed");
    BedDistance = 95.0f;//VALIDINPUT(Serial.parseFloat());
  }

  if (Serial1.available() && (BedDistance != 0.f))
  {
    //Serial1 isn't always avaialable, it fails a bit and then it avaiable
    //Serial1 is the serial ports pin 0, 1 used for the board
    //printRawData(radar);
    //humanDetection(radar, BodyDistance); //sets bodyDistance to a new value
    //[radardetect, distancedetect] = humanDetection(radar, bodyDistance); //cannot use to get fxtn bc void fxtn so just trust we can take these vars
    //implement smth here that'll send bodyDistance to 

  /*
    //WIFI STUFF
      // connect to web server on port 80:
          // connect to web server on port 80:
    if (client.connect(HOST_NAME, HTTP_PORT)) {
      // if connected:
      Serial.println("Connected to server");
      // make a HTTP request:
      // send HTTP header
      float_str = String(BodyDistance); //convert bodyDistance to string for PHP url
      queryString = "?temperature="+float_str;
      client.println(HTTP_METHOD + " " + PATH_NAME + queryString + " HTTP/1.1");
      Serial.println(HTTP_METHOD + " " + PATH_NAME + queryString); //wanna know which PHP link to go to
      delay(10000); //added bc want to give myself 60s to check if the link works
      client.println("Host: " + String(HOST_NAME));
      client.println("Connection: close");
      client.println();  // end HTTP header

      while (client.connected()) {
        if (client.available()) {
          // read an incoming byte from the server and print it to serial monitor:
          char c = client.read();
          Serial.print(c);
          //Serial.println("check");
        }
      }

      // the server's disconnected, stop the client:
      //client.stop(); //
      Serial.println();
      Serial.println("not disconnected bc removed that line");
    } else {  // if not connected:
      Serial.println("connection failed");
    }
  //WIFI STUFF
    //WIFI STUFF
  */

/*
    Serial.println(BodyDistance); 
    if (BodyDistance*100 <40.f || BodyDistance*100 >= 300.f)
    {
      Serial.println("Error in reading measurement");
      Serial.println(BodyDistance);
    }
    else if (BodyDistance*100 <= (BedDistance - BodyThickness))
    {
      Serial.println("Someone is present infront of the bed");
    }
    else if (BodyDistance*100 > (BedDistance*TOLERANCE))
    {
      Serial.println("Bed not detected");
    }
    else
    {
      Serial.println("No user detected");
    }
    */
    //heartRateDetection(radar);

    radar.HumanExis_Func();           //Human existence information output
    if(radar.sensor_report != 0x00){
      switch(radar.sensor_report){
        case DISVAL:
        // Modifies returnValue value
        /*
          Serial.print("The sensor judges the distance to the human body to be: ");
          Serial.print(radar.distance, DEC);
          Serial.println(" m");
          Serial.println("----------------------------");
          */

          BodyDistance = radar.distance;

          break;
      }
    }
    delay(200);                       //Add time delay to avoid program jam
    radar.Breath_Heart();           //Breath and heartbeat information output
    if(radar.sensor_report != 0x00){
      switch(radar.sensor_report){
        case HEARTRATEVAL:
          Serial.print("Sensor monitored the current heart rate value is: ");
          Serial.println(radar.heart_rate, DEC);
          Serial.println("----------------------------");

          // Store HeartRate
          // add counter, rolling average
          if(HeartRateStorage[0]<10)
          //TODO: reject invalid heartrates
          //heartrate = heartrate/count
          //count++
            HeartRateStorage[1] = (HeartRateStorage[1] + radar.heart_rate);
            HeartRateStorage[0] += 1;
          break;

        case BREATHVAL:
          Serial.print("Sensor monitored the current breath rate value is: ");
          Serial.println(radar.breath_rate, DEC);
          Serial.println("----------------------------");
          BreathingRate = radar.breath_rate;

          break;

      }
    }
    delay(200);                       //Add time delay to avoid program jam


  }


  currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
  // can delete this for just the some
  if (currentMillis - startMillis >= PERIOD)  //test whether the period has elapsed
  {
    //Calculate average HeartRate
    float AvgHeartRate = HeartRateData[1]/HeartRateData[0];
    HeartRateData[0] = 1;
    HeartRateData[1] = 0;

    
    SendData[0] = BodyDistance;
    SendData[1] =float(AvgHeartRate);
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
    startMillis = currentMillis;
  }

 
}

void printRawData(BreathHeart_60GHz radar)
{
  //200ms delay? to prevent jamming of data, idek
  //prints raw data
  radar.recvRadarBytes();           //Receive radar data and start processing
  radar.showData();                 //Serial port prints a set of received data frames
  delay(200);
}

void humanDetection(BreathHeart_60GHz radar, float& returnValue)
{

/* Output: No return value, but modifies the value returnValue by reference. */

  radar.HumanExis_Func();           //Human existence information output
  if(radar.sensor_report != 0x00){
    switch(radar.sensor_report){
      case NOONE:
        Serial.println("Nobody here.");
        Serial.println("----------------------------");
        break;
      case SOMEONE:
        Serial.println("Someone is here.");
        Serial.println("----------------------------");
        break;
      /*case NONEPSE:
        Serial.println("No human activity messages.");
        Serial.println("----------------------------");
        break; */
      /*case BODYVAL:
        Serial.print("The parameters of human body signs are: ");
        Serial.println(radar.bodysign_val, DEC);
        Serial.println("----------------------------");
        break; */
      case DISVAL:
      // Modifies returnValue value
        Serial.print("The sensor judges the distance to the human body to be: ");
        Serial.print(radar.distance, DEC);
        Serial.println(" m");
        Serial.println("----------------------------");

        returnValue = radar.distance; //where body distance is being calculated so it gets saved from this function? rewrites it?

        break;
    }
  }
  delay(200);                       //Add time delay to avoid program jam
}

void heartRateDetection(BreathHeart_60GHz radar)
{
  radar.Breath_Heart();           //Breath and heartbeat information output
  if(radar.sensor_report != 0x00){
    switch(radar.sensor_report){
      case HEARTRATEVAL:
        Serial.print("Sensor monitored the current heart rate value is: ");
        Serial.println(radar.heart_rate, DEC);
        Serial.println("----------------------------");
        break;
      case HEARTRATEWAVE:  //Valid only when real-time data transfer mode is on
        Serial.print("The heart rate waveform(Sine wave) -- point 1: ");
        Serial.print(radar.heart_point_1);
        Serial.print(", point 2 : ");
        Serial.print(radar.heart_point_2);
        Serial.print(", point 3 : ");
        Serial.print(radar.heart_point_3);
        Serial.print(", point 4 : ");
        Serial.print(radar.heart_point_4);
        Serial.print(", point 5 : ");
        Serial.println(radar.heart_point_5);
        Serial.println("----------------------------");
        break;
      case BREATHNOR:
        Serial.println("Sensor detects current breath rate is normal.");
        Serial.println("----------------------------");
        break;
      case BREATHRAPID:
        Serial.println("Sensor detects current breath rate is too fast.");
        Serial.println("----------------------------");
        break;
      case BREATHSLOW:
        Serial.println("Sensor detects current breath rate is too slow.");
        Serial.println("----------------------------");
        break;
      case BREATHNONE:
        Serial.println("There is no breathing information yet, please wait...");
        Serial.println("----------------------------");
        break;
      case BREATHVAL:
        Serial.print("Sensor monitored the current breath rate value is: ");
        Serial.println(radar.breath_rate, DEC);
        Serial.println("----------------------------");
        break;
      case BREATHWAVE:  //Valid only when real-time data transfer mode is on
        Serial.print("The breath rate waveform(Sine wave) -- point 1: ");
        Serial.print(radar.breath_point_1);
        Serial.print(", point 2 : ");
        Serial.print(radar.breath_point_2);
        Serial.print(", point 3 : ");
        Serial.print(radar.breath_point_3);
        Serial.print(", point 4 : ");
        Serial.print(radar.breath_point_4);
        Serial.print(", point 5 : ");
        Serial.println(radar.breath_point_5);
        Serial.println("----------------------------");
        break;
    }
  }
  delay(200);                       //Add time delay to avoid program jam

}