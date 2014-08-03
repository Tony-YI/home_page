#ifndef COMPASS_H
#define COMPASS_H

#include "mbed.h"
#include "define.h"

#define RUN_MSB          0xC7
#define RUN_LSB          0x10

#define STOP_MSB          0xC6
#define STOP_LSB          0x10

#define RESUME_MSB          0xD1
#define RESUME_LSB          0x10

#define RST_MSB          0xC2
#define RST_LSB          0x10

#define RESUME_MSB          0xD1
#define RESUME_LSB          0x10

#define COMPASS_TX      p9
#define COMPASS_RX      p10

#define _BUFFER_SIZE 512

#define DECLINATIONANGLE  -0.0457
#define OFFSET 0

#include <math.h>

class COMPASS 
{
public:
    COMPASS(MySerial* serial);
    uint16_t read();
    void putToBuffer(uint8_t data);

    
private:
    MySerial* _serial;
    uint16_t _degree;
    uint8_t flag;
    uint8_t count;
    float declinationAngle;
    int offset;
    uint16_t buffer_count;
    char temp[2];
    char buffer[_BUFFER_SIZE];
    uint16_t twobytes;
    uint8_t hundreds,tens,digits;
    
    void init();
    void write2Bytes(char msb, char lsb);
    
    void run();
    void stop();
    void resume();
    void reset();
    
  //  void check_time_out();
//    void time_out_init();
};

#endif /* COMPASS_H */