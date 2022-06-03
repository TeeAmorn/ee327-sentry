/* DC motor library for Arduino
created by Ashutosh M Bhatt (+91-9998476150) - Gujarat (INDIA) 
ver 1.0 - 12/10/2016

this library is used to control DC motors
it can control its speed from 0 to 100% and its direction
it can rotate motor forward and reverse at set speed 
it can start or stop the motor as well as provides DC BREAK for instant STOP 
just one has to select arduino pins for motor,then then start rotating motor using given library functions
*/
#ifndef DC_Motor_h
#define DC_Motor_h

#include "Arduino.h"
#include <analogWrite.h>

class DC_Motor
  { 
    private:
            int pin_1,pin_2,motor_speed,dir_flag;
    public:
    DC_Motor(int pin1,int pin2);
    DC_Motor(int pin1,int pin2,int speed_flag);
    void forward(void);
    void forward_with_set_speed(void);
    void reverse(void);
    void reverse_with_set_speed(void);
    void run_motor(int dir,int speed);
    void set_speed(int speed);  
    void start_motor(int dir);
    void jogg_full_speed(int dir);
    void stop_motor(void);
    void dc_break(void); 
    void motor_speed_zero(void); 
    void soft_start(int dir,int speed,int time_int_sec);
    void smooth_stop(int time_int_sec);
    void jogg_set_speed(int dir,int speed);
  };
  
#endif
