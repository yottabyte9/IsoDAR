bool MPUSetup(Adafruit_MPU6050 &mpu){
  if (!mpu.begin()) {
    return true;
    } else {
      return false;
    }
}

MPUSensorValues MPUReadValues(Adafruit_MPU6050 &mpu) {
    MPUSensorValues vals;
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    vals.accelX = a.acceleration.x;
    vals.accelY = a.acceleration.y;
    vals.accelZ = a.acceleration.z;
    vals.gyroX = g.gyro.x;
    vals.gyroY = g.gyro.y;
    vals.gyroZ = g.gyro.z;

    delay(100);
    return vals;
}

void MPUPrintValues(MPUSensorValues vals){
  Serial.println("");
  Serial.println("");
  Serial.println("Acceleration \t \t \tRotation");
  Serial.println("X: \tY: \tZ: \t\tX: \tY: \tZ:");
  Serial.print(vals.accelX);
  Serial.print("\t");
  Serial.print(vals.accelY);
  Serial.print("\t");
  Serial.print(vals.accelZ);

  Serial.print("\t \t");
  Serial.print(vals.gyroX);
  Serial.print("\t");
  Serial.print(vals.gyroY);
  Serial.print("\t");
  Serial.print(vals.gyroZ);
  Serial.println("");
}
