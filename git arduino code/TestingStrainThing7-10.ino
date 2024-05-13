// TestingStrainThing7-10
// H.Ross
// 2023-07-10
//
// Operates both actuators when the strain gauge is positive or negative.
// The motors both move at the same speed but direction is determined by the sign of the strain.

// Features
// -----------------------------------------------------------------------------------
//  - Check for motor fault M1, M2
//  - Initialize ADC (nau7802)
//  - Initialize Pololu motor shield
//  - Read strain gauge via ADC
//  - Move M1, M2 forward if strain gauge reports greater than 1000 ADC counts
//  - Move M1, M2 backward if strain gauge reports less than 1000 ADC counts
//  - Report motor current, motor speed and strain values to Serial Monitor
// ------------------------------------------------------------------------------------

#include "DualG2HighPowerMotorShield.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_NAU7802.h>

DualG2HighPowerMotorShield18v22 md;
Adafruit_MPU6050 mpu;
Adafruit_NAU7802 nau;


volatile float strain;
volatile float lastStrain;

//zero out offset for load sensor -- this needs to be an average of a few values 
//float offset_load = nau.read();
float strainOffset; // gonna need to make this the average of the first 10ish values eventually

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

  Serial.println("\n\nstart test\n");
  delay(10);

  if (!nau.begin()) {
    Serial.println("Failed to find NAU7802");
    } else {
      Serial.println("NAU7802 Found!");
    }

  Serial.println("initializing motor shield\n\n");
  md.init();
  md.calibrateCurrentOffsets();

  delay(1000); //let everything turn on and "warm up"

//set up offset
  // int numVals = 10;
  // int i = 0;
  // int val;
  // while (i < numVals) {
  //   int NewVal = abs(nau.read());
  //   val = val + NewVal;
    
  //   Serial.print(i);
  //   Serial.print("\tnew val: ");
  //   Serial.print(NewVal);
  //   Serial.print("\t\t total: ");
  //   Serial.println(val);
  //   delay(10);
  //   i++;
  // }
  // strainOffset = - (val / numVals);
  strainOffset = nau.read();
  // Serial.println(strainOffset);

  Serial.println("Rotational Position \tStrain \t \t \tMotor 1 Speed \tMotor 2 Speed \tMotor 1 Current \tMotor 2 Current");
  //Serial.println("X: \tY: \tZ: \t\tX: \tY: \tZ: \t\tStrain: \tSetting: \tSetting:");

}

int j = 0; //for break statement and printing in serial

//speed for the motors -- should be controlled by user input eventually 
volatile int m1_speed = 0;
volatile int m2_speed = 0;

int motor_stop = 0;
int m_forward = -200;
int m_back = 200;

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

  strain = nau.read() - strainOffset; //reading from load sensor -- the +25xxx is an offset

  float m1Current = md.getM1CurrentMilliamps();
  float m2Current = md.getM2CurrentMilliamps(); 


  if (abs(strain) > abs(1000)) {
    StrainOverload();
  } else {
    m1_speed = motor_stop;
    m2_speed = motor_stop;
  }


  if ((j % 100) == 0) {
    Serial.print("\t \t \t");
    Serial.print(strain);

    Serial.print("\t \t \t");
    Serial.print(m1_speed);
    Serial.print("\t \t");
    Serial.print(m2_speed);

    Serial.print("\t \t");
    Serial.print(m1Current);
    Serial.print("\t \t");
    Serial.println(m2Current);
  }
    //Serial.println("\n\n");
  j ++;
}


void StrainOverload() {
  //Serial.print("\nStrain guage reading! \n");
  //float strain2 = nau.read() + 25650;
  if ((nau.read() - strainOffset) > 1000) {
    m1_speed = m_forward;
    m2_speed = m_forward;
  } else if ((nau.read() - strainOffset) < -1000) {
    m1_speed = m_back;
    m2_speed = m_back;
  }
}