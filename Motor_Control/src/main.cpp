#include <Arduino.h>
#include "DC_Motor.h"
#include "testing.hpp"
#include <ESP32Encoder.h>
#include "double_motor.hpp"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"


// Recommended PWM GPIO pins on the ESP32 are: 2,4, 12-19, 21,23, 25-27, 32-33
Motors motors;
//ESP32Encoder base_enc;
//DC_Motor motor2(2,4);

//task for parallel processing of moving the motors
void moveMotorsTask(void * parameters) {
    //ESP32Encoder::useInternalWeakPullResistors=UP;
    while(1) {
        TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
        TIMERG0.wdt_feed=1;
        TIMERG0.wdt_wprotect=0;
        if (motors.new_command == "1") {        //north
            while(motors.absDistanceToPosn(motors.base_enc, NORTH) > 7) {
                motors.goshortestWay(NORTH, motors.base_enc, motors.base);
            }
            motors.base.motor_speed_zero(); //stops motor
            motors.desired_base = NORTH;    //updates desired base to north
        }
        else if (motors.new_command == "2") {   //east
            while(motors.absDistanceToPosn(motors.base_enc, EAST) > 7) {
                motors.goshortestWay(EAST, motors.base_enc, motors.base);
            }
            motors.base.motor_speed_zero(); //stops motor
            motors.desired_base = EAST;     //updates desired base to east
        }    
        else if (motors.new_command == "3") {   //south
            while(motors.absDistanceToPosn(motors.base_enc, SOUTH) > 7) {
                motors.goshortestWay(SOUTH, motors.base_enc, motors.base);
            }
            motors.base.motor_speed_zero(); //stops motor
            motors.desired_base = SOUTH;    //updates desired base to east
        }    
        else if (motors.new_command == "4") {   //west
            while(motors.absDistanceToPosn(motors.base_enc, WEST) > 7) {
                motors.goshortestWay(WEST, motors.base_enc, motors.base);
            }
            motors.base.motor_speed_zero(); //stops motor
            motors.desired_base = WEST;     //updates desired base to west
        }
        else if (motors.new_command != motors.old_command) {    //if we received new command
            motors.decipherInput(motors.new_command);       //decipher input into two numbers
            motors.old_command = motors.new_command;    //stores new command into old
        }
        ///////base/////////
        if (motors.absDistanceToPosn(motors.base_enc, motors.desired_base) < 10) {   //if within a certain threshold
            motors.base.motor_speed_zero();   //stop the base
        }
        else {
            motors.goshortestWay(motors.desired_base, motors.base_enc, motors.base);    //goes shortest way for base
        }

        ///////pan///////////
        if (motors.absDistanceToPosn(motors.pan_enc, motors.desired_base) < 10) { //if within a certain threshold
            motors.pan.motor_speed_zero();    //stop the pan
        }
        else {
            motors.goshortestWay(motors.desired_pan, motors.pan_enc, motors.pan);   //goes shortest way for pan
        }
    }
};

void setup() {
    Serial.begin(9600);   //serial for debugging
    
    //ESP32Encoder::useInternalWeakPullResistors=UP;
    //base_enc.attachFullQuad(16,17);
    
    //motors.moveMotors("0,0");
    

    xTaskCreate(
        moveMotorsTask,  //function to imnplement the task
        "moveMotors",           //name of the task
        4096,                   //stack size
        NULL,                   //task input parameter
        0,                      //priority of the task
        NULL                   //task handle
    );    
}
//slowest motor speed is 97
//fastest is 1

void loop() {
    /*
    Serial.print("Position: ");
    Serial.print(base_enc.getCount() % 445);
    Serial.print(", Direction: ");
    Serial.println(shortestWay(40, base_enc));
    */
    //check if data is available
    Serial.print("Encoder position: ");
    Serial.print(motors.base_enc.getCount());
    Serial.print(",  Distance to Posn: ");
    Serial.println(motors.absDistanceToPosn(motors.base_enc, motors.desired_base));

    if(Serial.available() > 0) {
        motors.new_command = Serial.readString();
    }
    
    //make sure to make this %lld so it doesnt overflow

}

//CCW is positive encoder and 0 direction
//CW is negative encoder and 1 direction