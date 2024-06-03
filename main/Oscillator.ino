#include "DualG2HighPowerMotorShield.h"
 
// Uncomment the version corresponding with the version of your shield.
// DualG2HighPowerMotorShield24v14 md;
// DualG2HighPowerMotorShield18v18 md;
// DualG2HighPowerMotorShield24v18 md;
 DualG2HighPowerMotorShield18v22 md;
 
void stopIfFault()
{
  if (md.getM2Fault())
  {
    md.disableDrivers();
    delay(1);
    Serial.println("M2 fault");
    while (1);
  }
}
 
void setup()
{
  Serial.begin(115200);
  Serial.println("Dual G2 High Power Motor Shield");
  md.init();
  md.calibrateCurrentOffsets();
 
  delay(10);
  
  // Uncomment to flip a motor's direction:
  //md.flipM2(true);
  md.enableDrivers();
  delay(1);  // The drivers require a maximum of 1ms to elapse when brought out of sleep mode.
  
}
 
void loop()
{
  int keeprun = 1;
  while (Serial.available() > 0) {
      keeprun = Serial.parseInt();
  }
  if(keeprun == 0){
    md.disableDrivers();
    delay(1);
    Serial.println("Manual user termination");
    while (1);
  }
  //md.setM2Speed(-100);
  
  md.setM2Speed(50);
  for(int i=0; i<30000; i++){
    while (Serial.available() > 0) {
      keeprun = Serial.parseInt();
    }
    if(keeprun == 0){
      md.disableDrivers();
      delay(1);
      Serial.println("Manual user termination");
      while (1);
    }
    delay(1);
  }
  md.setM2Speed(0);
  for(int i=0; i<30000; i++){
    while (Serial.available() > 0) {
      keeprun = Serial.parseInt();
    }
    if(keeprun == 0){
      md.disableDrivers();
      delay(1);
      Serial.println("Manual user termination");
      while (1);
    }
    delay(1);
  }
  md.setM2Speed(-50);
  for(int i=0; i<30000; i++){
    while (Serial.available() > 0) {
      keeprun = Serial.parseInt();
    }
    if(keeprun == 0){
      md.disableDrivers();
      delay(1);
      Serial.println("Manual user termination");
      while (1);
    }
    delay(1);
  }
  md.setM2Speed(0);
  for(int i=0; i<120000; i++){
    while (Serial.available() > 0) {
      keeprun = Serial.parseInt();
    }
    if(keeprun == 0){
      md.disableDrivers();
      delay(1);
      Serial.println("Manual user termination");
      while (1);
    }
    delay(1);
  }
  
}
