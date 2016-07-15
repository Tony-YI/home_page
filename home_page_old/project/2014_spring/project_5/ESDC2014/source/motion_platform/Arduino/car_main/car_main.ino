/******************************************************

*** ┏┓          ┏┓
**┏┛┻━━━━━━┛┻┓
**┃                 ┃
**┃      ━━━      ┃
**┃  ┳┛       ┗┳ ┃
**┃                 ┃
**┃ '''   ┻   '''  ┃
**┃                 ┃
**┗━━━┓   ┏━━━┛
******   ┃   ┃
******   ┃   ┃
******   ┃   ┃
******   ┃   ┗━━━━━━━━━━━━┓
******   ┃                         ┃━━┓
******   ┃         NO BUG          ┏━━┛
******   ┃                         ┃
******   ┗━┓  ┓  ┏━━━━┏━━┓━┛
********    ┃  ┛  ┛    ┃  ┛  ┛
********    ┃  ┃  ┃    ┃  ┃  ┃
********    ┗━┛━┛     ┗━┛━┛

This part is added by project ESDC2014 of CUHK team.
All the code with this header are under GPL open source license.
******************************************************/
#include <PinChangeInt.h>
#include <PinChangeIntConfig.h>
#include <EEPROM.h>
#include <fuzzy_table.h>
#include <PID_Beta6.h>
#include <MotorWheel.h>
#include <Omni4WD.h>

#include <communication.h>

/*

            \                    /
   wheel1   \                    /   wheel4
   Left     \                    /   Right
    
    
                              power switch
    
            /                    \
   wheel2   /                    \   wheel3
   Right    /                    \   Left

 */

/*
irqISR(irq1,isr1);
MotorWheel wheel1(5,4,12,13,&irq1);

irqISR(irq2,isr2);
MotorWheel wheel2(6,7,14,15,&irq2);

irqISR(irq3,isr3);
MotorWheel wheel3(9,8,16,17,&irq3);

irqISR(irq4,isr4);
MotorWheel wheel4(10,11,18,19,&irq4);
 */

irqISR(irq1,isr1);
MotorWheel wheel1(3,2,4,5,&irq1);

irqISR(irq2,isr2);
MotorWheel wheel2(11,12,14,15,&irq2);

irqISR(irq3,isr3);
MotorWheel wheel3(9,8,16,17,&irq3);

irqISR(irq4,isr4);
MotorWheel wheel4(10,7,18,19,&irq4);


Omni4WD Omni(&wheel1,&wheel2,&wheel3,&wheel4);
Car car(&Omni);
Communication com;

void setup()
{
  //TCCR0B=TCCR0B&0xf8|0x01;    // warning!! it will change millis()
  TCCR1B=TCCR1B&0xf8|0x01;    // Pin9,Pin10 PWM 31250Hz
  TCCR2B=TCCR2B&0xf8|0x01;    // Pin3,Pin11 PWM 31250Hz
    
  //Omni.PIDEnable(0.31,0.01,0,10);
  Omni.PIDEnable(0.26,0.01,0,10);
  Serial.begin(9600);
}

void loop()
{
  com.parseMessage();
  
  switch(com.getInfoOK())
  {
    case 1: //car motion
    car.carMove(com.getMoveDis(), com.getMoveDir(), com.getRotateDis(), com.getRotateDir());
    com.resetInfoOK();
    com.ACK();
    break;
    
    default:
    Omni.setCarStop();
    break;
  }
}

void serialEvent() //called between loops
{
  while(Serial.available())
  {
    char ch = Serial.read();
    com.putToBuffer(ch);
  }
}
