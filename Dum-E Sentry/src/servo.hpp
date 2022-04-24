#pragma once

#include <ESP32Servo.h>

//#define basePin 15  //change to desired pin
//#define panPin 2    //change

void servoInitialize(Servo &base, Servo &pan);
void servoMove();

struct Servos
{
    //Initializes the servos given pins
    Servos();
    int basePin;
    int panPin;
    int basePos;
    int panPos;
    Servo base;
    Servo pan;

    void moveServos(String direction);

};