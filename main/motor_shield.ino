void MotorSetup(){
  SabertoothTXPinSerial.begin(9600); // 9600 is the default baud rate for Sabertooth packet serial.
  ST.autobaud(); // Send the autobaud command to the Sabertooth controller(s).
}

void MotorSetLevel(double ytilt){
  double tolerance = 0.1;
  double max_tilt = 5; //error catching
  double speed = 20;  //slows motor operations to allow more precise checking?
  
  ST.stop();        // Check yo'self before it becomes wrecked.
  delay(500);      // Wait for things to settle and allow humans to read the 6-axis values
  /*
  Serial.println("Ytilt \t \ttolerance \tmax_tilt");        // Report values for debugging
  Serial.print(abs(ytilt));
  Serial.print("\t \t");
  Serial.print(tolerance);
  Serial.print("\t \t");
  Serial.println(max_tilt);
 */

  if(abs(ytilt) < tolerance){                                   // If the tilt is acceptably small, return
    Serial.println("[----------          ----------] Y within tolerance : NO MOVE\n");          // report action for debug                                  
    return;
  }
  if( abs(ytilt) > tolerance && abs(ytilt) < max_tilt){         // If tilted clockwise, extend M1 and retract M2
    if(ytilt > 0){
      //Serial.print("Y too big : Y tilt = \t");                // report action for debug
     Serial.println("[vvvvvvvvvv          ^^^^^^^^^^] Y too big\n");
      ST.motor(1, 1*speed);
      ST.motor(2, -1*speed);
    }
    else{
      //Serial.println("Y < 0, move M1- and M2+");              // if tilted, and not clockwise, retract M1 and extend M2
//      Serial.print("Y too small: Y tilt = \t");                // report action for debug
     Serial.println("[^^^^^^^^^^          vvvvvvvvvv] Y too small\n");
      Serial.println(ytilt);
      ST.motor(1, -1*speed);
      ST.motor(2, 1*speed);
    }
  }
}

MotorValues MotorMove(double ytilt, double strain, MotorValues mvals){
  MotorValues newvals;
  double nstrain = MotorStrainCalculate(strain);
  double tolerance = 0.2;
  double max_tilt = 2;

  if (abs(ytilt) > max_tilt) {
    Serial.println("over max tilt terminate all motors");
    ST.stop();
    delay(500);
    //while (1);
  }

  double range = max_tilt - tolerance;
  double fraction = abs(abs(ytilt) - tolerance) / range;


  if (nstrain > 0) {
    if (ytilt > tolerance) {
      Serial.println("CASE 1");
      double M1target = (1+fraction)*nstrain;
      double M2target = fraction*nstrain;
      if(M1target > mvals.M1Speed){
        newvals.M1Speed = mvals.M1Speed+5;
      }
      if(M1target < mvals.M1Speed){
        newvals.M1Speed = mvals.M1Speed-5;
      }
      if(M2target > mvals.M2Speed){
        newvals.M2Speed = mvals.M2Speed+5;
      }
      if(M2target < mvals.M2Speed){
        newvals.M2Speed = mvals.M2Speed-5;
      }
      ST.motor(1, newvals.M1Speed);
      ST.motor(2, newvals.M2Speed);
      Serial.print("M1 Speed: ");
      Serial.println(newvals.M1Speed);
      Serial.print("M2 Speed: ");
      Serial.println(newvals.M2Speed);
      return newvals;
    }
    else if (ytilt < -1 * tolerance) {
      Serial.println("CASE 2");
      double M1target = fraction*nstrain;
      double M2target = (1+fraction)*nstrain;
      if(M1target > mvals.M1Speed){
        newvals.M1Speed = mvals.M1Speed+5;
      }
      if(M1target < mvals.M1Speed){
        newvals.M1Speed = mvals.M1Speed-5;
      }
      if(M2target > mvals.M2Speed){
        newvals.M2Speed = mvals.M2Speed+5;
      }
      if(M2target < mvals.M2Speed){
        newvals.M2Speed = mvals.M2Speed-5;
      }
      ST.motor(1, newvals.M1Speed);
      ST.motor(2, newvals.M2Speed);
      Serial.print("M1 Speed: ");
      Serial.println(newvals.M1Speed);
      Serial.print("M2 Speed: ");
      Serial.println(newvals.M2Speed);
      return newvals;
    }
  }
  else {
    if (ytilt > tolerance) {
      Serial.println("CASE 3");
      double M1target = fraction*nstrain;
      double M2target = (1+fraction)*nstrain;
      if(M1target > mvals.M1Speed){
        newvals.M1Speed = mvals.M1Speed+5;
      }
      if(M1target < mvals.M1Speed){
        newvals.M1Speed = mvals.M1Speed-5;
      }
      if(M2target > mvals.M2Speed){
        newvals.M2Speed = mvals.M2Speed+5;
      }
      if(M2target < mvals.M2Speed){
        newvals.M2Speed = mvals.M2Speed-5;
      }
      ST.motor(1, newvals.M1Speed);
      ST.motor(2, newvals.M2Speed);
      Serial.print("M1 Speed: ");
      Serial.println(newvals.M1Speed);
      Serial.print("M2 Speed: ");
      Serial.println(newvals.M2Speed);
      return newvals;
    }
    else if (ytilt < -1 * tolerance) {
      Serial.println("CASE 4");
      double M1target = (1+fraction)*nstrain;
      double M2target = fraction*nstrain;
      if(M1target > mvals.M1Speed){
        newvals.M1Speed = mvals.M1Speed+5;
      }
      if(M1target < mvals.M1Speed){
        newvals.M1Speed = mvals.M1Speed-5;
      }
      if(M2target > mvals.M2Speed){
        newvals.M2Speed = mvals.M2Speed+5;
      }
      if(M2target < mvals.M2Speed){
        newvals.M2Speed = mvals.M2Speed-5;
      }
      ST.motor(1, newvals.M1Speed);
      ST.motor(2, newvals.M2Speed);
      Serial.print("M1 Speed: ");
      Serial.println(newvals.M1Speed);
      Serial.print("M2 Speed: ");
      Serial.println(newvals.M2Speed);
      return newvals;
    }
  }

  //no tilt
  double M1target = nstrain;
  double M2target = nstrain;
  if(M1target > mvals.M1Speed){
    newvals.M1Speed = mvals.M1Speed+5;
  }
  if(M1target < mvals.M1Speed){
    newvals.M1Speed = mvals.M1Speed-5;
  }
  if(M2target > mvals.M2Speed){
    newvals.M2Speed = mvals.M2Speed+5;
  }
  if(M2target < mvals.M2Speed){
    newvals.M2Speed = mvals.M2Speed-5;
  }
  ST.motor(1, newvals.M1Speed);
  ST.motor(2, newvals.M2Speed);
  Serial.print("M1 Speed: ");
  Serial.println(newvals.M1Speed);
  Serial.print("M2 Speed: ");
  Serial.println(newvals.M2Speed);
  return newvals;
}

/*
   -200k (400)
   -125k -> -75k (0)
   0 (-400)
*/
double MotorStrainCalculate(double strain) {
  double max_threshold = -30000;
  double min_threshold = 0;
  double target_pressure = -20000;
  double tolerance = 7500;

  if (strain <= max_threshold) {
    return 127;
  }
  else if (strain >= min_threshold) {
    return -127;
  }
  else if (strain > max_threshold && strain < target_pressure - tolerance) { // (-200k to -125k)
    double range = abs(max_threshold - (target_pressure - tolerance)); //75k range
    double fraction = abs(strain - (target_pressure - tolerance)) / range; //distance from -125k / range
    return 127 * fraction;
  }
  else if (strain >= target_pressure - tolerance && strain <= target_pressure + tolerance) {
    return 0;
  }
  else if (strain > target_pressure + tolerance && strain < min_threshold) { // (-75k to 0)
    double range = abs(min_threshold - (target_pressure + tolerance)); // 75k range
    double fraction = abs(strain - (target_pressure + tolerance)) / range; // distance from -75k / range
    return -127 * fraction;
  }
  return 0;
}

void StopIfFault(int keeprun){
  if(keeprun == 0){
    ST.stop();
    delay(1);
    Serial.println("Manual user termination");
    while (1);
  }
  /*
  if (md.getM1Fault())
  {
    md.disableDrivers();
    delay(1);
    Serial.println("M1 fault");
    //while (1);
  }
  if (md.getM2Fault())
  {
    md.disableDrivers();
    delay(1);
    Serial.println("M2 fault");
    //while (1);
  }
  */
}
