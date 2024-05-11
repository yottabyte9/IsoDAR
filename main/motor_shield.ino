void MotorSetup(DualG2HighPowerMotorShield18v22 &md){
  md.init();
  md.calibrateCurrentOffsets();
}

void MotorStrainMove(DualG2HighPowerMotorShield18v22 &md, double strain){
  md.setM1Speed(MotorStrainCalculate(strain));
  md.setM2Speed(MotorStrainCalculate(strain));
  Serial.print("M1 Speed: ");
  Serial.println(MotorStrainCalculate(strain));
  Serial.print("M2 Speed: ");
  Serial.println(MotorStrainCalculate(strain));
}

bool MotorTiltMove(DualG2HighPowerMotorShield18v22 &md, double ytilt, double strain){
  double nstrain = MotorStrainCalculate(strain);
  if(nstrain > 0){
    if(ytilt>0.2){
      md.setM1Speed(300);
      md.setM2Speed(100);      
      Serial.print("M1 Speed: ");
      Serial.println(300);
      Serial.print("M2 Speed: ");
      Serial.println(100);
      return true;
    }
    else if(ytilt<-0.2){
      md.setM1Speed(100);
      md.setM2Speed(300);
      Serial.print("M1 Speed: ");
      Serial.println(100);
      Serial.print("M2 Speed: ");
      Serial.println(300);
      return true;
    }
  }
  else{
    if(ytilt>0.2){
      md.setM1Speed(-100);
      md.setM2Speed(-300);
      Serial.print("M1 Speed: ");
      Serial.println(-100);
      Serial.print("M2 Speed: ");
      Serial.println(-300);
      return true;
    }
    else if(ytilt<-0.2){
      md.setM1Speed(-300);
      md.setM2Speed(-100);
      Serial.print("M1 Speed: ");
      Serial.println(-300);
      Serial.print("M2 Speed: ");
      Serial.println(-100);
      return true;
    }
  }
  
  return false;
}

/*
 * -200k (400)
 * -125k -> -75k (0)
 * 0 (-400)
 */
double MotorStrainCalculate(double strain){
  double max_threshold = -200000; 
  double min_threshold = 0
  double target_pressure = -100000; 
  double tolerance = 25000;

  if (strain <= max_threshold) {
    return 399;
  }
  else if (strain >= min_threshold) { 
    return -399;
  }
  else if (strain > max_threshold && strain < target_pressure - tolerance) { // (-200k to -125k)
    double range = abs(max_threshold - (target_pressure - tolerance)); //75k range
    double fraction = abs(strain - (target_pressure - tolerance)) / range; //distance from -125k / range
    return 399 * fraction;
  }
  else if (strain >= target_pressure - tolerance && strain <= target_pressure + tolerance) {
    return 0;
  }
  else if (strain > target_pressure + tolerance && strain < min_threshold) { // (-75k to 0)
    double range = abs(min_threshold - (target_pressure + tolerance)); // 75k range
    double fraction = abs(strain - (target_pressure + tolerance)) / range; // distance from -75k / range
    return -399 * fraction;
  }
  return 0;
}

void StopIfFault(DualG2HighPowerMotorShield18v22 &md, int keeprun){
  if(keeprun == 0){
    md.disableDrivers();
  delay(1);
    Serial.println("Manual user termination");
    while (1);
  }
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
