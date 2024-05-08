bool NAUSetup(Adafruit_NAU7802 &nau){
  if (!nau.begin()) {
    return true;
    } else {
      return false;
    }
}

float NAUSensorValues(Adafruit_NAU7802 &nau, float strain){
  float new_reading = nau.read();
  float strain_error = abs(strain - new_reading);
  float str_err2 = abs(strain + new_reading);

  if (strain < str_err2) {
    //Serial.println("Strain Overload");
  }
  return new_reading;
}

void NAUPrintValues(float val, float base){
  int rounded_val = 100*(int)(int(val-base)/100);
  Serial.print("Strain: ");
  Serial.println(rounded_val);
}
