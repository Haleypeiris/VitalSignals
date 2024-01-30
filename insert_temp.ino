/*
 * Created by ArduinoGetStarted.com
 *
 * This example code is in the public domain
 *
 * Tutorial page: https://arduinogetstarted.com/tutorials/arduino-mysql
 */

#include <WiFiS3.h>
//#include "wifi_login.h"  
//const char ssid[] = SECRET_SSID;  // your network SSID (name)
//const char pass[] = SECRET_PASS;  // your network password (use for WPA, or use as key for WEP)
//const char ssid[] = "NatalieiPhone";  // change your network SSID (name)
//const char pass[] = "nataliehaw";   // change your network password (use for WPA, or use as key for WEP)
//const char ssid[] = "Spirit_Birdy";  // change your network SSID (name)
//const char pass[] = "137queenstreetsouth\\,"; //this is the wifi password that works

const char ssid[] = "NatalieiPhone";  // change your network SSID (name)
const char pass[] = "nataliehaw";   // change your network password (use for WPA, or use as key for WEP)

WiFiClient client;
int status = WL_IDLE_STATUS;

int HTTP_PORT = 80;
String HTTP_METHOD = "GET";
//char HOST_NAME[] = "192.168.2.120";  // change to your PC's IP address EACH TIME FOR DIFFERENT NETWORK
char HOST_NAME[] = "172.20.10.2"; //mac wifi
//String PATH_NAME = "/insert_temp.php";
String PATH_NAME = "/capstone/insert_temp.php";
String queryString = "?temperature=33.1"; //change temp as you want

void setup() {
  Serial.begin(9600);
  Serial.println("Test");
  Serial.println(pass);
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.println(pass);
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

  // connect to web server on port 80:
  if (client.connect(HOST_NAME, HTTP_PORT)) {
    // if connected:
    Serial.println("Connected to server");
    // make a HTTP request:
    // send HTTP header
    client.println(HTTP_METHOD + " " + PATH_NAME + queryString + " HTTP/1.1");
    client.println("Host: " + String(HOST_NAME));
    client.println("Connection: close");
    client.println();  // end HTTP header

    while (client.connected()) {
      if (client.available()) {
        // read an incoming byte from the server and print it to serial monitor:
        char c = client.read();
        Serial.print(c);
      }
    }

    // the server's disconnected, stop the client:
    client.stop();
    Serial.println();
    Serial.println("disconnected");
  } else {  // if not connected:
    Serial.println("connection failed");
  }
}

void loop() {
} //issue: printing all the code inside the serial monitor