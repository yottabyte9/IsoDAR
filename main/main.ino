#include <Adafruit_MPU6050.h>
#include <Adafruit_NAU7802.h>
#include <Sabertooth.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// Define I2C addresses
#define SSD1306_ADDRESS 0x2A
#define MPU6050_ADDRESS 0x68


Adafruit_MPU6050 mpu; //6 direction tilt sensor
Adafruit_NAU7802 nau; //strain sensor, negative values = resistance
Sabertooth ST(128);

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
int mode = 1;
MPUSensorValues initial_tilt;
MPUSensorValues past_tilt;


MotorValues mvals;


void setup(){

  Serial.begin(9600);
  delay(1000);

  if(MPUSetup(mpu)){
    Serial.println("Failed to find MPU6050 chip");
  } else {
      Serial.println("MPU6050 Found!");
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);

  if(NAUSetup(nau)){
    Serial.println("Failed to find NAU7802");
  } else {
      Serial.println("NAU7802 Found!");
  }
  
  delay(1000);
  strain = nau.read(); //read in initial strain after 1 second (first second gives random values)
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  initial_tilt.accelZ = a.acceleration.z;
  initial_tilt.accelX = a.acceleration.x;

  //MotorSetup();
  SabertoothTXPinSerial.begin(9600); // 9600 is the default baud rate for Sabertooth packet serial.
  ST.autobaud(); // Send the autobaud command to the Sabertooth controller(s).

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
  Serial.println("Completed Setup");
}


void loop() {
  MPUSensorValues MPUValues = MPUReadValues(mpu);   // read new 6-axis values
  //MPUPrintValues(MPUValues);                        // print those values on serial monitor
  double NAUValues = NAUSensorValueBase(nau);       // read new strain gauge values
  double NAUValuesAdjusted = NAUPrintValuesAdjusted(NAUValues, strain); // print those values on serial monitor
  if (Serial.available() > 0) {
      String input = Serial.readStringUntil('\n');  // Read input as string until newline
      mode = input.toInt();                        // Convert to integer
      Serial.print("Raw input: ");
      Serial.println(input);
      Serial.print("Parsed mode: ");
      Serial.println(mode);                        // Display parsed mode
  }
  MPUValues.accelY *= -1;
  Serial.print("Tilt: ");
  Serial.println(MPUValues.accelY);
  
  if(mode == 0){
    ST.stop();
    delay(1);
    Serial.println("Manual user termination");
    while (1);
  }
  else if(mode == 1){
    Serial.println("Do motor set level function \n \n");
    MotorSetLevel(MPUValues.accelY);
    Serial.println("completed motor set level function \n \n");
  }
  else if(mode == 2){
    bool smacked = false;
    if( abs( abs(MPUValues.accelZ) - abs(initial_tilt.accelZ)) > 4){
      for(int i=0; i<5; i++){
        Serial.println("YOU SMACKED IT");
      }
      smacked = true;
    }
  
    if( abs( abs(MPUValues.accelX) - abs(initial_tilt.accelX)) > 4){
      for(int i=0; i<5; i++){
        Serial.println("YOU SMACKED IT");
      }
      smacked = true;
    }
  
    if( abs( abs(MPUValues.accelY) - abs(past_tilt.accelY)) > 0.5){
      for(int i=0; i<5; i++){
        Serial.println("YOU SMACKED IT");
      }
      smacked = true;
    }
  
    if(!smacked){
      mvals = MotorMove(MPUValues.accelY, NAUValuesAdjusted, mvals);
      past_tilt = MPUValues;
    }
  }
  else if(mode == 3){
    ST.motor(1, 50);
    ST.motor(2, 50);
    Serial.println("Junk");
    Serial.println("M1 Speed: 50");
    delay(10);
    Serial.println("M2 Speed: 50");
    delay(1000);
    ST.motor(1, 0);
    ST.motor(2, 0);
    Serial.println("Junk");
    Serial.println("M1 Speed: 0");
    delay(10);
    Serial.println("M2 Speed: 0");
    MotorSetLevel(MPUValues.accelY);
    MotorSetLevel(MPUValues.accelY);
    MotorSetLevel(MPUValues.accelY);
    delay(1000);
  }
  else if(mode == 4){
    ST.motor(1, 50);
    Serial.println("Junk");
    Serial.println("M1 Speed: 50");
  }
  else if(mode == 5){
    ST.motor(1, 0);
    Serial.println("Junk");
    Serial.println("M1 Speed: 0");
  }
  else if(mode == 6){
    ST.motor(1, -50);
    Serial.println("Junk");
    Serial.println("M1 Speed: -50");
  }
  else if(mode == 7){
    ST.motor(2, 50);
    Serial.println("Junk");
    Serial.println("M2 Speed: 50");
  }
  else if(mode == 8){
    ST.motor(2, 0);
    Serial.println("Junk");
    Serial.println("M2 Speed: 0");
  }
  else if(mode == 9){
    ST.motor(2, -50);
    Serial.println("Junk");
    Serial.println("M2 Speed: -50");
  }
 


  // Check for I2C errors for both devices
  Wire.beginTransmission(SSD1306_ADDRESS);
  byte error1 = Wire.endTransmission();
  if (error1) {
    i2cFaultDetected = true;
    Serial.print(F("I2C error with SSD1306: "));
    Serial.println(error1);
    ST.motor(1, 0);
    ST.motor(2, 0);
    mode == 8;
  }

  Wire.beginTransmission(MPU6050_ADDRESS);
  byte error2 = Wire.endTransmission();
  if (error2) {
    i2cFaultDetected = true;
    Serial.print(F("I2C error with MPU6050: "));
    Serial.println(error2);
    ST.motor(1, 0);
    ST.motor(2, 0);
    mode == 8;
    
  }
  if (i2cFaultDetected) {
    Serial.println(F("I2C fault detected!"));
    ST.motor(1, 0);
    ST.motor(2, 0);
    mode == 8;
  }
  
  delay(10);
}
