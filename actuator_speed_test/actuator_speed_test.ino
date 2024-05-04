#include "DualG2HighPowerMotorShield.h"

// Uncomment the version corresponding with the version of your shield.
// DualG2HighPowerMotorShield24v14 md;
// DualG2HighPowerMotorShield18v18 md;
// DualG2HighPowerMotorShield24v18 md;
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
  // put your main code here, to run repeatedly:
  int speed;
  Serial.print("Enter a speed");
  while (!Serial.available()) {
    //wait for input
  }

  speed = Serial.parseInt();
  Serial.println(speed);

 if (speed < 0) {
  speed = -speed;
 }

  delay (1000);

    md.enableDrivers();
  delay(1);  // The drivers require a maximum of 1ms to elapse when brought out of sleep mode.

  for (int i = 0; i <= speed; i++)
  {
    md.setM1Speed(i);
    md.setM2Speed(i);
    stopIfFault();
    //if (i%200 == 100)
    {
      Serial.print("M1 current: ");
      Serial.println(md.getM1CurrentMilliamps());
       Serial.print("M2 current: ");
      Serial.println(md.getM2CurrentMilliamps());
	}
    delay(2);
  }
 if (speed > 0) {
  speed = -speed;
 }

  for (int i = 0; i >= -speed; i--)
  {
    md.setM1Speed(i);
    md.setM2Speed(i);

    stopIfFault();
    //if (i%200 == 100)
    {
      Serial.print("M1 current: ");
      Serial.println(md.getM1CurrentMilliamps());
       Serial.print("M2 current: ");
      Serial.println(md.getM2CurrentMilliamps());
	}
    delay(2);
  }


  md.disableDrivers(); // Put the MOSFET drivers into sleep mode.
  delay(500);

}
