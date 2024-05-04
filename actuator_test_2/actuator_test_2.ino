#include "DualG2HighPowerMotorShield.h"

 DualG2HighPowerMotorShield18v22 md;

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
  // put your setup code here, to run once:
Serial.begin(115200);
Serial.println("Dual G2 High Power Motor Shield");
  md.init();
  md.calibrateCurrentOffsets();

  delay(10);

}


void loop() {
  int speed;
  Serial.print("Enter a speed");
  while (!Serial.available()) {
    //wait for input
  }

  speed = Serial.parseInt();
  Serial.println(speed);
  delay (1000);


  if (speed < 0 ) {
    speed = -speed;
    md.setM1Speed(speed);
    md.setM2Speed(speed);
    stopIfFault();
  }

  if (speed > 0) {
    md.setM1Speed(speed);
    md.setM2Speed(speed);
    stopIfFault();
  }
  md.enableDrivers();
  delay(1);  // The drivers require a maximum of 1ms to elapse when brought out of sleep mode.
}
