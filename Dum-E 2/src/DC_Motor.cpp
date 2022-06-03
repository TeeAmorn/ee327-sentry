/* DC motor library for Arduino
created by Ashutosh M Bhatt (+91-9998476150) - Gujarat (INDIA) 
ver 1.0 - 12/10/2016

this library is used to control DC motors
it can control its speed from 0 to 100% and its direction
it can rotate motor forward and reverse at set speed 
it can start or stop the motor as well as provides DC BREAK for instant STOP 
it can provide soft start to motor - means the speed of motor will gradually increase from minimum to the desire level
also it can provide smooth stop to motor - means the speed of motor will gradually decrease from the current speed to minimum
just one has to select arduino pins for motor,then then start rotating motor using given library functions
*/
#include "DC_Motor.h"


DC_Motor::DC_Motor(int pin1,int pin2)   // this is a constructor
  {
     pinMode(pin1,OUTPUT);                                                  // this will create an instance of DC_Motor
     pinMode(pin2,OUTPUT);                                              // in the arduino sketch with motor driver pins
     digitalWrite(pin1,LOW);
     digitalWrite(pin2,LOW);
     pin_1 = pin1;
     pin_2 = pin2;    
  }
DC_Motor::DC_Motor(int pin1,int pin2,int speed_flag)   // this is a another constructor
  {                                                    // this will create an instance of DC_Motor
     if(speed_flag==1)                                 // in the arduino sketch with motor driver pins with analog output only
       {
        pin_1 = pin1;
        pin_2 = pin2;
       }
  } 
/////////////////////////// this function will start rotating motor in either direction //////////////////////////////  
 void DC_Motor::start_motor(int dir)                        
  {      
    if(dir==1)                                              //if direction is set to 1 it will rotate forward
      {
         digitalWrite(pin_1,HIGH);
         digitalWrite(pin_2,LOW); 
      }
    else                                                    // else for 0 it will rotate reverse
      {
         digitalWrite(pin_1,LOW);
         digitalWrite(pin_2,HIGH);  
      }
  } 
/////////////////////// this function will jerk the motor in either direction ///////////////////////////////////////  
void DC_Motor::jogg_full_speed(int dir)                        
  {      
    if(dir==1)
      {
         digitalWrite(pin_1,HIGH);
         digitalWrite(pin_2,LOW); 
         delay(5000);         
         digitalWrite(pin_1,LOW); 
      }
    else
      {
         digitalWrite(pin_1,LOW);
         digitalWrite(pin_2,HIGH); 
         delay(5000);         
         digitalWrite(pin_2,LOW);  
      }
  }     
/////////////////// this function will start rotating motor forward ///////////////////////////////////////////////////                             
 void DC_Motor::forward()                        
  {
      digitalWrite(pin_1,HIGH);
      digitalWrite(pin_2,LOW);                                               
  }
/////////////////////////////// // this function will start rotating motor reverse ////////////////////////////////////  
void DC_Motor::reverse()                        
  {
      digitalWrite(pin_1,LOW);
      digitalWrite(pin_2,HIGH);                                               
  }
//////////////////////////////////// this function will apply DC break to motor ///////////////////////////////////////  
void DC_Motor::dc_break()                        
  {
      digitalWrite(pin_1,HIGH);
      digitalWrite(pin_2,HIGH);                                               
  } 
////////////////////////////////// // this function will stop the motor //////////////////////////////////////////////  
void DC_Motor::stop_motor()                        
  {
      digitalWrite(pin_1,LOW);
      digitalWrite(pin_2,LOW);                                               
  }   
//////////////////////////////////// // this function will set motor speed between 0 - 100% /////////////////////////  
void DC_Motor::set_speed(int speed)                        
  {
      motor_speed = speed;
      motor_speed = map(motor_speed,0,100,0,255);                                             
  } 
///////////////////// this function will start rotating motor forward at set speed  ////////////////////////////////////   
void DC_Motor::forward_with_set_speed()                       
  {
      analogWrite(pin_1,motor_speed);
      analogWrite(pin_2,0);
  }
/////////////////////////  // this function will start rotating motor reverse at set speed  ////////////////////////////  
void DC_Motor::reverse_with_set_speed()                       
  {
      analogWrite(pin_2,motor_speed);
      analogWrite(pin_1,0);
  }
///////////////////////   // this function will run motor forward or reverse at set speed //////////////////////////////        
void DC_Motor::run_motor(int dir, int speed)                       
  {
     speed = map(speed,0,100,0,255);
     if(dir==1)
       {
          analogWrite(pin_1,speed);
          analogWrite(pin_2,0);
       }   
     else
       {   
           analogWrite(pin_2,speed);
           analogWrite(pin_1,0);
       } 
  } 
/////////////////////////// this function will apply jerk to motor in either direction at set speed /////////////////////  
void DC_Motor::jogg_set_speed(int dir, int speed)             
  {
     speed = map(speed,0,100,0,255);
     if(dir==1)
       {
          analogWrite(pin_1,speed);
          analogWrite(pin_2,0);
          delay(5000);
          analogWrite(pin_1,0);
       }   
     else
       {   
           analogWrite(pin_2,speed);
           analogWrite(pin_1,0);
           delay(5000);
          analogWrite(pin_2,0);
       } 
  }
//////////////////////// this function will reduce motor speed to zero   /////////////////////////////////////           
void DC_Motor::motor_speed_zero()                                                            
  {
       analogWrite(pin_1,0);  
       analogWrite(pin_2,0);
  } 
////////////////  this function will provide soft start to motor at desire speed in specified time ///////////////
void DC_Motor::soft_start(int dir,int speed,int time_in_sec)
  {
     int s,dly;
     speed = map(speed,0,100,0,255);
     motor_speed = speed;
     dly = time_in_sec*1000/motor_speed;
     if(dir==1)
       {
           dir_flag=1;
           analogWrite(pin_2,0);
           for(s=0;s<speed;s++)
             {
               analogWrite(pin_1,s); 
               delay(dly);
             }
        }
     else  
       {
           dir_flag=0;
           analogWrite(pin_1,0);
           for(s=0;s<speed;s++)
             {
               analogWrite(pin_2,s); 
               delay(dly);
             }
        }
   }                                          
///////////////////////// this function will provide smooth stop to motor in specified time
void DC_Motor::smooth_stop(int time_in_sec)
  {
     int s,d,dly;
     dly = time_in_sec*1000/motor_speed;
     if(dir_flag==1)
       {
           analogWrite(pin_2,0);
           for(s=motor_speed;s>0;s--)
             {
               analogWrite(pin_1,s); 
               delay(dly);
             }
        }
     else  
       {
           analogWrite(pin_1,0);
           for(s=motor_speed;s>0;s--)
             {
               analogWrite(pin_2,s); 
               delay(dly);
             }
        }
   }           
