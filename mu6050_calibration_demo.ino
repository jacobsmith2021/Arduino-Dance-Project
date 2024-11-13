/*  Arduino Mpu6060 sensor example
* 11/12/24
* Get tilt angles on X and Y, and rotation angle on Z
 * Angles are given in degrees, displays on SSD1306 OLED
 * 
 * License: MIT
 */
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MPU6050_light.h>
 
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
 
 
MPU6050 mpu(Wire);
unsigned long timer = 0;
 
void setup() {
  Serial.begin(115200);                           // Ensure serial monitor set to this value also    
  delay(2000);
  Serial.println("setting up, don't move");
  Wire.begin();
  mpu.begin();
  mpu.calcGyroOffsets();                          // This does the calibration  
  Serial.println("done setup"); 
}

float x;
float y;
float z;
long start_time=millis();

long current_time;
void loop() {
  mpu.update();  
  current_time=millis();
  x=mpu.getAngleX();
  y=mpu.getAngleY();
  z=mpu.getAngleZ();

  Serial.println("x:");
  Serial.println(x);
  Serial.println("x:");
  Serial.println(y);
  Serial.println("x:");
  Serial.println(z);

  if((start_time-timer)>10)                         // print data every 10ms
  {                                           
  
    start_time = millis();  
  }
}
