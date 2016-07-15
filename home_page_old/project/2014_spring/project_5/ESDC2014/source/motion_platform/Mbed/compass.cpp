#include "compass.h"
/*
Serial pc(USBTX, USBRX);


*/

COMPASS::COMPASS(MySerial* serial)
{
   this->_serial = serial;
   
   serial->baud(56000);
   serial->format(8,SerialBase::None, 1);
   
   init();
}

uint16_t COMPASS::read()
{
    printf("before resume  \r\n");
    resume();
    printf("after resume \r\n");

    wait(0.5);
         printf("before run \r\n");

    run();
             printf("after run \r\n");

    printf("enter while buffer_count: %d \r\n",buffer_count);

    
    
    printf("enter while 1 \r\n");
    while(1)
    {
          if(buffer_count<=2)
         {
             buffer_count=0;
             break;
         }
         
         buffer_count--;
         uint8_t tempc1=buffer[buffer_count];
         buffer_count--;
         uint8_t tempc2=buffer[buffer_count];
         
                 temp[0]= tempc1;
                 temp[1]= tempc2;
                 
                 if( 0xa0==( (temp[0]) & 0xe0))
                 { 
                    flag=1;
                    printf("match !!!!!\r\n");
                 }
             
            
            
        
        
        if(flag==1)
        {
                      twobytes=(temp[0])*256+(uint8_t)(temp[1]);
            
                     digits= temp[1] & 0x0f;
            
                      tens= (temp[1]>>4) & 0x0f;
            
                      hundreds= temp[0] & 0x07;
        
                     _degree=100*hundreds+10*tens+digits;
                      printf("buffer[0]: %x\r\n",temp[0]);
                      printf("buffer[1]: %x\r\n",temp[1]);
                     temp[0]=0;
                     temp[1]=0;
                     flag=0;
           
            buffer_count=0;
            
             
             // printf("hundreds: %d\r\n",hundreds);
//              printf("tens: %d\r\n",tens);
//              printf("digits: %d\r\n",digits);
          
               resume();
               
               
               _degree+=90;
               
               if(_degree>=360)
               _degree-=360;
              
                   printf("degree: %d\r\n",_degree);
              printf("----------------------\r\n");
               return _degree;
//            break;
        }
   
    }
     // printf("leave while 1 \r\n");
//       wait(0.5);
//       
//     printf("before resume2 \r\n");
//
//     
//           printf("after resume2 \r\n");

           
         
    return 0;
}

void COMPASS::init() 
{
    buffer[0]=0;
    buffer[1]=0;
    count=0;
    flag=0;
    buffer_count=0;
    for(int i=0;i<_BUFFER_SIZE;i++)
    {
        buffer[i]=0;
    }
}

void COMPASS::run() 
{
    write2Bytes(RUN_MSB,RUN_LSB);
}

void COMPASS::stop() 
{
    write2Bytes(STOP_MSB,STOP_LSB);
}

void COMPASS::resume() 
{
    write2Bytes(RESUME_MSB,RESUME_LSB);
}

void COMPASS::reset() 
{
    write2Bytes(RST_MSB,RST_LSB);
}

void COMPASS::write2Bytes(char msb, char lsb) 
{
    _serial->putc(lsb);
    _serial->putc(msb);
    
}

void COMPASS::putToBuffer(uint8_t data) 
{
    if(buffer_count<_BUFFER_SIZE-10)
    buffer_count++;
    else
     {
     printf("Error full buffer \r\n");
     buffer_count=0;
     }
     
    buffer[buffer_count]=data;
    
}
//
//void COMPASS::check_time_out()
//{
//    if(flag == 1)
//    {
//        ON();
//        wait(5);
//        mbed_reset();
//    }
//    else
//    {
//        OFF();
//    }
//}
//
//void COMPASS::time_out_init()
//{
//    setFlag();
//    time_out.detach();
//    time_out.attach(this, &Buzzer::check_time_out, TIME_OUT);
//}
