#include <Adafruit_MPU6050.h>
#include <Adafruit_NAU7802.h>
#include <DualG2HighPowerMotorShield.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

Adafruit_MPU6050 mpu; //6 direction tilt sensor
Adafruit_NAU7802 nau; //strain sensor, negative values = resistance
DualG2HighPowerMotorShield18v22 md; //motors (-400 to 400) positive is up

//Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);

struct MPUSensorValues { //struct for tilt sensor (6 values)
    double accelX, accelY, accelZ;
    double gyroX, gyroY, gyroZ;
};

double strain; //initial strain
int keeprun = 1;

void setup() {
  Serial.begin(115200);
  delay(1000);

  if(MPUSetup(mpu)){
    Serial.println("Failed to find MPU6050 chip");
  } else {
      Serial.println("MPU6050 Found!");
  }

  if(NAUSetup(nau)){
    Serial.println("Failed to find NAU7802");
  } else {
      Serial.println("NAU7802 Found!");
  }
  
  delay(1000);
  strain = nau.read(); //read in initial strain after 1 second (first second gives random values)
  
  MotorSetup(md);
    
}


void loop() {
  MPUSensorValues MPUValues = MPUReadValues(mpu);
  MPUPrintValues(MPUValues);
  double NAUValues = NAUSensorValueBase(nau);
  double NAUValuesAdjusted = NAUPrintValuesAdjusted(NAUValues, strain);
  while (Serial.available() > 0) {
      keeprun = Serial.parseInt();
  }
  StopIfFault(md, keeprun);

  
  if(!MotorTiltMove(md, MPUValues.accelY, NAUValuesAdjusted)){
    MotorStrainMove(md, NAUValuesAdjusted);
  }

  delay(100);
  
  //md.setM2Speed(300);
  //md.setM1Speed(100);
}
