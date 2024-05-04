//6-23: Working

#include <Adafruit_MPU6050.h>
#include <Adafruit_NAU7802.h>

Adafruit_MPU6050 mpu;
Adafruit_NAU7802 nau;

void setup() {
  Serial.begin(115200);

  Serial.println("\n\nstart test\n");
  delay(10);

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
  Serial.println("Acceleration (m/s^2) \t \tRotation (rads/s) \t \tLoad");
  Serial.println("X: \tY: \tZ: \t\tX: \tY: \tZ: \t\tStrain:");
  int i = 10;
  while (i > 0) {
    sensors_event_t a, g, temp; //a=accel, g=gyro, temp -- reading from 6-way accel/gyro sensor
    mpu.getEvent(&a, &g, &temp);

    float strain = nau.read(); //reading from load sensor

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

    Serial.print("\t \t");
    Serial.print(strain);

    i --;
    delay(1000); 
    
  }
  Serial.println("\n\n");
}