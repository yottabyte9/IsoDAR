// divides actuator motion into various modes
// 8-1: working on tilt routine
//POSITIVE IS UP
//this version almost works!
//  current issues: 
//  modes 3 and 4 don't respond to strain input correctly yet
//    mode 3: trying to use button to reset where 0 strain is, but not working yet
//  tilt routines need to be actually figured out lol

#include "DualG2HighPowerMotorShield.h"
#include <Adafruit_MPU6050.h>
#include <MPU6050.h>
#include <Adafruit_NAU7802.h>
#include "RunningAverage.h"

DualG2HighPowerMotorShield18v22 md;
Adafruit_MPU6050 mpu;
MPU6050 mpu2;
Adafruit_NAU7802 nau;
RunningAverage rav(10); //the numerical value here is how many values to include in the average

unsigned long timeRun = millis(); // hold miliseconds since program has started running
unsigned long timeRunLoop;

// pin assignments
const int mode2Pin = A2;
const int mode3Pin = 11;
const int mode4Pin = 3;
const int directionSwitchPin = 5; 
const int buttonPin = A3;

// variables for sensors
float rotPosX; // will store position of the MPU
int max_speed = 100; // maximum possible adjustment speed is this + 50 -- change this value to make it adjust faster over whole range of tilts. max is 350
int max_rot = 10; // Don't change this. tilt sensor goes from 0 to 10 in ccw direction and 0 to -10 in cw direction. 

int strain; // will store the value from the strain gauge
int strain_rav; //will store the running average value of the last 10 strains
int strain_off; // will store the offset from the strain gauge
int rav_samps = 0; //number of samples average program is allowed to hold before it gets cleared again to not take up so much memory

  // tolerances
float angleSensitive = 1.0; // tolerance. make this number smaller for more leveling precision, but too small and it will never level out
int strainSensitive = 1500; // tolerance. this value can swing kind of wildly

// speed for the motors
int m1_speed; // store speed for left motor
int m2_speed; // store speed for right motor

int motor_stop = 0; // stop!!!!!!!

  // mode 2 -- manual control mode
int mode2MoveUp = 200; //whether this value is positive or negative depends on the orientation of the strain gauge lol
int mode2MoveDown = - 200; 

  // mode 3 -- for injection
int mode3Speed; // will store the proportional speed for strain gauge

  // mode 4 -- for decompression
int mode4Speed; // will store the proportional speed for strain gauge

  // tilt speed can be called from any void
int tilt_speed; // holds the speeds that change with the tilt angle


//controls for the various modes

// make everything stop moving if power to one of the motors is disrupted. This routine comes from Pololu
void stopIfFault() {
  if (md.getM1Fault()) {
    md.disableDrivers();
	  delay(1);
    while(1);
  }
  if (md.getM2Fault()) {
    md.disableDrivers();
	  delay(1);
    while(1);
  }
}

void Mode2() { //manual control mode. moves arms up and down, direction based on switch 

  while(digitalRead(mode2Pin) == HIGH) { //these while loops exist in all the mode voids so that it'll a) keep the program in the loop as long as the switch is turned in that direction, and b) the program will kick back to the main loop when the switch is changed. I have no idea if this will work lol

    // motor1
    md.setM1Speed(m1_speed);
    stopIfFault();
    // motor2
    md.setM2Speed(m2_speed);
    stopIfFault();
    
    Serial.print("Mode 2: Manual Control Mode\t"); 
    Serial.print("tilt: ");
    Serial.print(rotPosX);
    Serial.print("\t\tdigital reads: ");
    Serial.print(digitalRead(mode2Pin));
    Serial.print(",");
    Serial.print(digitalRead(mode3Pin)); 
    Serial.print(",");
    Serial.print(digitalRead(mode4Pin)); 
    Serial.print(",");
    Serial.print(digitalRead(directionSwitchPin));
    Serial.print(",");
    Serial.print(digitalRead(buttonPin));
    Serial.print("\t\tMotor 1 speed: ");
    Serial.print(m1_speed);
    Serial.print("\tMotor 2 Speed: ");
    Serial.println(m2_speed);
   

    //detect tilt
    int n = 0;
    if ((n % 10) == 0) {
      sensors_event_t a, g, temp; //a=rotational Position, g=gyro, temp -- reading from 6-way accel/gyro sensor
      mpu.getEvent(&a, &g, &temp); //I have no idea why this is structured in two lines like this but it's what Adafruit says to do
      rotPosX = a.acceleration.x; //grabs value from mpu

      //if one of the sensor readings goes out of tolerance, send to the adjustment routine 
      if (abs(rotPosX) > angleSensitive) {
        tilt();
      } 
    n ++;
    }
    
    if (digitalRead(buttonPin) == HIGH) {
      if(digitalRead(directionSwitchPin) == HIGH) { // if switch high, move up 
        m1_speed = mode2MoveDown;
        m2_speed = mode2MoveDown;
      }
      else if(digitalRead(directionSwitchPin) == LOW) { // if switch low, move down
        m1_speed = mode2MoveUp;
        m2_speed = mode2MoveUp;
      }
    }
    else {
      m1_speed = motor_stop;
      m2_speed = motor_stop;      
    }
  }
  
  if (digitalRead(mode2Pin) == LOW) {
    loop();
  }

}

void Mode3() { // Injection mode. Compresses based on pressure

  while(digitalRead(mode3Pin) == HIGH) { 

    // motor1
    md.setM1Speed(m1_speed);
    stopIfFault();
    // motor2
    md.setM2Speed(m2_speed);
    stopIfFault();

    Serial.print("Mode 3: Injection Mode\t\t"); 
    Serial.print("tilt: ");
    Serial.print(rotPosX);
    Serial.print("\tstrain: ");
    Serial.print(strain);
    Serial.print("\tdigital reads: ");
    Serial.print(digitalRead(mode2Pin));
    Serial.print(",");
    Serial.print(digitalRead(mode3Pin)); 
    Serial.print(",");
    Serial.print(digitalRead(mode4Pin)); 
    Serial.print(",");
    Serial.print(digitalRead(directionSwitchPin));
    Serial.print(",");
    Serial.print(digitalRead(buttonPin));
    Serial.print("\tMotor 1 speed: ");
    Serial.print(m1_speed);
    Serial.print("\tMotor 2 Speed: ");
    Serial.println(m2_speed);

    //tilt detection
    int n = 0;
    if (n % 10 == 0) {
      sensors_event_t a, g, temp; //a=rotational Position, g=gyro, temp -- reading from 6-way accel/gyro sensor
      mpu.getEvent(&a, &g, &temp); //I have no idea why this is structured in two lines like this but it's what Adafruit says to do
      rotPosX = a.acceleration.x; //grabs value from mpu

      //if the tilt sensor reading goes out of tolerance, send to the tilt adjustment routine 
      if (abs(rotPosX) > angleSensitive) {
        tilt();
      } 
    }
    n++;

    //Get new 0 for ADC
    if(buttonPin == HIGH) {
      Serial.print("hi");
      rav.clear(); //start with a totally clear array for the strain averages
      int i = 0;
      while (i < 10) {
        int val = nau.read();
        rav.addValue(val);
      i++;
      }
      strain_off = rav.getAverage();
    }

    //Get readings from strain gauge and smooth out strain gauge readings using a running average
    rav.addValue(nau.read()); 
    strain_rav = rav.getAverage();
    rav_samps ++;
    strain = strain_rav - strain_off;
    if (rav_samps > 1000) {
      rav_samps = 0;
      rav.clear();
    }

    //determine speed by strain
    mode3Speed = (- (0.01) * strain) - 30; //might have to change the sign on this -- test strain guage direction

    //nau
    if (strain > strainSensitive) { 
      m1_speed = mode3Speed;
      m2_speed = mode3Speed;
    } 
    else {
      m1_speed = motor_stop;
      m2_speed = motor_stop;
    }

  }
  if (digitalRead(mode3Pin) == LOW) {
    loop();
  }
}

void Mode4() { // Release mode. Decompresses based on pressure, allowing argon to slowly fill bellows after injection is finished. 

  while(digitalRead(mode4Pin) == HIGH) {

    // motor1
    md.setM1Speed(m1_speed);
    stopIfFault();
    // motor2
    md.setM2Speed(m2_speed);
    stopIfFault();

    Serial.print("Mode 4: Decompression Mode");
    Serial.print("tilt: ");
    Serial.print(rotPosX);
    Serial.print("\tstrain: ");
    Serial.print(strain);
    Serial.print("\tdigital reads: ");
    Serial.print(digitalRead(mode2Pin));
    Serial.print(",");
    Serial.print(digitalRead(mode3Pin)); 
    Serial.print(",");
    Serial.print(digitalRead(mode4Pin)); 
    Serial.print(",");
    Serial.print(digitalRead(directionSwitchPin));
    Serial.print(",");
    Serial.print(digitalRead(buttonPin));
    Serial.print("\t\tMotor 1 speed: ");
    Serial.print(m1_speed);
    Serial.print("\tMotor 2 Speed: ");
    Serial.println(m2_speed);

    //tilt detection
    int n = 0;
    if (n % 10 == 0) {
      sensors_event_t a, g, temp; //a=rotational Position, g=gyro, temp -- reading from 6-way accel/gyro sensor
      mpu.getEvent(&a, &g, &temp); //I have no idea why this is structured in two lines like this but it's what Adafruit says to do
      rotPosX = a.acceleration.x; //grabs value from mpu

      //if the tilt sensor reading goes out of tolerance, send to the tilt adjustment routine 
      if (abs(rotPosX) > angleSensitive) {
        tilt();
      } 
    }
    n++;

    //Get readings from strain gauge and smooth out strain gauge readings using a running average
    rav.addValue(nau.read()); 
    strain_rav = rav.getAverage();
    rav_samps ++;
    strain = strain_rav - strain_off;
    if (rav_samps > 100) {
      rav_samps = 0;
      rav.clear();
    }

    //determine speed by strain
    mode3Speed = ((0.01) * strain) + 30; //might have to change the sign on this -- test strain guage direction

    //nau
    if (strain > strainSensitive) { 
      m1_speed = mode3Speed;
      m2_speed = mode3Speed;
    } 
    else {
      m1_speed = motor_stop;
      m2_speed = motor_stop;
    }

  }
  if (digitalRead(mode4Pin) == LOW) {
    loop();
  }
}

void tilt() { // adjust speeds based on what's happening.

  while (abs(rotPosX) > angleSensitive) {
    
    //set up motors
    // motor1
    md.setM1Speed(m1_speed);
    stopIfFault();
    // motor2
    md.setM2Speed(m2_speed);
    stopIfFault();

    sensors_event_t a, g, temp; //a=rotational Position, g=gyro, temp -- reading from 6-way accel/gyro sensor
    mpu.getEvent(&a, &g, &temp); //I have no idea why this is structured in two lines like this but it's what Adafruit says to do
    rotPosX = a.acceleration.x; //grabs value from mpu

    //determine speed by tilt
    tilt_speed = (max_speed / max_rot) * rotPosX;

  //THESE NEED EDITING TO MAKE THE MOVE THE RIGHT DIRECTION
    if ((rotPosX > angleSensitive) && (digitalRead(mode3Pin) == LOW) && (digitalRead(mode4Pin) == LOW)) {  // this one is for mode 1 and 2
      m1_speed = - (tilt_speed + 50); 
      m2_speed = tilt_speed + 50; 
    } 
    else if ((rotPosX < - angleSensitive) && (digitalRead(mode3Pin) == LOW) && (digitalRead(mode4Pin) == LOW)) { // this one is for mode 1 and 2
      m1_speed = - (tilt_speed - 50); 
      m2_speed = tilt_speed - 50; 
    }
    else if ((rotPosX > angleSensitive) && (digitalRead(mode2Pin) == LOW) && (digitalRead(mode3Pin) == HIGH) && (digitalRead(mode4Pin) == LOW)) { // this one is for mode 3 -- ADJUSTS DOWN
      m1_speed = tilt_speed - 50; 
      m2_speed = motor_stop; 
    }
    else if ((rotPosX < - angleSensitive) && (digitalRead(mode2Pin) == LOW) && (digitalRead(mode3Pin) == HIGH) && (digitalRead(mode4Pin) == LOW)) { // this one is for mode 3
      m1_speed = motor_stop; 
      m2_speed = tilt_speed - 50; 
    }
    else if ((rotPosX > angleSensitive) && (digitalRead(mode2Pin) == LOW) && (digitalRead(mode3Pin) == LOW) && (digitalRead(mode4Pin) == HIGH)) {  // for modes 4 -- ADJUSTS UP
      m1_speed = - (tilt_speed + 50); 
      m2_speed = tilt_speed + 50; 
    } 
    else if ((rotPosX < - angleSensitive) && (digitalRead(mode2Pin) == LOW) && (digitalRead(mode3Pin) == LOW) && (digitalRead(mode4Pin) == HIGH)) { // for modes 4
      m1_speed = - (tilt_speed - 50); 
      m2_speed = tilt_speed - 50; 
    } 
    
    Serial.print("tilt: \tPosition: ");
    Serial.print(rotPosX);
    Serial.print("\tMotor 1 speed: ");
    Serial.print(m1_speed);
    Serial.print("\tMotor 2 Speed: ");
    Serial.println(m2_speed);
  }
}

void setup() {
  Serial.begin(115200);

  // set pins for modes
  pinMode(mode2Pin, INPUT);
  pinMode(mode3Pin, INPUT);
  pinMode(mode4Pin, INPUT);
  pinMode(directionSwitchPin, INPUT);
  pinMode(buttonPin, INPUT);

  digitalWrite(mode2Pin, LOW);
  digitalWrite(mode3Pin, LOW);
  digitalWrite(mode4Pin, LOW);
  digitalWrite(directionSwitchPin, LOW);
  digitalWrite(buttonPin, LOW);
  
  // set up sensors
  
  // turn things on
  // turn on mpu
  mpu.begin();
  // turn on strain gauge/adc
  nau.begin();
  nau.setRate(NAU7802_RATE_10SPS);
  // turn on motor shield
  md.init();

  // let everything turn on and "warm up"
  delay(1000); 
  
  //set up ADC
  rav.clear(); //start with a totally clear array for the strain averages
  int i = 0;
  while (i < 10) {
    int val = nau.read();
    rav.addValue(val);
    i++;
  }
  strain_off = rav.getAverage();


  // calibrate motor shield
  md.calibrateCurrentOffsets();

  // calibrate MPU
  mpu2.setXAccelOffset(835);  // lower number tilts ccw

  //activate motors
  md.enableDrivers();
  delay(1);  // The drivers require a maximum of 1ms to elapse when brought out of sleep mode.

  // timer
  unsigned long timeNow = millis();
  timeRunLoop = millis() - timeNow; //time starts at the loop once everything is turned on
}

void loop() { // "Mode 1"

  m1_speed = motor_stop;
  m2_speed = motor_stop;
  
  //detect tilt
  int n = 0;
  if ((n % 10) == 0) {
    sensors_event_t a, g, temp; //a=rotational Position, g=gyro, temp -- reading from 6-way accel/gyro sensor
    mpu.getEvent(&a, &g, &temp); //I have no idea why this is structured in two lines like this but it's what Adafruit says to do
    rotPosX = a.acceleration.x; //grabs value from mpu

    //if one of the sensor readings goes out of tolerance, send to the adjustment routine 
    if (abs(rotPosX) > angleSensitive) {
      tilt();
    } 
    else {
      m1_speed = motor_stop;
      m2_speed = motor_stop;
    }
  }
  n ++;
  
  Serial.print("Mode 1: waiting\t\t");
  Serial.print("tilt: ");
  Serial.print(rotPosX);
  Serial.print("\t\tdigital reads: ");
  Serial.print(digitalRead(mode2Pin));
  Serial.print(",");
  Serial.print(digitalRead(mode3Pin)); 
  Serial.print(",");
  Serial.print(digitalRead(mode4Pin)); 
  Serial.print(",");
  Serial.print(digitalRead(directionSwitchPin));
  Serial.print(",");
  Serial.print(digitalRead(buttonPin));
  Serial.print("\t\tMotor 1 speed: ");
  Serial.print(m1_speed);
  Serial.print("\tMotor 2 Speed: ");
  Serial.println(m2_speed);

  //send program to correct places depending on switch is on
  if (digitalRead(mode2Pin) == HIGH) {
    Mode2();
  }

  if (digitalRead(mode3Pin) == HIGH) {
    Mode3();
  }

  if (digitalRead(mode4Pin) == HIGH) {
    Mode4();
  }
}
