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

#ifndef _PORT_H
#define _PORT_H

MyDigitalOut IntelToMbed_LED(LED1); //uart port LED between Intel Board and Mbed
MyDigitalOut MbedToArduino_LED(LED2); //uart port LED between Mbed and Arduino
MySerial DEBUG(USBTX, USBRX); //usb serial port between computer and Mbed
MySerial IntelToMbed(p13, p14); //uart port between Intel Board and Mbed
MySerial MbedToArduino(p28, p27); //uart port between Mbed and Arduino
MyPwmOut lifter_pwmUp(p21);
MyPwmOut lifter_pwmDown(p22);
MyDigitalOut lifter_enable(p23);
MyInterruptIn lifter_encoder_A(p18);
MyDigitalIn lifter_encoder_B(p17);

MyPwmOut camera_platform_pwmRoll(p24);//roll            min counter clockwise, max clockwise 
MyPwmOut camera_platform_pwmPitch(p26);//pitch           min down,  max up,  mid 1700
MyPwmOut camera_platform_pwmYaw(p25);//yaw             min right, max left, mid 1500

MyDigitalOut buzzer_pin(p29);
Buzzer buzzer(&buzzer_pin);


MySerial CompassData(p9, p10); ////uart port between compass and Mbed

Communication com(&DEBUG, &IntelToMbed, &MbedToArduino, &CompassData);
Lifter lifter(&lifter_enable, &lifter_pwmUp, &lifter_pwmDown, &lifter_encoder_A, &lifter_encoder_B);
Camera_platform camera_platform(&camera_platform_pwmRoll, &camera_platform_pwmPitch, &camera_platform_pwmYaw);

void IntelToMbedRxHandler()
{
    //__disable_irq();//disable interupt when receiving data from XBEE_UART
    uint8_t _x = IntelToMbed.getc();
    com.putToBuffer(_x, 0); //function inside Communication::
    //__enable_irq();
}
void MbedToArduinoRxHandler()
{
    //__disable_irq();//disable interupt when receiving data from XBEE_UART
    uint8_t _x = MbedToArduino.getc();
    com.putToBuffer(_x, 1); //function inside Communication::
    //__enable_irq();
}
void CompassDataRxHandler()
{
    //__disable_irq();
    uint8_t _x = CompassData.getc();
    com.putToBuffer(_x,2);
    //__enable_irq();
}


void LifterPulseHandler()
{
    if(lifter.pulseCount < lifter.targetPulseCount)
    {
        lifter.pulseCount++;
        if(lifter.getDir() == 0) //up
        {
            lifter.setLifterUp();
        }
        else if(lifter.getDir() == 2)//down
        {
            lifter.setLifterDown();
        }
    }
    else
    {
        lifter.targetPulseCount = 0;
        lifter.pulseCount = 0;
        lifter.setLifterStop();
    }
}

void init_PORT() //used in main() function
{
    DEBUG.baud(9600);
    printf("&CompassData %p...\r\n",&CompassData);
    IntelToMbed.baud(9600);
    IntelToMbed.attach(&IntelToMbedRxHandler); //serial interrupt function
    
    MbedToArduino.baud(9600);
    MbedToArduino.attach(&MbedToArduinoRxHandler); //serial interrupt function
    
    CompassData.baud(56000);
    CompassData.attach(&CompassDataRxHandler);
    
    lifter_encoder_A.fall(&LifterPulseHandler); //interrupt
    
    camera_platform_pwmRoll.period_ms(20); //20ms periodic, 1000us to 2000us
    camera_platform_pwmPitch.period_ms(20); //20ms periodic, 1000us to 2000us
    camera_platform_pwmYaw.period_ms(20); //20ms periodic, 1000us to 2000us
    
     camera_platform_pwmRoll.pulsewidth_us(ROLL_MID);
    camera_platform_pwmPitch.pulsewidth_us(PITCH_MID);
    camera_platform_pwmYaw.pulsewidth_us(YAW_MID);
}

#endif