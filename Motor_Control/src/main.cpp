#include <Arduino.h>
#include "DC_Motor.h"
#include "double_motor.hpp"

// Recommended PWM GPIO pins on the ESP32 are: 2,4, 12-19, 21,23, 25-27, 32-33
//DC_Motor Pan(2,4);
DC_Motor motor2(2,4);
Motors motors;
Encoder encoder(5, 18);
//gonna use pins 2,4 for base, 16,17 for pan
// 5, 18 for base encoder
//for pan encoder
void setup() {
  Serial.begin(9600);
  motor2.stop_motor();
  delay(5000);
}

void loop() {
  motor2.run_motor(1, 50);
  delay(3000);
  motor2.run_motor(0, 50);
  delay(3000);
}

//test to do
//check what encoder starts at
//Serial.println(encoder.read());