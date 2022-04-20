#include <Arduino.h>
#include <ESP32Servo.h> // Not the same as Arduino Servo library
// ESP32Servo repo w/ examples: https://github.com/madhephaestus/ESP32Servo
// I use the potetionmeter example here

// Need to attach servo to a PWM pin. 
// Possible PWM pins on ESP32:
int servoPin = 23; //Temp value
int servoPos = 0; //Servo position. Should be an angle from 0-180 degrees
// Different servos have different pulse widths that corresponds to an angle

// For SG90, 500 us is 0 degrees and 2400 is 180 degrees
// These values vary so change according to the servo
// int servoMin = 1000; 
// int servoMax = 2000;
Servo myServo; //Initialize servo object


void setup() {
  Serial.begin(9600); // Begin comms to board. For manual control of servo through console
  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myServo.setPeriodHertz(50); // Define frequency of servo (50 Hz is standard)
  //myServo.attach(servoPin, servoMin, servoMax); // Attach servo object to servo pin, define min and max
  myServo.attach(servoPin);
}

void loop() {
  
  
  
  // Code to control servo using console input
  // Serial.println("Input servo angle");
  // while (Serial.available() == 0){
  //   // If there is no input to console, don't do anything
  // }
  // servoPos=Serial.parseInt(); // Read console input, that will be servo angle
  // Serial.print("Desired angle: ");
  // Serial.println(servoPos);
  // myServo.write(servoPos);
  // delay(50);
  


  // Making the servo go through all possible angles 
  
  while(1){
    // sweep from 0 -> 180 degrees
    for (servoPos = 0; servoPos <= 180; servoPos++){
      myServo.write(servoPos);
      Serial.println(servoPos);
      //delay(50);
      }
    // Sweep back from 180 -> 0 degrees
    for (servoPos = 180; servoPos >= 0; servoPos--){
      myServo.write(servoPos);
      Serial.println(servoPos);
      //delay(50);
    }
  }
  
  
  

 /*
  // Basic demonstration of full range of servo
  myServo.write(0); // Write specified angle to the servo
  delay(2000); // Wait for servo to get there
  myServo.write(180); 
  delay(2000);
  */
}