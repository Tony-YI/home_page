#ifndef PORT_DEFINATION_H
#define PORT_DEFINATION_H
#include "mbed.h"



extern Serial pc;
extern Serial Xbee;//Serial for Xbee, which send message to the quadcopter.
extern Serial gps1;
extern Serial gps2;
//extern Serial gps3;

extern DigitalOut led1;//LED for rx
extern DigitalOut led2;//LED on when get a valid message from GPS
extern DigitalOut led3;//LED on when get a 3D-Fix
extern DigitalOut led4;//LED for Xbee rx;

extern void init_port();
#endif