#include <Arduino.h>
#include "DC_Motor.h"
#include "testing.hpp"
#include <ESP32Encoder.h>
//#include "double_motor.hpp"

// Recommended PWM GPIO pins on the ESP32 are: 2,4, 12-19, 21,23, 25-27, 32-33
//DC_Motor Pan(2,4);
DC_Motor motor2(2,4);
//Motors motors;
//Encoder encoder(17, 5);
//ESP32Encoder encoder;

//gonna use pins 2,4 for base, 16,17 for pan
// 5, 18 for base encoder
//for pan encoder
void setup() {

  Serial.begin(9600);
  //ESP32Encoder::useInternalWeakPullResistors=UP;
  //encoder.attachFullQuad(18, 19);
  //motor2.smooth_stop(1);
}
//slowest motor speed is 97
//fastest is 1

void loop() {
  //String input = "-3279,-69";
  //int output = decipherInput2(input);

  //Serial.print("Position: ");
  //Serial.println(encoder.getCount());
  //Serial.println(output);
  motor2.run_motor(0, 80);
  //motor2.smooth_stop(5);
  //motor2.stop_motor();
  //delay(2000);
  //motor2.reverse();
  //delay(5000);
  //motor2.smooth_stop(5);
  //motor2.stop_motor();
  //delay(2000);
  /*
  Serial.print("Position: ");
  Serial.print((encoder.getCount() % 445));
  Serial.print(", Direction: ");
  Serial.println(shortestWay(20, encoder));
  */
   //motor2.run_motor(1, 80);

}


//test what direction is left and which one is right
//same with up and down
//CCW is positive encoder
//CW is negative encoder