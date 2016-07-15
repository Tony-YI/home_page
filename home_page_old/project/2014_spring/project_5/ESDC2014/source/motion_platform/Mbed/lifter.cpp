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
#include "lifter.h"

Lifter::Lifter(MyDigitalOut* _enable, MyPwmOut* _pwmUp, MyPwmOut* _pwmDown, MyInterruptIn* _encoder_A, MyDigitalIn* _encoder_B)
{
    pulseCount = 0;
    targetPulseCount = 0;
    _dir = 0;
    _stop = 0;
    
    this->_enable = _enable;
    this->_pwmUp = _pwmUp;
    this->_pwmDown = _pwmDown;
    this->_encoder_A = _encoder_A;
    this->_encoder_B = _encoder_B;
    
    setLifterStop();
}

Lifter::~Lifter()
{
    delete _enable;
    delete _pwmUp;
    delete _pwmDown;
    delete _encoder_A; //6 pulse per round
    delete _encoder_B; //5v is up, 0v is down. looks like no use
}

void Lifter::lifterUp(uint16_t mm)
{
    targetPulseCount = mm * RPMM;
    //targetPulseCount = mm;
    setLifterUp();
}

void Lifter::lifterDown(uint16_t mm)
{
    targetPulseCount = mm * RPMM;
    //targetPulseCount = mm;
    setLifterDown();
}

uint8_t Lifter::getDir()
{
    /*
    if(*_encoder_B == 1) //up
    {
        return true;
    }
    else //down
    {
        return false;
    }
    */
    return _dir;
}
uint8_t Lifter::isStopped()
{
    return _stop;
}

void Lifter::lifterMove(uint16_t move_dis, uint8_t move_dir, uint16_t rotate_dis, uint8_t rotate_dir)
{
    pulseCount = targetPulseCount = 0;
    _stop = 0;
    _dir = move_dir ;
    if(_dir == 0x00) //up
    {
        lifterUp(move_dis);
    }
    else if(_dir == 0x02) //down
    {
        lifterDown(move_dis);
    }
}

void Lifter::setLifterStop()
{
    *_enable = 0;
    *_pwmUp = 1.0f;
    *_pwmDown = 1.0f;
    _stop = 1;
}
void Lifter::setLifterUp()
{
    *_enable = 0;
    *_pwmUp = 1.0f;
    *_pwmDown = 0.0f;
}
void Lifter::setLifterDown()
{
    *_enable = 0;
    *_pwmUp = 0.0f;
    *_pwmDown = 1.0f;
}
