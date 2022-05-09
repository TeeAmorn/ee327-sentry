#include <Arduino.h>
#include "servo.hpp"
//#include <ESP32Servo.h> // Not the same as Arduino Servo library

// Recommended PWM GPIO pins on the ESP32 are: 2,4, 12-19, 21,23, 25-27, 32-33

Servos servos;

int baud_rate = 115200; //desired baud rate

void setup() {
  Serial.begin(baud_rate);
  Serial.println();
}

void loop() {
  
  // Code to control servo using console input
  Serial.println("Input servo direction");
  while (Serial.available() == 0){
    // If there is no input to console, don't do anything
  }
  String input = Serial.readString(); //Read the console input
  servos.moveServos(input);
  //Serial.print(input);
  Serial.print("basePos: ");
  Serial.println(servos.basePos);
}