#include "Arduino.h"
#include "60ghzbreathheart.h"
#include "RTC.h"
#include "WiFiS3.h"

//Marcos
#define TOLERANCE 1.1f
#define VALIDINPUT(x) (x>40.f && x<200.f) ? x : 0.f;

//#include <SoftwareSerial.h>
// Choose any two pins that can be used with SoftwareSerial to RX & TX
//#define RX_Pin A2
//#define TX_Pin A3

//SoftwareSerial mySerial = SoftwareSerial(RX_Pin, TX_Pin);

// we'll be using software serial
//BreathHeart_60GHz radar = BreathHeart_60GHz(&mySerial);

// can also try hardware serial with

//Variables
BreathHeart_60GHz radar = BreathHeart_60GHz(&Serial1);
float setBedDistance = 0.f;
float bodyThickness = 10.f; //cm
static float bodyDistance = 300.f; //cm

void setup() {
  // put your setup code here, to run once:
  //specs for serial1 is 115200
  Serial.begin(115200);
  Serial1.begin(115200);


  //  mySerial.begin(115200);

  while(!Serial);   //When the serial port is opened, the program starts to execute.

  Serial1.println("Readly1");
  //.println("Readly2");
}

void loop()
{
  // put your main code here, to run repeatedly:

  if(setBedDistance == 0.f)
  {
    Serial.print("distance from the bed");
    setBedDistance = VALIDINPUT(Serial.parseFloat());
  }

  if (Serial1.available() && (setBedDistance != 0.f))
  {
    //Serial1 isn't always avaialable, it fails a bit and then it avaiable
    //Serial1 is the serial ports pin 0, 1 used for the board
    //printRawData(radar);
    humanDetection(radar, bodyDistance); //sets bodyDistance to a new value
    if (bodyDistance <0.4f || bodyDistance >= 300.f)
    {
      Serial.println("Error in reading measurement");
    }
    else if (bodyDistance <= (setBedDistance - bodyThickness))
    {
       Serial.println("Someone is present infront of the bed");
    }
    else if (bodyDistance > (setBedDistance*TOLERANCE))
    {
      Serial.println("Bed not detected");
    }
    else
    {
      Serial.println("No user detected");
    }
    //heartRateDetection(radar);
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
      case BODYVAL:
        Serial.print("The parameters of human body signs are: ");
        Serial.println(radar.bodysign_val, DEC);
        Serial.println("----------------------------");
        break;
      case DISVAL:
      // Modifies returnValue value
        Serial.print("The sensor judges the distance to the human body to be: ");
        Serial.print(radar.distance, DEC);
        Serial.println(" m");
        Serial.println("----------------------------");

        returnValue = radar.distance;

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