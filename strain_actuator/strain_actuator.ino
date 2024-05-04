#include <DualG2HighPowerMotorShield.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_NAU7802.h>

DualG2HighPowerMotorShield18v22 md;
Adafruit_MPU6050 mpu;
Adafruit_NAU7802 nau;

volatile float strain;
volatile float lastStrain;
volatile int m1_s = 100;
volatile int m2_s = 100;

void stopIfFault() {
  if (md.getM1Fault()) {
    md.disableDrivers();
    delay(1);
    Serial.println("M1 Fault");
    while(1);
  }

    if (md.getM2Fault()) {
    md.disableDrivers();
    delay(1);
    Serial.println("M2 Fault");
    while(1);
  }
}

void StrainOverload() {
  Serial.println("Strain Overload");
  if(strain > (lastStrain +1000)) {
    m1_s ++;
    m2_s = m1_s;
  }
  else {
    m1_s --;
    m2_s = m1_s;
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

  delay(1000); //power on
  

  // average base reading
  int n = 0;
//  if (n < 100) {
//    ADC_avg.insert(nau.read());
//    n++;
//  }
//  delay(100);
}
int i =0;
int j = 1000;
void loop() {
    //turn on and set up motors
    md.enableDrivers();
    delay(1);  // The drivers require a maximum of 1ms to elapse when brought out of sleep mode.
    //motor1
    md.setM1Speed(m1_s);
    stopIfFault();
    //motor2
    md.setM2Speed(m2_s);
    stopIfFault();
    strain = nau.read();
    const float new_str = nau.read();
    if ((strain + 100) > strain) {
      md.setM1Speed(0);
      md.setM2Speed(0);
    }

    if ((strain - 100) < strain) {
      md.setM1Speed(0);
      md.setM2Speed(0);
    }
    if (strain < 80000) {
      md.setM1Speed(m1_s);
      md.setM1Speed(m2_s);
    }
    else {
      md.setM1Speed(-m1_s);
      md.setM2Speed(-m2_s);
    }

    Serial.println("Acceleration \t \t \tRotation");
  Serial.println("X: \tY: \tZ: \t\tX: \tY: \tZ:");
  if (i < j) {
    sensors_event_t a, g, temp; 
    mpu.getEvent(&a, &g, &temp);

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
    Serial.print(m1_s);
    Serial.print("\t \t");
    Serial.print(m2_s);


    i ++;
    delay(1000); 
    
  }
  Serial.println("\n\n");
  lastStrain = strain;

}
