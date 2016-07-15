/******************************************************

****┏┓          ┏┓
**┏┛┻━━━━━━┛┻┓
**┃                 ┃
**┃      ━━━      ┃
**┃  ┳┛       ┗┳ ┃
**┃                 ┃
**┃ '''    ┻   ''' ┃
**┃                 ┃
**┗━━┓       ┏━━┛
*******┃       ┃
*******┃       ┃
*******┃       ┃
*******┃       ┗━━━━━━━━┓
*******┃                      ┃━┓
*******┃      NO BUG          ┏━┛
*******┃                      ┃
*******┗━┓  ┓  ┏━┏━┓  ━┛
***********┃  ┛  ┛    ┃  ┛  ┛
***********┃  ┃  ┃    ┃  ┃  ┃
***********┗━┛━┛     ┗━┛━┛

This part is added by project ESDC2014 of CUHK team.
All the code with this header are under GPL open source license.
This program is running on Mbed Platform 'mbed LPC1768' avaliable in 'http://mbed.org'.
**********************************************************/
#include "mbed.h"
#include "define.h"

#ifndef _LIFTER_H
#define _LIFTER_H

/*
DigitalOut lifter_enable(p23);
PwmOut lifter_pwmUp(p21);
PwmOut lifter_pwmDown(p22);
DigitalIn lifter_encoder_A(p26);
DigitalIn lifter_encoder_B(p25);
*/

//100 pulse => 8 mm
#define RPMM 13 //pulseCount per mm

class Lifter
{
public:
    Lifter(MyDigitalOut* _enable, MyPwmOut* _pwmUp, MyPwmOut* _pwmDown, MyInterruptIn* encoder_A, MyDigitalIn* encoder_B);
    ~Lifter();
    void lifterUp(uint16_t mm);
    void lifterDown(uint16_t mm);
    void lifterMove(uint16_t move_dis, uint8_t move_dir, uint16_t rotate_dis, uint8_t rotate_dir);
    uint8_t getDir();
    uint8_t isStopped();
    
    uint32_t pulseCount;
    uint32_t targetPulseCount;
    
    void setLifterStop();
    void setLifterUp();
    void setLifterDown();
    
private:
    MyDigitalOut* _enable;
    MyPwmOut* _pwmUp;
    MyPwmOut* _pwmDown;
    MyInterruptIn* _encoder_A; //6 pulse per round
    MyDigitalIn* _encoder_B; //5v is up, 0v is down. looks like no use
    uint8_t _dir;
    uint8_t _stop; //0 means moving, 1 means stopped
};

#endif