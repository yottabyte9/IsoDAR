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
  
  Serial.begin(115200);
  
  //initialize motor driver
  Serial.println("Dual G2 High Power Motor Shield");
  md.init(); //initallizes pinModes and timer1
  md.calibrateCurrentOffsets(); //records voltage offsets from motors when speed 0

}

void loop() {
  md.enableDrivers();
  delay(1);  // The drivers require a maximum of 1ms to elapse when brought out of sleep mode.

  for (int i = 0; i <= 400; i++) // Ramp Motor 1 from full stop to max speed FWD
  {
    md.setM1Speed(i);
    stopIfFault();
    if (i%200 == 100)
    {
      Serial.print("M1 current: ");
      Serial.println(md.getM1CurrentMilliamps());
    }
    
    md.setM2Speed(i);
    stopIfFault();
    if (i%200 == 100)
    {
      Serial.print("M2 current: ");
      Serial.println(md.getM2CurrentMilliamps());
    }
    
    delay(2);
  }


  for (int i = 400; i >= -400; i--) // Ramp Motor 1 from max speed FWD to max speed BWD
  {
    md.setM1Speed(i);
    stopIfFault();
    if (i%200 == 100)
    {
      Serial.print("M1 current: ");
      Serial.println(md.getM1CurrentMilliamps());
	  }

    md.setM2Speed(i);
    stopIfFault();
    if (i%200 == 100)
    {
      Serial.print("M2 current: ");
      Serial.println(md.getM2CurrentMilliamps());
    }

    delay(2);
  }

  for (int i = -400; i <= 0; i++) // Ramp Motor 1 from max speed BWD to full stop
  {
    md.setM1Speed(i);
    stopIfFault();
    if (i%200 == 100)
    {
      Serial.print("M1 current: ");
      Serial.println(md.getM1CurrentMilliamps());
	  }

    md.setM2Speed(i);
    stopIfFault();
    if (i%200 == 100)
    {
      Serial.print("M2 current: ");
      Serial.println(md.getM2CurrentMilliamps());
    }
    
    delay(2);
  }

  md.disableDrivers(); // Put the MOSFET drivers into sleep mode.
  delay(500); 

}
