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

#ifndef _BUZZER_H
#define _BUZZER_H

#include "define.h"
#include "mbed.h"

#define BUZZER_TARGET_NOT_FOUND 0x01
#define BUZZER_TAKE_PHOTO 0x02

#define TIME_OUT 20 //8 seconds

class Buzzer
{
public:
    Buzzer(MyDigitalOut* buzzer);
    ~Buzzer();
    void ON();
    void OFF();
    void setFlag();
    void cleanFlag();
    
    void check_time_out();
    void time_out_init();
    
    void notice(uint8_t type);
    void target_not_found();
    void take_photo();
private:
    MyDigitalOut* _buzzer; 
    uint8_t flag;
    Timeout time_out;
    
    void boot();
};

#endif