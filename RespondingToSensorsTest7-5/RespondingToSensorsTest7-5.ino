#include "DualG2HighPowerMotorShield.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_NAU7802.h>

DualG2HighPowerMotorShield18v22 md;
Adafruit_MPU6050 mpu;
Adafruit_NAU7802 nau;

volatile float strain;
volatile float lastStrain;

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

  Serial.println("initializing motor shield\n\n");
  md.init();
  md.calibrateCurrentOffsets();

  delay(1000); //let everything turn on and "warm up"

  //zero out offset for load sensor
  std::set<float> ADC_avg; 
  int x = 0;
  if (x < 50) {
    ADC_avg.insert(nau.read());
    x ++;
  }


  Serial.println("Acceleration (m/s^2) \t \tRotation (rads/s) \t \tLoad \t \tMotor 1 Speed \tMotor 2 Speed");
  Serial.println("X: \tY: \tZ: \t\tX: \tY: \tZ: \t\tStrain: \tSetting: \tSetting:");

}

int j = 0; //for break statement 
int num_loops = 10000; //number here is number of times the loop will run

//speed for the motors -- should be controlled by user input eventually 
volatile int m1_speed = 100;
volatile int m2_speed = 67;

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


    if (abs(strain) > (1000 + abs(lastStrain))) {
      StrainOverload();
    }
    lastStrain = strain;



    sensors_event_t a, g, temp; //a=accel, g=gyro, temp -- reading from 6-way accel/gyro sensor
    mpu.getEvent(&a, &g, &temp);

    float accelX = a.acceleration.x;
    float accelY = a.acceleration.y;
    float accelZ = a.acceleration.z;

    float gyroX = g.gyro.x;
    float gyroY = g.gyro.y;
    float gyroZ = g.gyro.x;

    strain = nau.read() + 25100; //reading from load sensor -- the +25xxx is an offset

    if ((j % 100) == 0) {
      Serial.println("");
      Serial.print(accelX);
      Serial.print("\t");
      Serial.print(accelY);
      Serial.print("\t");
      Serial.print(accelZ);

      Serial.print("\t \t");
      Serial.print(gyroX);
      Serial.print("\t");
      Serial.print(gyroY);
      Serial.print("\t");
      Serial.print(gyroZ);

      //Serial.print("\nStrain: ");
      Serial.print("\t \t");
      Serial.print(strain);

      Serial.print("\t \t");
      Serial.print(m1_speed);
      Serial.print("\t \t");
      Serial.print(m2_speed);

    }
    //Serial.println("\n\n");
    j ++;
  }

  if (j >= num_loops) {
    md.disableDrivers();
  }
}

void StrainOverload() {
  Serial.print("\n Strain guage reading alert! \n");
  if (strain > (1000 + lastStrain)) {
    m1_speed ++;
    m2_speed = m1_speed;
  } else {
    m1_speed --;
    m2_speed = m1_speed;
  }

}