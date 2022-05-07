#pragma once

#include "DC_Motor.h"
#include <Encoder.h>

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
    Encoder base_enc, pan_enc;
    String command; //this is the input string to tell us where to go
    long desired_base, desired_pan;
    int new_dir;
    
    //dir == 1 means right, up
    //dir == 0 means left, down
    //dir == 2 means stop

    void moveMotors(String dir);
    void stopMotors();
    int shortestWay(int position, Encoder &enc);
    void goshortestWay(int position, Encoder &enc, DC_Motor &motor);
    void decipherInput(String dir);
};