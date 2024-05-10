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

double MotorStrainCalculate(double strain){
  if(strain < -50000){ //push up
    return 200;
  }
  else{
    return -200;
  }
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
