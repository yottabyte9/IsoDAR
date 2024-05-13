#include "DualG2HighPowerMotorShield.h"
#include <Adafruit_MPU6050.h>
#include <MPU6050.h>

DualG2HighPowerMotorShield18v22 md;
Adafruit_MPU6050 mpu;
MPU6050 mpu2;

volatile float rotPosX; //will store position of the MPU

//speed for the motors
int m1_speed = 0; //default back to stop!
int m2_speed = 0;

int motor_stop = 0;

int max_speed = 200;
int max_rot = 10;
volatile int m_var_speed;

float angleSensitive = 0.5; //tolerance.

void Adjust() {
  //determine speed by tilt
  m_var_speed = (max_speed / max_rot) * rotPosX;

  //sends speeds to motors
  if (rotPosX > angleSensitive) {
    m1_speed = m_var_speed;
    m2_speed = motor_stop;
  } else if (rotPosX < angleSensitive) {
    m1_speed = motor_stop;
    m2_speed = - m_var_speed;
  }
}


void setup() {
  Serial.begin(115200);

  mpu.begin();
  mpu2.setXAccelOffset(840);

  md.init();
  md.calibrateCurrentOffsets();

}

void loop() {
  //turn on and set up motors
  md.enableDrivers();
  delay(1);  // The drivers require a maximum of 1ms to elapse when brought out of sleep mode.
  //motor1
  md.setM1Speed(m1_speed);
  stopIfFault();
  //motor2
  md.setM2Speed(m2_speed);
  stopIfFault();

  sensors_event_t a, g, temp; //a=rotational Position, g=gyro, temp -- reading from 6-way accel/gyro sensor
  mpu.getEvent(&a, &g, &temp);

  rotPosX = a.acceleration.x; //grabs value from mpu

  if (abs(rotPosX) > angleSensitive) {
    Adjust();
  } 
  else {
    m1_speed = motor_stop;
    m2_speed = motor_stop;
  }

  Serial.print(rotPosX);
  Serial.print("\t");
  Serial.print(m1_speed);
  Serial.print("\t");
  Serial.println(m2_speed);
}

// make everything stop moving if power to one of the motors is disrupted
void stopIfFault() {
  if (md.getM1Fault())
  {
    md.disableDrivers();
	delay(1);
    //Serial.println("M1 fault");
    while (1);
  }
  if (md.getM2Fault())
  {
    md.disableDrivers();
	delay(1);
    //Serial.println("M2 fault");
    while (1);
  }
}