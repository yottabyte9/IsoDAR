/*
variable dump
  global
    j = the number of times void loop has run
    num_loops = the number of times the void loop is allowed to run
    m1_speed = speed of motor 1, eventually controlled by pot
    m2_speed = speed of motor 2, eventually controlled by pot
  loop
    i = the number of times the sensor data printing while loop runs before reprinting the headers
*/

#include "DualG2HighPowerMotorShield.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_NAU7802.h>

DualG2HighPowerMotorShield18v22 md;
Adafruit_MPU6050 mpu;
Adafruit_NAU7802 nau;

void stopIfFault()
{
  if (md.getM1Fault())
  {
    md.disableDrivers();
	delay(1);
    Serial.println("M1 fault");
    while (1);
  }
  if (md.getM2Fault())
  {
    md.disableDrivers();
	delay(1);
    Serial.println("M2 fault");
    while (1);
  }
}

void setup() {
  Serial.begin(115200);

  Serial.println("\n\nstart test of arms and sensors\n");
  delay(10);

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050");
    } else {
      Serial.println("MPU6050 Found!");
    }
  if (!nau.begin()) {
    Serial.println("Failed to find NAU7802");
    } else {
      Serial.println("NAU7802 Found!");
    }
  
  Serial.println("initializing motor shield\n");
  md.init();
  md.calibrateCurrentOffsets();

}

int j = 0; //for break statement 
int num_loops = 100; //number here is number of times the loop will run

//speed for the motors -- should be controlled by pots!
int m1_speed = 300;
int m2_speed = 300;

void loop() {
  
  if (j < num_loops) { 
    
    //turn on and set up motors
    md.enableDrivers();
    delay(1);  // The drivers require a maximum of 1ms to elapse when brought out of sleep mode.
    //motor1
    md.setM1Speed(m1_speed);
    stopIfFault();
    //motor2
    md.setM2Speed(m2_speed);
    stopIfFault();
        

        
    /* Print out the sensor values */
    Serial.println("Acceleration (m/s^2) \t \tRotation (rads/s) \t \tLoad \t \t \tMotor1 Current \tMotor2 Current");
    Serial.println("X: \tY: \tZ: \t\tX: \tY: \tZ: \t\tStrain: \t \tmA: \t \tmA:");
    int i = 10;
    while (i > 0) {
      sensors_event_t a, g, temp; //a=accel, g=gyro, temp -- reading from 6-way accel/gyro sensor
      mpu.getEvent(&a, &g, &temp);

      float strain = nau.read(); //reading from load sensor

      float m1Current = md.getM1CurrentMilliamps();
      float m2Current = md.getM2CurrentMilliamps();    

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

      Serial.print("\t \t");
      Serial.print(m1Current);
      Serial.print("\t \t");
      Serial.print(m2Current);


      //md.disableDrivers(); // Put the MOSFET drivers into sleep mode.
      delay(500);

      i --;
    
    }
    Serial.println("\n\n");
    j ++;
  }

  if (j >= num_loops) {
    md.disableDrivers();
  }

}