#pragma once

#include <ESP32Servo.h>

#define basePin 15  //change to desired basePin
#define panPin 2    //changed to desired panPin

void servoInitialize(Servo &base, Servo &pan);
