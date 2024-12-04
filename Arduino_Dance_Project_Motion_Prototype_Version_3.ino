/*
Arduino Dance Project Motion Prototype Version 3

Version 1: using rotation sensor to turn on vibrotactors
Version 2: sending rotation wirelessly using ArduinoDanceprojectReceive
Version 3: using calibration code to get position,sending which vibrotactor to vibrate and also vibrating them on the local machine


Using the  MPU6050 acceleromoter, gyroscpe,and termperature sensor and digital outputs based off the 
X, Y,and Z Axis thresholds

Based on AdaFruit MPU6050 Example
from https://github.com/jacobsmith2021/Arduino-Dance-Project

To Use
1) Solder to header pins safely
2) Plug MPU6050 sensor into GND, VCC to 5V, SDA, and SCL  on Arduino chip
3) Plug LEDS or any other digital output into idigital ports 0, 1, and 2
  make sure your digital output circuit is correct, LED's usually need a 220 Ω (Ohm, unit for resistance) resistor
4) download this program to your chip and see the values you get from the serial monitor and the digital outputs 

To Do
  A) figure out how to use the Z Axis
  B) make this wireless to another arduino
*/
//values to change
int X_PIN=0;
int Y_PIN=1;
int X_PIN_BEHIND=2;
int Y_PIN_BEHIND=3;

float THRESHOLD=20;
//end values to change

#include <ArduinoMqttClient.h>
//Using for communication
//The rest of it is to pick the right library for each type of chip
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
//Whenever using the chip use it
  
ArduinoLEDMatrix matrix;
//matrix is the variable allows us to print out stuff
  
// To connect with SSL/TLS:
// 1) Change WiFiClient to WiFiSSLClient.
// 2) Change port value from 1883 to 8883.
// 3) Change broker value to a server with a known SSL/TLS root certificate 
//    flashed in the WiFi module.

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "test.mosquitto.org";
//mosquitto server run for free
int        port     = 1883;
const char topic[]  = "arduino/simple2";

const long interval = 200;
//time to wait to print out numbers
unsigned long previousMillis = 0;

int count = 1;

//#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <MPU6050_light.h>

 
MPU6050 mpu(Wire);
unsigned long timer = 0;

float x_axis_value=0;
float y_axis_value=0;

bool x_axis_at_threshold=false;
bool y_axis_at_threshold=false;

bool x_axis_behind_threshold=false;
bool y_axis_behind_threshold=false;



float x;
float y;
long start_time=millis();

long current_time;

String message;

void setup(void) {

    Serial.begin(115200);                           // Ensure serial monitor set to this value also    
  delay(2000);
  Serial.println("setting up, don't move");
  Wire.begin();
  mpu.begin();
  mpu.calcGyroOffsets();                          // This does the calibration  
  Serial.println("done setup"); 

  matrix.begin();
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

  pinMode(X_PIN, OUTPUT);
  pinMode(Y_PIN, OUTPUT);
  pinMode(X_PIN_BEHIND, OUTPUT);
  pinMode(Y_PIN_BEHIND, OUTPUT);
  
  delay(1000);

  digitalWrite(X_PIN, HIGH);  
  digitalWrite(Y_PIN, HIGH);  
  digitalWrite(X_PIN_BEHIND, HIGH);  
  digitalWrite(Y_PIN_BEHIND, HIGH);  
  
  delay(1000);

  digitalWrite(X_PIN, LOW);  
  digitalWrite(Y_PIN, LOW);  
  digitalWrite(X_PIN_BEHIND, LOW);  
  digitalWrite(Y_PIN_BEHIND, LOW);  
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

    //Serial.print("Sending message to topic: ");
    //Serial.println(topic);
    //Serial.print("count ");
    //Serial.println(count);

    // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(topic);
    //essage="".concat(x_axis_at_threshold).concat("-").concat(y_axis_at_treshold);
    Serial.println(message);
    mqttClient.print(message);

    Serial.println();
    mqttClient.endMessage();

    //Serial.println(y_axis_value);

    count++;
    print_text(String(count));
  }

   mpu.update();  
  current_time=millis();
 

 

  if((start_time-timer)>10)                         // print data every 10ms
  {                                           
  
    start_time = millis();  
  }
  delay(100);
}

void print_text(String text){
  Serial.println(text);
  long length=text.length();
  while (length<4){
    text+=" ";
    length++;
  }

  /* Get new sensor events with the readings */
  
  x_axis_value=mpu.getAngleX();
  y_axis_value=mpu.getAngleY();
 

  x_axis_at_threshold=x_axis_value>THRESHOLD;
  y_axis_at_threshold=y_axis_value>THRESHOLD;

  x_axis_behind_threshold=x_axis_value<THRESHOLD*-1;
  y_axis_behind_threshold=y_axis_value<THRESHOLD*-1;


  Serial.print("X ");
  Serial.print(x_axis_value);
  Serial.print("\t");
  Serial.print(x_axis_at_threshold);
  Serial.print("\t");
  Serial.print(x_axis_behind_threshold);
  Serial.print("\tY ");
  Serial.print(y_axis_value);
  Serial.print("\t");
  Serial.print(y_axis_at_threshold);
  Serial.print("\t");
  Serial.print(y_axis_behind_threshold);
  Serial.println();


  digitalWrite(X_PIN, x_axis_at_threshold);  
  digitalWrite(Y_PIN, y_axis_at_threshold);  
  digitalWrite(X_PIN_BEHIND, x_axis_behind_threshold);  
  digitalWrite(Y_PIN_BEHIND, y_axis_behind_threshold);  


  delay(100);



}
