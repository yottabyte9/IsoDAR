#include "DualG2HighPowerMotorShield.h"
#include <Adafruit_MPU6050.h>
#include <MPU6050.h>

DualG2HighPowerMotorShield18v22 md;
Adafruit_MPU6050 mpu;
MPU6050 mpu2;

volatile float rotPosX; // will store position of the MPU

// speed for the motors
int m1_speed = 0; // default back to stop -- i realize weeks later that this doesn't matter, but I'm leaving it bc it makes me feel better to have more stopping
int m2_speed = 0;

int motor_stop = 0; // stop!!!!!!!

int max_speed = 100; // maximum possible adjustment speed is this + 50 -- change this value to make it adjust faster over whole range of tilts. max is 350
int max_rot = 10; // Don't change this. tilt sensor goes from 0 to 10 in cw direction and 0 to -10 in ccw direction. 
int m_var_speed; // holds the speeds that change with the tilt angle

float angleSensitive = 1.0; // tolerance. make this number smaller for more leveling precision, but too small and it will never level out
int strainSensitive = 1000; // tolerance. this value can swing kind of wildly, so this number should stay above 300 at least

unsigned long timeRun; // hold miliseconds since program has started running

// make everything stop moving if power to one of the motors is disrupted. This routine comes from Pololu
void stopIfFault() {
  if (md.getM1Fault()) {
    md.disableDrivers();
	  delay(1);
    while (1);
  }
  if (md.getM2Fault()) {
    md.disableDrivers();
	  delay(1);
    while (1);
  }
}

// routine for when sensors go out of tolerance
void Adjust() {

  // adjust speeds based on what's happening
  if (rotPosX > angleSensitive) {  // tilt right (positive)
    m1_speed = motor_stop; 
    m2_speed = m_var_speed + 50; // bring up left side
  } 
  else if (rotPosX < -angleSensitive) { // tilt left (negative)
    m1_speed = - (m_var_speed - 50); // bring up right side
    m2_speed = motor_stop;
  } 
}

void setup() {
  Serial.begin(115200);

  // turn on mpu
  mpu.begin();

  // turn on motor shield
  md.init();

  delay(1000); // let everything turn on and "warm up"
  
  // calibrate motor shield
  md.calibrateCurrentOffsets();
  // calibrate MPU
  mpu2.setXAccelOffset(835);  // lower number tilts ccw
}

void loop() {
  // turn on and set up motors
  md.enableDrivers();
  delay(1);  // The drivers require a maximum of 1ms to elapse when brought out of sleep mode.
  // motor1
  md.setM1Speed(m1_speed);
  stopIfFault();
  // motor2
  md.setM2Speed(m2_speed);
  stopIfFault();

  sensors_event_t a, g, temp; //a=rotational Position, g=gyro, temp -- reading from 6-way accel/gyro sensor
  mpu.getEvent(&a, &g, &temp); //I have no idea why this is structured in two lines like this but it's what Adafruit says to do

  rotPosX = a.acceleration.x; //grabs value from mpu

  float m1Current = md.getM1CurrentMilliamps(); //currents read from the motor shield. not sure how accurate these are
  float m2Current = md.getM2CurrentMilliamps(); 

  timeRun = millis() - 1946; //the 1946ms is how long it takes for the program to get to the loop, based on trying it a few times. subtracting means loop starts at 0ms

  //determine speed by tilt
  m_var_speed = (max_speed / max_rot) * rotPosX;

  //if one of the sensor readings goes out of tolerance, send to the adjustment routine 
  if ((abs(rotPosX) > angleSensitive)) {
    Adjust();
  } 
  else {
    m1_speed = motor_stop;
    m2_speed = motor_stop;
  }
  
  //print values. Excel Datastreamer can read values from serial into a csv as long as they're formatted like this. For datastreamer to work, unfortunately, can't print anything else in serial except for comma deliniated values
  Serial.print(timeRun); //how long the program's been running
  Serial.print(",");

  Serial.print(rotPosX); //tilt from MPU

  Serial.print(",");
  Serial.print(m1_speed);
  Serial.print(",");
  Serial.print(m2_speed);

  Serial.print(",");
  Serial.print(m1Current);
  Serial.print(",");
  Serial.println(m2Current);
}

