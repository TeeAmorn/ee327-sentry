#include "servo.hpp"

void servoInitialize(int baud) {
    Serial.begin(baud); // Begin comms to board. For manual control of servo through console
    // Allow allocation of all timers
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
}
