#include "Port_Defination.h"

Serial pc(USBTX,USBRX);
Serial Xbee(p28,p27);

Serial gps1(p9,p10);
Serial gps2(p13,p14);
//Serial gps3(p13,p14);

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);

void init_port()
{
    pc.baud(9600);//pc.baud(115200);
    gps1.baud(38400);//GPS baud rate 38400
    gps2.baud(38400);
  //  gps3.baud(38400);
    Xbee.baud(57600);//Xbee baud rate as 57600.
}
