#include <Arduino.h>
#include "servo.hpp"
#include <ESP32Servo.h> // Not the same as Arduino Servo library
// ESP32Servo repo w/ examples: https://github.com/madhephaestus/ESP32Servo
// I use the potetionmeter example here

// Need to attach servo to a PWM pin. 
// Possible PWM pins on ESP32:
// Recommended PWM GPIO pins on the ESP32 are: 2,4, 12-19, 21,23, 25-27, 32-33
int basePin = 15; //Pin for base servo
int panPin = 2;   //Pin for pan servo
int basePos = 90; //Servo position. Should be an angle from 0-180 degrees
int panPos = 180;
char servoDir;  //Servo direction
// Different servos have different pulse widths that corresponds to an angle

// For SG90, 500 us is 0 degrees and 2400 is 180 degrees
// These values vary so change according to the servo
// int servoMin = 1000; 
// int servoMax = 2000;
Servo base; //Initialize base servo object
Servo pan;  //Initialize pan servo object

int baud_rate = 9600; //desired baud rate


void setup() {
  servoInitialize(baud_rate);

  base.setPeriodHertz(50); // Define frequency of servo (50 Hz is standard)
  pan.setPeriodHertz(50);  
  //myServo.attach(servoPin, servoMin, servoMax); // Attach servo object to servo pin, define min and max
  base.attach(basePin); //
  pan.attach(panPin); //
  base.write(basePos);
  pan.write(panPos);
  delay(50);
}

void loop() {
  
  // Code to control servo using console input
  Serial.println("Input servo direction");
  while (Serial.available() == 0){
    // If there is no input to console, don't do anything
  }
  //servoPos=Serial.parseInt(); // Read console input, that will be servo angle
  servoDir = Serial.read(); //Read the console input
  //Serial.print("Desired angle: ");
  //Serial.println(servoPos);

  if (servoDir == 's') {
    panPos += 5;
    if (panPos > 180) {
      panPos = 180;
    }
    pan.write(panPos);
    Serial.println(panPos);
  }
  else if (servoDir == 'w') {
    panPos -= 5;
    if (panPos < 90) {
      panPos = 90;
    }
    pan.write(panPos);
    Serial.println(panPos);
  }
  else if (servoDir == 'a') {
    basePos += 10;
    if (basePos > 180) {
      basePos = 180;
    }
    base.write(basePos);
    Serial.println(basePos);
  }
  else if (servoDir == 'd') {
    basePos -= 10;
    if (basePos < 0) {
      basePos = 0;
    }
    base.write(basePos);
    Serial.println(basePos);
  }
  else if (servoDir == 'A') {
    basePos += 30;
    if (basePos > 180 ) {
      basePos = 180;
    } 
    base.write(basePos);
    Serial.println(basePos);
  }
  else if (servoDir == 'D') {
      basePos -= 30;
    if (basePos < 0 ) {
      basePos = 0;
    } 
    base.write(basePos);
    Serial.println(basePos);
  }
  delay(5);
  

  /*
  // Making the servo go through all possible angles 
  while(1){
    for (servoPos = 0; servoPos <= 180; servoPos++){
      myServo.write(servoPos);
      Serial.println(servoPos);
      delay(50);
      }
  }
  */
  

  /*
  // Basic demonstration of full range of servo
  myServo.write(0); // Write specified angle to the servo
  delay(2000); // Wait for servo to get there
  myServo.write(180); 
  delay(2000);
  */
}