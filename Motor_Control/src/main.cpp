#include <Arduino.h>
#include "DC_Motor.h"
#include "double_motor.hpp"

//Motors motors(4, 5, 6, 7);
//String dir;
// Recommended PWM GPIO pins on the ESP32 are: 2,4, 12-19, 21,23, 25-27, 32-33
//DC_Motor Pan(2,4);
DC_Motor motor2(2,4);
Motors motors;
//gonna use pins 2,4 for base
// 16, 17 for pan
// 5, 18 for encoder
void setup() {
  motor2.stop_motor();
  delay(5000);
}

void loop() {
  motor2.soft_start(1,90,10);
  delay(3000);
  motor2.smooth_stop(5);
   motor2.soft_start(0,50,6);
  delay(3000);
  motor2.smooth_stop(3);
  //motors.moveMotors(dir);
}