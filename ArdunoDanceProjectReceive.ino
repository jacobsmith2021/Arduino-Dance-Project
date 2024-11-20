/*
  Recieves the count from the sender arduino and prints it to the LED matrix
  Made from Arduino MQTT Client Simple Recieve Demo
  Arduino Dance Project
  V2 now activates viboratctors base don the axis value
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

// To connect with SSL/TLS:
// 1) Change WiFiClient to WiFiSSLClient.
// 2) Change port value from 1883 to 8883.
// 3) Change broker value to a server with a known SSL/TLS root certificate 
//    flashed in the WiFi module.

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "test.mosquitto.org";
int        port     = 1883;
const char topic[]  = "arduino/simple2";

#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"
	
ArduinoLEDMatrix matrix;

int X_PIN=0;
int Y_PIN=1;
int Z_PIN=2;


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
  pinMode(X_PIN, OUTPUT);
  pinMode(Y_PIN, OUTPUT);
  pinMode(Z_PIN, OUTPUT);
}

void setup() {
  matrix.begin();
  Serial.begin(9600);
  delay(2000);
  // add some static text
  // will only show "UNO" (not enough space on the display)
  matrix.textScrollSpeed(50);
  matrix.textFont(Font_4x6); // can also be 4 x 6
  print_text("rec");

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

  print_text("list");

  // subscribe to a topic
  mqttClient.subscribe(topic);

  // topics can be unsubscribed using:
  // mqttClient.unsubscribe(topic);

  print_text("read");
}
float message_float;
void loop() {
  int messageSize = mqttClient.parseMessage();
  if (messageSize) {
    print_text("!");
    // we received a message, print out the topic and contents
    Serial.print("Received a message with topic '");
    Serial.print(mqttClient.messageTopic());
    Serial.print("', length ");
    Serial.print(messageSize);
    Serial.println(" bytes:");
    String message="";
    char character;

    // use the Stream interface to print the contents
    while (mqttClient.available()) {
      character=char((mqttClient.read()));
      message+=character;
      Serial.print(character);
    }
    Serial.println();
    Serial.println("message is\t");
    Serial.println(message);
    Serial.println("message as a float");
    message_float=message.toFloat();
    message_float/=10;
    Serial.println(message_float);
    if (message_float<-45){
      digitalWrite(X_PIN, HIGH);
    }else{
    digitalWrite(X_PIN, LOW);
    }

    if (message_float>45){
      digitalWrite(Y_PIN, HIGH);   
    }else{
      digitalWrite(Y_PIN, LOW); 
    } 
    delay(100);
    //print_text(message);
  }
}


