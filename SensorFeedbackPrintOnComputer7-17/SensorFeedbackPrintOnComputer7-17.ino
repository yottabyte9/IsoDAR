#include <DualG2HighPowerMotorShield.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_NAU7802.h>
#include <MPU6050.h>

DualG2HighPowerMotorShield18v22 md;
Adafruit_MPU6050 mpu;
Adafruit_NAU7802 nau;
MPU6050 mpu2;

int j = 0; //for slowing printout

volatile float rotPosX; //will store position of the ADC
volatile float strain; //will store the value from the strain guage
float strainOffset; //will store the offset from the strain guage

//speed for the motors
int m1_speed = 0; //default back to stop!
int m2_speed = 0;

int motor_stop = 0;
int m_forward = -200; //for strain guage adjustments
int m_back = 200;
int m_forward_slow = -100; //for angle adjustments -- too much faster than 100 and they will miss and adjust past where they need and on and on
int m_back_slow = 100;

float angleSensitive = 1.0; //tolerance. make this number smaller for more leveling precision, but too small and it will never level out
int strainSensitive = 1000; //tolerance. this value can swing kind of wildly, so this number should stay above 100 at least


// make everything stop moving if power to one of the motors is disrupted
void stopIfFault() {
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

//routine for when sensors go out of tolerance
void Adjust() {

  Serial.println("\ncorrection");

  //adjust speeds based on what's happening
  if (rotPosX > angleSensitive) {
    //flag
   
      Serial.println("Angle correction");
    
    m1_speed = m_back_slow;
    m2_speed = motor_stop;

  } else if (rotPosX < -angleSensitive) {
    //flag
    
      Serial.println("Angle correction");
    
    m1_speed = motor_stop;
    m2_speed = m_back_slow;

  } else if (strain > strainSensitive) {
    //flag

      Serial.println("Strain guage correction");
    
    m1_speed = m_forward;
    m2_speed = m_forward;

  } else if (strain < -strainSensitive) {
    //flag

      Serial.println("Strain guage correction");
    
    m1_speed = m_back;
    m2_speed = m_back;
  }
}

void setup() {
  Serial.begin(115200);

  Serial.println("\n\nstart test - 'SensorFeedbackPrintOnComputer7-17'\n");
  delay(10);

  //turn on and check sensors
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

  delay(1000); //let everything turn on and "warm up"
  
  //turn on and calibrate motor shield
  Serial.println("initializing motor shield\n\n");
  md.init();
  md.calibrateCurrentOffsets();

  //calibrate MPU
  mpu2.setXAccelOffset(835);  //lower number tilts ccw

  //calibrate ADC
  strainOffset = nau.read(); // gonna need to make this the average of the first 10ish values eventually
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

  strain = nau.read() - strainOffset; //reading from load sensor

  float m1Current = md.getM1CurrentMilliamps(); //currents read from the motor shield. not sure how accurate these are
  float m2Current = md.getM2CurrentMilliamps(); 

  //if one of the sensor readings goes out of tolerance, send to the adjustment routine 
  if ((abs(rotPosX) > angleSensitive) or (abs(strain) > strainSensitive)) {
    Adjust();
  } else {
    m1_speed = motor_stop;
    m2_speed = motor_stop;
  }

  //header for printout. adjust remainder to have it print more or less often
  //if ((j % 100) == 0) {
  //  Serial.println("\n\nRotational Position \tStrain \t \tMotor 1 Speed \tMotor 2 Speed \tMotor 1 Current Motor 2 Current");
  //}
  
  //value printout. adjust remainder to grab data more or less often -- more is more precise, but goes faster and harder to read
  //if ((j % 100) == 0) {

    Serial.print(rotPosX);

    Serial.print("\t \t \t");
    Serial.print(strain);

    Serial.print("\t \t");
    Serial.print(m1_speed);
    Serial.print("\t \t");
    Serial.print(m2_speed);

    Serial.print("\t \t");
    Serial.print(m1Current);
    Serial.print("\t \t");
    Serial.println(m2Current);
  //}
  //j is incremented for slowing printouts 
  j ++;
}

