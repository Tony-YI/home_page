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

/*
The communication protocol is inside source/intel_board/lib/message.h
*/
#include "lifter.h"
#include "camera_platform.h"
#include "mbed.h"
#include "buzzer.h"

#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H



#define RUN_MSB     0xC7
#define RUN_LSB     0x10
#define ACK_RUN_MSB     0xC7
#define ACK_RUN_LSB     0x10

#define STOP_MSB    0xC6
#define STOP_LSB    0x10
#define ACK_STOP_MSB    0xD6
#define ACK_STOP_LSB    0x01

#define RESUME_MSB      0xD1
#define RESUME_LSB      0x10
#define ACK_RESUME_MSB      0xD1
#define ACK_RESUME_LSB      0x10

#define RST_MSB     0xC2
#define RST_LSB     0x10    

#define DECLINATIONANGLE  -0.0457

#define BUFFER_SIZE 1024
#define STARTER 0x7e
#define COMPASS_STARTER 0x7d
#define BUZZER_STARTER 0x7c

#define DEBUG_ON 1

class Communication
{
public:
    Communication(MySerial* _DEBUG, MySerial *_IntelToMbed, MySerial *_MbedToArduino, MySerial *CompassData);
    ~Communication();

    void putToBuffer(uint8_t _x, uint8_t communication_type); //0 is IntelToMbed, 1 is MbedTOArduino
    uint8_t getByte(uint8_t communication_type); //0 is IntelToMbed, 1 is MbedTOArduino
    uint16_t get2Bytes(uint8_t communication_type); //0 is IntelToMbed, 1 is MbedTOArduino
    void putByte(uint8_t _x, uint8_t _i); //_i = 0 is _DEBUG, _i = 1 is _IntelToMbed, _i = 2 is _MbedToArduino
    void put2Bytes(uint16_t _x, uint8_t _i);
    void parseMessage();
    void forwardMessage(); //forward the message to the Arduino
    void ACK(Lifter* lifter, Camera_platform* camera_platform); //send ACK back to the intel board

    uint8_t getInfoOK(uint8_t communication_type); //0 is IntelToMbed, 1 is MbedTOArduino
    void resetInfoOK(uint8_t communication_type); //0 is IntelToMbed, 1 is MbedTOArduino
    uint16_t getMoveDis();
    uint16_t getRotateDis();
    uint8_t getMoveDir();
    uint8_t getRotateDir();

    uint16_t campass_degree;
    
      int read();
    
    uint8_t buzzer_type;
    
private:
    void init();
    uint8_t* buffer_IntelToMbed;
    uint8_t* buffer_MbedToArduino;
    uint8_t* buffer_compass;
    uint8_t* forward_msg_buffer; //for forwarding message to the car
    uint16_t in_IntelToMbed;
    uint16_t out_IntelToMbed;
    uint16_t in_MbedToArduino;
    uint16_t out_MbedToArduino;
    uint8_t state_IntelToMbed;
    uint8_t state_MbedToArduino;
    uint8_t check_sum;
    uint8_t info_ok_IntelToMbed; //0 is not ok, 1 is car, 2 is lifter and 3 is camera platform
    uint8_t info_ok_MbedToArduino; //0 is not ok, 1 is ok
    
    uint8_t action_type;
    uint16_t move_dis;
    uint8_t move_dir;
    uint16_t rotate_dis;
    uint8_t rotate_dir;
    
    
    uint8_t _MSB;
    uint8_t _LSB;

    uint16_t _in;
    uint16_t _out;
    
    
    void write2Bytes(char msb, char lsb);
    void run();
    void stop();
    void resume();
    void reset();
    void clearBuffer();
    
    MySerial *_DEBUG;
    MySerial *_IntelToMbed;
    MySerial *_MbedToArduino;
    MySerial *CompassData;
};

#endif