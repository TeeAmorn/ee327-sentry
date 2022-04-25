#include "servo.hpp"

Servos::Servos()
    : basePin {15},
      panPin {2},
      basePos {90},
      panPos {180}
{ 
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

void Servos::moveServos(String direction) {
    if (direction == "down") {
        panPos += 5;
        if (panPos > 180) {
            panPos = 180;
        }
    }
    else if (direction == "up") {
        panPos -= 5;
        if (panPos < 90) {
            panPos = 90;
        }
    }
    else if (direction == "left") {
        basePos += 10;
        if (basePos > 180) {
            basePos = 180;
        }
    }
    else if (direction == "right") {
        basePos -= 10;
        if (basePos < 0) {
            basePos = 0;
        }
    }
    base.write(basePos);
    pan.write(panPos);
} 