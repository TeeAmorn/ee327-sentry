#include "servo.hpp"

Servos::Servos()
    : basePin {4},
      panPin {18},
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
    decipherInput(direction);

    if (desired_pan > 0) {
        panPos += 4;
        if (panPos > 180) {
            panPos = 180;
        }
    }
    else if (desired_pan < 0) {
        panPos -= 4;
        if (panPos < 90) {
            panPos = 90;
        }
    }
    if (desired_base < 0) {
        basePos += 8;
        if (basePos > 180) {
            basePos = 180;
        }
    }
    else if (desired_base > 0) {
        basePos -= 8;
        if (basePos < 0) {
            basePos = 0;
        }
    }
    base.write(basePos);
    pan.write(panPos);
    delay(10);
} 

void Servos::decipherInput(String input) {
    int base_length = 0;    //length of first number
    int pan_length = 0;     //length of second number
    for (int i = 0; input[i] != ','; i++) {
        base_length++;  //counts how long first number is
    }
    for (int i = base_length + 1; input[i] != '\0'; i++) {
        pan_length++;   //counts how second number is
    }

    int temp = 0;
    //for base
    if (input[0] == 45) {
        for (int i = 1; i < base_length; i++) {
            temp = (input[i] - 48) * pow(10, base_length - i - 1) + temp;
        }
        desired_base = 0 - temp;
    }
    else {
        for (int i = 0; i < base_length; i++) {
            temp = (input[i] - 48) * pow(10, base_length - i - 1) + temp;
        }
        desired_base = temp;
    }

    temp = 0;
    //for pan
    if (input[base_length+1] == 45) {
        int temp = 0;
            for (int i = base_length + 2; i < input.length(); i++) {
                temp = (input[i] - 48) * pow(10, pan_length - i + base_length) + temp;
        }
        desired_pan = 0 - temp;
    }
    else {
        for (int i = base_length + 1; i < input.length(); i++) {
            temp = (input[i] - 48) * pow(10, pan_length - i + base_length) + temp;
        }
        desired_pan = temp;
    }
    Serial.print("desired_base: ");
    Serial.println(desired_base);
    Serial.print("desired_pan: ");
    Serial.println(desired_pan);
}