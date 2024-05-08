void test1(DualG2HighPowerMotorShield18v22 &md){
  Serial.println("im here");
  md.enableDrivers();
  delay(1);  // The drivers require a maximum of 1ms to elapse when brought out of sleep mode.
  //motor1
  md.setM1Speed(200);
  //motor2
  md.setM2Speed(200);
  delay(1000);
  md.setM1Speed(0);
  //motor2
  md.setM2Speed(0);
  delay(1000);
  md.setM1Speed(-200);
  //motor2
  md.setM2Speed(-200);
  delay(1000);
}
