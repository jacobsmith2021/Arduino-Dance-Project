/*
 sends a count to the other arduino and also prints it to the LED Matrix
  Made from Arduino MQTT Client Simple Recieve Demo
  Arduino Dance Project
  Jacob Smith
  8/20/24
*/

#include <ArduinoMqttClient.h>
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
  #include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
  #include <WiFi101.h>
#elif defined(ARDUINO_ARCH_ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ARDUINO_PORTENTA_H7_M7) || defined(ARDUINO_NICLA_VISION) || defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_GIGA) || defined(ARDUINO_OPTA)
  #include <WiFi.h>
#elif defined(ARDUINO_PORTENTA_C33)
  #include <WiFiC3.h>
#elif defined(ARDUINO_UNOR4_WIFI)
  #include <WiFiS3.h>
#endif

#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;    // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"
	
ArduinoLEDMatrix matrix;
	
// To connect with SSL/TLS:
// 1) Change WiFiClient to WiFiSSLClient.
// 2) Change port value from 1883 to 8883.
// 3) Change broker value to a server with a known SSL/TLS root certificate 
//    flashed in the WiFi module.

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "test.mosquitto.org";
int        port     = 1883;
const char topic[]  = "arduino/simple";

const long interval = 1000;
unsigned long previousMillis = 0;

int count = 1;
void setup() {
  matrix.begin();
  Serial.begin(9600);
  delay(2000);
  // add some static text
  // will only show "UNO" (not enough space on the display)
  matrix.textScrollSpeed(50);
  matrix.textFont(Font_5x7); // can also be 4 x 6
  print_text("Sen");
  //Initialize serial and wait for port to open:
  
  // attempt to connect to WiFi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }
  print_text("wifi");



  // You can provide a unique client ID, if not set the library uses Arduino-millis()
  // Each client must have a unique client ID
  // mqttClient.setId("clientId");

  // You can provide a username and password for authentication
  // mqttClient.setUsernamePassword("username", "password");
  delay(3000);
  int error_count=0;
  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    print_text("E"+String(error_count));
    error_count++;
    delay(1000);
  }

  print_text("send");
 
}

void loop() {
  // call poll() regularly to allow the library to send MQTT keep alives which
  // avoids being disconnected by the broker
  mqttClient.poll();

  // to avoid having delays in loop, we'll use the strategy from BlinkWithoutDelay
  // see: File -> Examples -> 02.Digital -> BlinkWithoutDelay for more info
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;

    Serial.print("Sending message to topic: ");
    Serial.println(topic);
    Serial.print("count ");
    Serial.println(count);

    // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(topic);
    mqttClient.print(count);
    mqttClient.endMessage();

    Serial.println();

    count++;
    print_text(String(count));
  }

}

void print_text(String text){
  Serial.println(text);
  long length=text.length();
  while (length<4){
    text+=" ";
    length++;
  }
 // Make it scroll!
//  matrix.beginDraw();
//  matrix.stroke(0xFFFFFFFF);
// matrix.beginText(0, 1, 0xFFFFFF);
// matrix.println(text);
// matrix.endText(SCROLL_LEFT);
// matrix.endDraw();
// non scrolling
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(text);
  matrix.endText();
  matrix.endDraw();
  //delay(100);

}
