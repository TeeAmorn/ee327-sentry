#include "testing.hpp"

int shortestWay(int target, ESP32Encoder &enc) {
    long pos = enc.getCount();  //gets the current position of the motor
    pos = pos% 445;
    if (pos > target+445/2) {
        pos = pos - 445;
    }
    else if (pos < target-445/2) {
        pos = pos + 445;
    }
    if ((target > pos) && (target-445/2 < pos)) { 
        return 1;
    }
    else if ((target+445/2 > pos) && (target < pos)) {
        return 0;
    }
    //return 2;   //this means target = pos
    return 0;
}

int decipherInput(String input) {
    int base_length = 0;    //length of first number
    int pan_length = 0;     //length of second number
    int desired_base = 0;
    int desired_pan = 0;
    for (int i = 0; input[i] != ','; i++) {
        base_length++;  //counts how long first number is
    }
    for (int i = base_length + 1; input[i] != '\0'; i++) {
        pan_length++;   //counts how second number is
    }
    for (int i = 0; i < base_length; i++) {
        desired_base = (input[i] - 48) * pow(10, base_length - i - 1) + desired_base;
    }
    for (int i = base_length + 1; i < input.length(); i++) {
        desired_pan = (input[i] - 48) * pow(10, pan_length - i + base_length) + desired_pan;
    }
    return desired_pan;
}

int decipherInput2(String input) {
    int base_length = 0;    //length of first number
    int pan_length = 0;     //length of second number
    int desired_base = 0;
    int desired_pan = 0;
    for (int i = 0; input[i] != ','; i++) {
        base_length++;  //counts how long first number is
    }
    for (int i = base_length + 1; input[i] != '\0'; i++) {
        pan_length++;   //counts how second number is
    }

    //for base
    int temp = 0;
    if (input[0] == 45) {
        for (int i = 1; i < base_length; i++) {
            temp = (input[i] - 48) * pow(10, base_length - i - 1) + temp;
        }
        desired_base = desired_base - temp;
    }
    else {
        for (int i = 0; i < base_length; i++) {
            temp = (input[i] - 48) * pow(10, base_length - i - 1) + temp;
        }
        desired_base = desired_base + temp;
    }

    //for pan
    temp = 0;
    if (input[base_length+1] == 45) {
            for (int i = base_length + 2; i < input.length(); i++) {
                temp = (input[i] - 48) * pow(10, pan_length - i + base_length) + temp;
        }
        desired_pan = desired_pan - temp;
    }
    else {
        for (int i = base_length + 1; i < input.length(); i++) {
            temp = (input[i] - 48) * pow(10, pan_length - i + base_length) + temp;
        }
        desired_pan = desired_pan + temp;
    }
    return desired_pan;
}