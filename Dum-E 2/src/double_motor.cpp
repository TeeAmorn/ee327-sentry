#include "double_motor.hpp"


Motors::Motors() 
    : base(2, 4),
      pan(12, 13),  //change these pins
      old_command("1"),
      new_command("1")
{
    ESP32Encoder::useInternalWeakPullResistors=UP;

    base_enc.attachFullQuad(16,17);
    pan_enc.attachFullQuad(18,19); //change these pins
    //set encoder to some value

}

//checks input, if input is 1,2,3, or 4, it goes to one of the predetermined locations, otherwise it
//changes the input into two numbers to go to
//maybe use a hall sensor for the motors to know where they are
//make it move until it is home and call it 0
//make move motors set the target
/*
void Motors::moveMotors(String input) {
    ESP32Encoder::useInternalWeakPullResistors=UP;
    command = input;    //saves the initial input to command
    if (command == "1") {
        while(base_enc.getCount() != CAM1Position) {
            goshortestWay(CAM1Position, base_enc, base);
        }
    }
    else if (command == "2") {
        while(base_enc.getCount() != CAM2Position) {
            goshortestWay(CAM2Position, base_enc, base);
        }
    }    
    else if (command == "3") {
        while(base_enc.getCount() != CAM3Position) {
            goshortestWay(CAM3Position, base_enc, base);
        }
    }    
    else if (command == "4") {
        while(base_enc.getCount() != CAM3Position) {
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
*/

void Motors::stopMotors() {
    base.stop_motor();
    pan.stop_motor();
}

int Motors::shortestWay(int target, ESP32Encoder &enc) {
    long pos = enc.getCount();  //gets the current position of the motor
    pos = pos% ENCODERMAX;
    if (pos > target+ENCODERMAX/2) {
        pos = pos - 445;
    }
    else if (pos < target-ENCODERMAX/2) {
        pos = pos + 445;
    }
    
    if ((target > pos) && (target-ENCODERMAX/2 < pos)) { 
        return 0;
        //return 1;
    }
    else if ((target+ENCODERMAX/2 > pos) && (target < pos)) {
        return 1;
        //return 0;
    }
    //return 2;   //this means target = pos
    return 0;
}

void Motors::goshortestWay(int target, ESP32Encoder &enc, DC_Motor &motor) {
    int direction = shortestWay(target, enc);
    if (direction == 2) {
        base.stop_motor();  //if we're at target stop
    }
    motor.run_motor(direction, 40);    //else move in the direction of the target
}

//arduinoJSON

void Motors::decipherInput(String input) {
    int base_length = 0;    //length of first number
    int pan_length = 0;     //length of second number
    for (int i = 0; input[i] != ','; i++) {
        base_length++;  //counts how long first number is
    }
    for (int i = base_length + 1; input[i] != '\0'; i++) {
        pan_length++;   //counts how second number is
    }

    int temp = 0;
    //for base
    if (input[0] == 45) {
        for (int i = 1; i < base_length; i++) {
            temp = (input[i] - 48) * pow(10, base_length - i - 1) + temp;
        }
        desired_base = base_enc.getCount() - temp;
    }
    else {
        for (int i = 0; i < base_length; i++) {
            temp = (input[i] - 48) * pow(10, base_length - i - 1) + temp;
        }
        desired_base = base_enc.getCount() + temp;
    }

    temp = 0;
    //for pan
    if (input[base_length+1] == 45) {
        int temp = 0;
            for (int i = base_length + 2; i < input.length(); i++) {
                temp = (input[i] - 48) * pow(10, pan_length - i + base_length) + temp;
        }
        desired_pan = pan_enc.getCount() - temp;
    }
    else {
        for (int i = base_length + 1; i < input.length(); i++) {
            temp = (input[i] - 48) * pow(10, pan_length - i + base_length) + temp;
        }
        desired_pan = pan_enc.getCount() + temp;
    }
}

int Motors::absDistanceToPosn(ESP32Encoder &enc, int target) {
    long pos = enc.getCount();
    pos = pos % ENCODERMAX;
    target = target % ENCODERMAX;
    if (target < 0) {
        target = ENCODERMAX + target;   //make target posive
    }
    if (pos < 0) {
        pos = ENCODERMAX + pos;  //make pos positive
    }
    int temp = abs(pos - target);

    if (temp > 180) {
        return ENCODERMAX - temp;
    }
    return temp;
}