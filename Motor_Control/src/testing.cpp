#include "testing.hpp"

int shortestWay(int target, Encoder &enc) {
    long pos = enc.read();  //gets the current position of the motor
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
    return 2;   //this means target = pos
}