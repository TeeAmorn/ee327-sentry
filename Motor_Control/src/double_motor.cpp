#include "double_motor.hpp"

#define CAM1Position 1
#define CAM2Position 2
#define CAM3Position 3
#define CAM4Position 4
#define ENCODERMAX 100
//^^^^ make sure to change

Motors::Motors() 
    : base(2, 4),
      pan(16, 17),
      enc(5, 18)
{
    //set encoder to some value
}

//make this code into switch
void Motors::moveMotors(String dir) {
    if (dir == "stop") {
        stopMotors();
    }
    else if (dir == "left") {
        base.run_motor(0, 100);
    }
    else if (dir == "right") {
        base.run_motor(1, 100);
    }
    else if (dir == "down") {
        pan.run_motor(0, 50);
    }
    else if (dir == "up") {
        pan.run_motor(1, 50);
    }
    else if (dir == "1") {
        goshortestWay(CAM1Position);
    }
    else if (dir == "2") {
        goshortestWay(CAM2Position);
    }    
    else if (dir == "3") {
        goshortestWay(CAM3Position);
    }    
    else if (dir == "4") {
        goshortestWay(CAM4Position);
    }
}

void Motors::stopMotors() {
    base.stop_motor();
    pan.stop_motor();
}

void Motors::moveOppositeDir(DC_Motor &motor, int &dir, int speed) {
    motor.stop_motor();
    delay(10);
    if(dir == 1) {  //if going right
        dir = 0;    //go left
    }
    else if(dir == 0) { //if going left
        dir = 1;        //go right
    }
    motor.run_motor(dir, speed);
}

int Motors::shortestWay(int target) {
    long pos = enc.read();
    pos = pos % ENCODERMAX;
    //do some mod math
    if (target > pos) {
        return 1;
    }
    else if (target < pos) {
        return 0;
    }
}

void Motors::goshortestWay(int target) {
    base.run_motor(shortestWay(target), 100);
    while ((enc.read() % ENCODERMAX) != target) {
    }
    base.stop_motor();
}