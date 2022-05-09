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
    int basePin, panPin;    //pins
    int basePos, panPos;    //position of the servo
    Servo base, pan;        //declare both servos
    String command;         //string of input command
    int desired_base, desired_pan;  //the desired position of pan and base


    void moveServos(String direction);
    void decipherInput(String dir);
};