bool NAUSetup(Adafruit_NAU7802 &nau){
  if (!nau.begin()) {
    return true;
    } else {
      return false;
    }
}

double NAUSensorValueBase(Adafruit_NAU7802 &nau){ 
  double new_reading = nau.read();
  /*
  double strain_error = abs(strain - new_reading);
  double str_err2 = abs(strain + new_reading);

  if (strain < str_err2) {
    Serial.println("Strain Overload");
  }
  */
  return new_reading;
}

double NAUPrintValuesAdjusted(double val, double base){
  //int rounded_val = 100*(int)(int(val-base)/100);
  Serial.print("Strain: ");
  //Serial.println(rounded_val);
  Serial.println(val-base);
  return (val-base);
}
