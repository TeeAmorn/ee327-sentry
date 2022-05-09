#pragma once

#include "DC_Motor.h"
#include <stdio.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
//#include <Encoder.h>
#include <ESP32Encoder.h>

using namespace std;

#define CAM1Position 0
#define CAM2Position 111
#define CAM3Position 223
#define CAM4Position 334
#define ENCODERMAX 445  //13355 ticks in 30 revolutions

struct Motors
{
    Motors();
    //int base1, base2, pan1, pan2;
    DC_Motor base, pan;
    ESP32Encoder base_enc, pan_enc;
    String command; //this is the input string to tell us where to go
    long desired_base, desired_pan;
    int new_dir;
    //dir == 1 means CCW
    //dir == 0 means CW
    //dir == 2 means stop

    void moveMotors(String dir);

    //stops both motors
    void stopMotors();

    //determines wether to go right or left, up or down given the target and the encoder of the motor 
    //we wish to move
    int shortestWay(int position, ESP32Encoder &enc);

    //give target encoder position, the encoder of the motor we wish to move, and the motor
    void goshortestWay(int position, ESP32Encoder &enc, DC_Motor &motor);

    //given a string input deciphers the input into base and pan directions in pixels
    void decipherInput(String dir);
};