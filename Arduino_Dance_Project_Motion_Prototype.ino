/*
Arduino Dance Project Motion Prototype

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
  Serial.print(x_axis_v alue);
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



}