#include "double_motor.hpp"

Motors::Motors() 
    : base(2, 4),
      pan(16, 17),
      base_enc(5, 18),
      pan_enc(), //give pins
      command("1")
{
    //set encoder to some value
}

//checks input, if input is 1,2,3, or 4, it goes to one of the predetermined locations, otherwise it
//changes the input into two numbers to go to
void Motors::moveMotors(String input) {
    command = input;    //saves the initial input to command
    while(1) {
        if (command == "1") {
            while(base_enc.read() != CAM1Position) {
                goshortestWay(CAM1Position, base_enc, base);
            }
        }
        else if (command == "2") {
            while(base_enc.read() != CAM2Position) {
                goshortestWay(CAM2Position, base_enc, base);
            }
        }    
        else if (command == "3") {
            while(base_enc.read() != CAM3Position) {
                goshortestWay(CAM3Position, base_enc, base);
            }
        }    
        else if (command == "4") {
            while(base_enc.read() != CAM3Position) {
                goshortestWay(CAM4Position, base_enc, base);
            }
        }
        else {
            decipherInput(command); //decipher input into two numbers
            //this will change desired_base and desired_pan
            goshortestWay(desired_base, base_enc, base);    //goes shortest way for base
            goshortestWay(desired_pan, pan_enc, pan);       //goes shortest way for pan
            //add an iterupt that checks the input
            //if there's something at the input then command = new input
            //if there'es nothing at the input then this just keeps looping
        }
    }
}


//make a function that goes to the desired input
//have a variable that is pan_desired and base_desired
//make a while loop that checks pan desired
//make a while inside that check base desired
//inside each while loop make a check serial avaialable
//interrupt goes do something else and then comes abck


//stops both motors
void Motors::stopMotors() {
    base.stop_motor();
    pan.stop_motor();
}


//determines wether to go right or left, up or down given the target and the encoder of the motor 
//we wish to move
int Motors::shortestWay(int target, Encoder &enc) {
    long pos = enc.read();  //gets the current position of the motor
    pos = pos % ENCODERMAX; 
    //do some mod math
    if (target > pos) { 
        return 1;
    }
    else if (target < pos) {
        return 0;
    }
    else {
        return 2;   //this means target = pos
    }
}

//give target encoder position, the encoder of the motor we wish to move, and the motor
void Motors::goshortestWay(int target, Encoder &enc, DC_Motor &motor) {
    int direction = shortestWay(target, enc);
    if (direction == 2) {
        base.stop_motor();  //if we're at target stop
    }
    motor.run_motor(direction, 100);    //else move in the direction of the target
}

void Motors::decipherInput(String input) {
    //parse the input into two numbers
    //desired_base
    //desired_pan
}
