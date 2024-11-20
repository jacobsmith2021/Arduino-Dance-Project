/*
Arduino Dance Project Motion Prototype Version 2

Version 1: using rotation sensor to turn on vibrotactors
Version 2: sending rotation wirelessly using ArduinoDanceprojectReceive

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

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;
int X_PIN=0;
int Y_PIN=1;
int Z_PIN=2;

float x_axis_value=0;
float y_axis_value=0;
float z_axis_value=0;

bool x_axis_at_threshold=false;
bool y_axis_at_threshold=false;
bool z_axis_at_threshold=false;

float THRESHOLD=2;

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
const char topic[]  = "arduino/simple";

#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"
	
ArduinoLEDMatrix matrix;

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


void setup(void) {
  pinMode(X_PIN, OUTPUT);
  pinMode(Y_PIN, OUTPUT);
  pinMode(Z_PIN, OUTPUT);
  
  Serial.begin(9600);
  delay(3000);
  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);

  digitalWrite(X_PIN, HIGH);  
  digitalWrite(Y_PIN, HIGH);  
  digitalWrite(Z_PIN, HIGH);  
  delay(1000);

  digitalWrite(X_PIN, LOW);  
  digitalWrite(Y_PIN, LOW);  
  digitalWrite(Z_PIN, LOW); 
  delay(1000);


  matrix.begin();
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

void loop() {

  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  x_axis_value=g.gyro.x;
  y_axis_value=g.gyro.y;
  z_axis_value=g.gyro.z;

  x_axis_at_threshold=x_axis_value>THRESHOLD;
  y_axis_at_threshold=y_axis_value>THRESHOLD;
  z_axis_at_threshold=z_axis_value>THRESHOLD;


  Serial.print("X ");
  Serial.print(x_axis_value);
  Serial.print("\t");
  Serial.print(x_axis_at_threshold);
  Serial.print("\tY ");
  Serial.print(y_axis_value);
  Serial.print("\t");
  Serial.print(y_axis_at_threshold);
  Serial.print("\tZ ");
  Serial.print(z_axis_value);
  Serial.print("\t");
  Serial.print(z_axis_at_threshold);
  Serial.println("");



  digitalWrite(X_PIN, x_axis_at_threshold);  
  digitalWrite(Y_PIN, y_axis_at_threshold);  
  digitalWrite(X_PIN, z_axis_at_threshold);  

  delay(100);

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
    print_text(message);
  }


}