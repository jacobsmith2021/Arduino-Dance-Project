/*
Arduino Dance Project Motion Prototype Version 4

Version 1: using rotation sensor to turn on vibrotactors
Version 2: sending rotation wirelessly using ArduinoDanceprojectReceive
Version 3: using calibration code to get position,sending which vibrotactor to vibrate and also vibrating them on the local machine
Version 4: sending which tactors to vibrate wirelessly, also incldues receive mode configurable via jumper pin

Using the  MPU6050 acceleromoter, gyroscpe,and termperature sensor and digital outputs based off the 
X, Y,and Z Axis thresholds

Based on AdaFruit MPU6050 Example
from https://github.com/jacobsmith2021/Arduino-Dance-Project

To Use
1) Solder to header pins safely
2) Plug MPU6050 sensor into GND, VCC to 5V, SDA, and SCL  on Arduino chip
3) Plug Vibrotactors, LEDS or any other digital output into digital ports 0, 1, and 2, and 3
  make sure your digital output circuit is correct, LED's usually need a 220 Ω (Ohm, unit for resistance) resistor
4) If you want to use Recieve mode, bring a jumper wire from 3.4 V or 5 V  to digital port 13
5) download this program to your chip and see the values you get from the serial monitor and the digital outputs 

To Do
  1) Let multiple arduino chips send and recieve at the same time
*/
//values to change

//the digitial ports of the vibrotactors
int X_PIN=0;
int Y_PIN=1;
int X_PIN_BEHIND=2;
int Y_PIN_BEHIND=3;

//the pin to check if this should be in send or recieve mode
int JUMPER_PIN=13;

// how many degrees to set the vibrotactors by
float THRESHOLD=20;
//how often to send the sensor values to other arduinos
const long INTERVAL = 1000;
//the name of the queue of sensor values
const char topic[]  = "arduino/arduinodanceproject";

// whether to print debug information
bool PRINT_DEBUG_DATA=false;
//end values to change

//library imports
#include <ArduinoMqttClient.h>
#include <WiFiS3.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <MPU6050_light.h>
#include "arduino_secrets.h"
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"

//objects defined
ArduinoLEDMatrix matrix;
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);
MPU6050 mpu(Wire);

//global variables defined
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;    // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

const char broker[] = "test.mosquitto.org";
//mosquitto server run for free
int        port     = 1883;

//time to wait to print out numbers
unsigned long previousMillis = 0;

int count = 0;

float x;
float y;

float x_axis_value=0;
float y_axis_value=0;

bool x_axis_at_threshold=false;
bool y_axis_at_threshold=false;

bool x_axis_behind_threshold=false;
bool y_axis_behind_threshold=false;

bool WIFI_WORKING=true;
bool MESSAGE_SUBSCRIPTION_WORKING=true;
bool SEND_MODE;

String message;

String parsed_x_axis_value;

String parsed_y_axis_value;

int messageSize;

char character;

unsigned long currentMillis = 0;

int split_index;

void setup(void) {
  double setup_start_time=millis();
  delay(1000);
  Serial.begin(115200);                         
  Wire.begin();
  matrix.begin();

  pinMode(JUMPER_PIN,INPUT);
  pinMode(X_PIN, OUTPUT);
  pinMode(Y_PIN, OUTPUT);
  pinMode(X_PIN_BEHIND, OUTPUT);
  pinMode(Y_PIN_BEHIND, OUTPUT);

  digitalWrite(X_PIN, HIGH);  
  digitalWrite(Y_PIN, HIGH);  
  digitalWrite(X_PIN_BEHIND, HIGH);  
  digitalWrite(Y_PIN_BEHIND, HIGH);  
  
  delay(250);

  digitalWrite(X_PIN, LOW);  
  digitalWrite(Y_PIN, LOW);  
  digitalWrite(X_PIN_BEHIND, LOW);  
  digitalWrite(Y_PIN_BEHIND, LOW);  

  // add some static text
  // will only show "UNO" (not enough space on the display)
  matrix.textScrollSpeed(50);
  matrix.textFont(Font_5x7); // can also be 4 x 6
  //Initialize serial and wait for port to open:

  previousMillis=0;
  int max_serial_time=2000;
  while(!Serial){
    currentMillis = millis();
    if (currentMillis-previousMillis>max_serial_time){
      Serial.print("Serial Monitor Not Connected but took more than\t");
      Serial.print(max_serial_time);
      Serial.println("\t,quitting");
      delay(3000);
      break;
    }
    delay(250);
  }
  
  SEND_MODE=not digitalRead(JUMPER_PIN);
  Serial.println("\n\nBrandeis University Arduino Dance Project\n");
  if (SEND_MODE){
    Serial.println("will use this chip's position to run the vibrotactors");
    Serial.println("setting up acceleration sensor, don't move");
    mpu.begin();
    mpu.calcGyroOffsets();                          // This does the calibration  
    Serial.println("done setting up accleration sensor"); 
  }else{
    Serial.println("will use another chip's position to run the vibrotactors");
  }


  // attempt to connect to WiFi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  int wifi_error_count=0;
  int max_wifi_error_count=3;
  while (true) {
    if(WiFi.begin(ssid, pass) == WL_CONNECTED){
      Serial.println("\nwifi working");
      break;
    }
    
    if(wifi_error_count>max_wifi_error_count){
      Serial.print("\nWIFI error count was greater than\t");
      Serial.print(max_wifi_error_count);
      Serial.println("\t,will run without WIFI");
      delay(3000);
      WIFI_WORKING=false;
      break;
    }
    // failed, retry
    Serial.print(".");
    wifi_error_count++;
  }
  if (WIFI_WORKING){
    Serial.print("Attempting to connect to message broker\t");
    Serial.print(broker);
    Serial.print("\tand topic\t");
    Serial.println(topic);
    int message_subscription_error_count=0;
    int max_message_subscription_error_count=10;
    while (true){
      if(mqttClient.connect(broker, port)){
        Serial.println("\nMessage Subscription Working");
        break;
      }
      if(message_subscription_error_count>max_message_subscription_error_count){
        Serial.print("\nMessage Subscription error count was greater than max error count\t");
        Serial.print(max_message_subscription_error_count);
        Serial.print(", Error code = ");
        Serial.print(mqttClient.connectError());
        Serial.println(", quitting");
        delay(3000);
        MESSAGE_SUBSCRIPTION_WORKING=false;
        break;
      }
      message_subscription_error_count++;
      Serial.print(",");
      delay(3000);
    }
    if (!SEND_MODE){
      Serial.print("subscribing to topic\t");
      Serial.print(topic);
      Serial.println();
      mqttClient.subscribe(topic);
    }
  }
  double setup_end_time=millis();
  double setup_elapsed_seconds=(setup_end_time-setup_start_time)/1000;
  Serial.print("Completed Setup in\t");
  Serial.print(setup_elapsed_seconds);
  Serial.println("\tseconds");
}


void loop() {
  currentMillis = millis();
  count+=1;
  
  // if send mode
  if (SEND_MODE){
    // update the acclerometer values
    mpu.update();  
    x_axis_value=mpu.getAngleX();
    y_axis_value=mpu.getAngleY();
    mqttClient.poll();
    // send the message every INTERVAL milliseconds
    if (currentMillis - previousMillis >= INTERVAL) {
      message=String(x_axis_value)+","+String(y_axis_value);

      Serial.print("Sending Sensor Data:\t");
      Serial.println(message);

      mqttClient.beginMessage(topic);
      mqttClient.print(message);
      mqttClient.endMessage();
      
      previousMillis = currentMillis;
  }
  // if not in send mode, receive messages
  }else{
    messageSize = mqttClient.parseMessage();
    
    if (messageSize) {
      Serial.print("received message size:\t");
      Serial.print(messageSize);
      Serial.print("\t");
      message="";
      while (mqttClient.available()) {
        character=char((mqttClient.read()));
        message+=character;
        Serial.print(character);
      }
      Serial.println();
      split_index=message.indexOf(",");
      parsed_x_axis_value=message.substring(0,split_index);
      parsed_y_axis_value=message.substring(split_index+1,message.length());
      x_axis_value=parsed_x_axis_value.toFloat();
      y_axis_value=parsed_y_axis_value.toFloat();
    }

  }
  
  x_axis_at_threshold=x_axis_value>THRESHOLD;
  y_axis_at_threshold=y_axis_value>THRESHOLD;

  x_axis_behind_threshold=x_axis_value<THRESHOLD*-1;
  y_axis_behind_threshold=y_axis_value<THRESHOLD*-1;
  if (SEND_MODE){
    Serial.print("sending  \t");
  }else{
    Serial.print("receiving\t");
  }
  Serial.print("\tX ");
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
  if (PRINT_DEBUG_DATA){
    Serial.print("\tcount\t");
    Serial.print(count);
    Serial.print("\tcurrentmillis\t");
    Serial.print(currentMillis);
    Serial.print("\tpreviousmillis\t");
    Serial.print(previousMillis);
    Serial.print("\tdiff\t");
    Serial.print(currentMillis-previousMillis);
  }
  Serial.println();

  digitalWrite(X_PIN, x_axis_at_threshold);  
  digitalWrite(Y_PIN, y_axis_at_threshold);  
  digitalWrite(X_PIN_BEHIND, x_axis_behind_threshold);  
  digitalWrite(Y_PIN_BEHIND, y_axis_behind_threshold);  
  delay(100);
}
