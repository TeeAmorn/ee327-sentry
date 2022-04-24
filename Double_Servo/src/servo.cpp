#include "servo.hpp"

void servoInitialize(Servo &base, Servo &pan) {
    // Allow allocation of all timers
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

    base.setPeriodHertz(50); // Define frequency of servo (50 Hz is standard)
    pan.setPeriodHertz(50);  
    //myServo.attach(servoPin, servoMin, servoMax); // Attach servo object to servo pin, define min and max
    base.attach(basePin); //
    pan.attach(panPin);
}


