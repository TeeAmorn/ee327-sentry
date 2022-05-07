#include <Arduino.h>
#include "DC_Motor.h"
#include <Encoder.h>
#include "testing.hpp"
//#include "double_motor.hpp"

// Recommended PWM GPIO pins on the ESP32 are: 2,4, 12-19, 21,23, 25-27, 32-33
//DC_Motor Pan(2,4);
DC_Motor motor2(2,0);
//Motors motors;
Encoder encoder(17, 5);
//gonna use pins 2,4 for base, 16,17 for pan
// 5, 18 for base encoder
//for pan encoder
void setup() {
  Serial.begin(9600);
  //motor2.stop_motor();
  //delay(5000);
}

void loop() {
  //motor2.run_motor(1, 50);
  Serial.print("Position: ");
  Serial.print((encoder.read() % 445));
  Serial.print(", Direction: ");
  Serial.println(shortestWay(20, encoder));
  //Serial.println(encoder.read());
  //delay(3000);
  //motor2.run_motor(0, 50);
  //delay(3000);
}


//test what direction is left and which one is right
//same with up and down
//CCW is positive encoder
//CW is negative encoder