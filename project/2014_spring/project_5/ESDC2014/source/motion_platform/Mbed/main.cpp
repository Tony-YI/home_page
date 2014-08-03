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
#include "communication.h"
#include "port.h"

int main()
{
    init_PORT();
   
    while(1)
    {
        buzzer.time_out_init();
        buzzer.OFF();
        com.parseMessage();
        if(com.getInfoOK(0) == 1) //car
        {
            printf("main(). Car action starting...\r\n");
            com.forwardMessage();
            com.ACK(&lifter, &camera_platform);
            com.resetInfoOK(0);
            com.resetInfoOK(1);
            printf("main(). Car action ended...\r\n");
        }
        else if(com.getInfoOK(0) == 2) //lifter
        {
            printf("main(). Lifter action starting...\r\n");
            lifter.lifterMove(com.getMoveDis(), com.getMoveDir(), com.getRotateDis(), com.getRotateDir());
            com.ACK(&lifter, &camera_platform);
            com.resetInfoOK(0);
            com.resetInfoOK(1);
            printf("main(). Lifter action ended...\r\n");
        }
        else if(com.getInfoOK(0) == 3) //camera_platform
        {
            printf("main(). camera_platform action starting...\r\n");
            camera_platform.cameraPlatformMove(com.getMoveDis(), com.getMoveDir(), com.getRotateDis(), com.getRotateDir());
            com.ACK(&lifter, &camera_platform);
            com.resetInfoOK(0);
            com.resetInfoOK(1);
            printf("main(). Camera_platform action ended...\r\n");
        }
        else if(com.getInfoOK(0) == 4) //compass
        {
            printf("main(). Compass action starting...\r\n");
            com.ACK(&lifter, &camera_platform);
            com.resetInfoOK(0);
            com.resetInfoOK(1);
            printf("main(). Compass action ended...\r\n");
        }
        else if(com.getInfoOK(0) == 5) //buzzer
        {
            printf("main(). Buzzer action starting...\r\n");
            buzzer.notice(com.buzzer_type);
            com.ACK(&lifter, &camera_platform);
            com.resetInfoOK(0);
            com.resetInfoOK(1);
            printf("main(). Buzzer action ended...\r\n");
        }
        
        buzzer.cleanFlag();
    }
}




