#include <Adafruit_MPU6050.h>
#include <Adafruit_NAU7802.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

Adafruit_MPU6050 mpu;
Adafruit_NAU7802 nau;

Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);



void setup() {
  Serial.begin(115200);
  delay(1000);


  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    } else {
      Serial.println("MPU6050 Found!");
    }
  if (!nau.begin()) {
    Serial.println("Failed to find NAU7802");
    } else {
      Serial.println("NAU7802 Found!");
    }
  delay(10);



}


void loop() {

  /* Print out the values */
  Serial.println("Acceleration \t \t \tRotation");
  Serial.println("X: \tY: \tZ: \t\tX: \tY: \tZ:");
  int i = 10;
  while (i > 0) {
    sensors_event_t a, g, temp; //a=accel, g=gyro, temp
    mpu.getEvent(&a, &g, &temp);

    Serial.println("");
    Serial.print(a.acceleration.x);
    Serial.print("\t");
    Serial.print(a.acceleration.y);
    Serial.print("\t");
    Serial.print(a.acceleration.z);

    Serial.print("\t \t");
    Serial.print(g.gyro.x);
    Serial.print("\t");
    Serial.print(g.gyro.y);
    Serial.print("\t");
    Serial.print(g.gyro.z);

    i --;
    delay(100); 
    
  }
  
}