//#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>

Adafruit_MPU6050 mpu;
Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);
void setup() {
  // baud rate
  Serial.begin(115200);


  if (!mpu.begin(0x68)) {
    Serial.println("Failed to find MPU");
    while(1) {
      delay(10);
    }
  }
  Serial.println("MPU found");

    Serial.println("OLED test");
    delay(250); //wait for power up
    display.begin(0x3c, true); // default address

    Serial.println("OLED begun");

    display.display();
    delay(1000); // wait for splash screen

    display.clearDisplay();
    display.display();

}

void loop() {
  int counter = 0;
 // sensors_event_t a, g, t;
  //mpu.getEvent(&a, &g, &t);
 // Serial.println(g.gyro.x);
  display.clearDisplay();
  while (true) {
    display.clearDisplay();
    //Serial.println("MPU printout test");
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 10);
    display.println("Fresh data test");
    display.println("count: ");
    display.print(counter);
    //Serial.println(g.gyro.x);
    display.display();
    delay(1000);
    counter++;
  }

}
