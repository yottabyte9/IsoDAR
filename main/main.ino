#include <Adafruit_MPU6050.h>
#include <Adafruit_NAU7802.h>
#include <DualG2HighPowerMotorShield.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// Define I2C addresses
#define SSD1306_ADDRESS 0x2A
#define MPU6050_ADDRESS 0x68


Adafruit_MPU6050 mpu; //6 direction tilt sensor
Adafruit_NAU7802 nau; //strain sensor, negative values = resistance
DualG2HighPowerMotorShield18v22 md; //motors (-400 to 400)

bool i2cFaultDetected = false; // Flag to track I2C faults


//Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);

struct MotorValues{ //struct for previous iteration motor speeds
  double M1Speed;
  double M2Speed;
};

struct MPUSensorValues { //struct for tilt sensor (6 values)
    double accelX, accelY, accelZ;
    double gyroX, gyroY, gyroZ;
};

double strain; //initial strain
int keeprun = 1;

MotorValues mvals;


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

  // Initialize I2C devices
  Wire.beginTransmission(0x2A);
  if (Wire.endTransmission() != 0) {
    Serial.println(F("Failed to find device at 0x2A"));
  }

  Wire.beginTransmission(0x68);
  if (Wire.endTransmission() != 0) {
    Serial.println(F("Failed to find device at 0x68"));
  }
  mvals.M1Speed = 0;
  mvals.M2Speed = 0;
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
  
  mvals = MotorMove(md, MPUValues.accelY, NAUValuesAdjusted, mvals);
  //md.setM1Speed(200);
  //md.setM2Speed(200);

  // Check for I2C errors for both devices
  Wire.beginTransmission(SSD1306_ADDRESS);
  byte error1 = Wire.endTransmission();
  if (error1) {
    i2cFaultDetected = true;
    Serial.print(F("I2C error with SSD1306: "));
    Serial.println(error1);
  }

  Wire.beginTransmission(MPU6050_ADDRESS);
  byte error2 = Wire.endTransmission();
  if (error2) {
    i2cFaultDetected = true;
    Serial.print(F("I2C error with MPU6050: "));
    Serial.println(error2);
  }
  if (i2cFaultDetected) {
    Serial.println(F("I2C fault detected!"));
  }


  delay(1);

}
