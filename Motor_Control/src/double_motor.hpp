#pragma once

#include "DC_Motor.h"
#include <Encoder.h>

struct Motors
{

    Motors();
    //int base1, base2, pan1, pan2;
    DC_Motor base, pan;
    Encoder enc;
    int new_dir;
    //dir == 1 means right, up
    //dir == 0 means left, down
    //dir == 2 means stop



    void moveMotors(String dir);
    void stopMotors();
    void moveOppositeDir(DC_Motor &, int &, int speed);
    int shortestWay(int position);
    void goshortestWay(int position);
};