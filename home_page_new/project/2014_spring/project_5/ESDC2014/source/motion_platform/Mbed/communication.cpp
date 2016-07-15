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
#include <communication.h>

Communication::Communication(MySerial* _DEBUG, MySerial *_IntelToMbed, MySerial *_MbedToArduino, MySerial* CompassData)
{
    this->_DEBUG = _DEBUG;
    this->_IntelToMbed = _IntelToMbed;
    this->_MbedToArduino = _MbedToArduino;
    this->CompassData = CompassData;
    init();
}

Communication::~Communication()
{
    delete[] buffer_IntelToMbed;
    delete[] buffer_MbedToArduino;
    delete[] buffer_compass;
    delete[] forward_msg_buffer;
    delete _DEBUG;
    delete _IntelToMbed;
    delete _MbedToArduino;
    delete CompassData;
    
   
}

void Communication::init()
{
    buffer_IntelToMbed = new uint8_t[BUFFER_SIZE];
    buffer_MbedToArduino = new uint8_t[BUFFER_SIZE];
    buffer_compass = new uint8_t [BUFFER_SIZE];

    
    forward_msg_buffer = new uint8_t[9]; //the message struct is 9 byte
    
    in_IntelToMbed = 0;
    out_IntelToMbed = 0;
    in_MbedToArduino = 0;
    out_MbedToArduino = 0;
    state_IntelToMbed = 0;
    state_MbedToArduino = 0;
    check_sum = 0;
    info_ok_IntelToMbed = 0;
    info_ok_MbedToArduino = 0;
    
    buzzer_type=0;
    
   
    _MSB = 0;
    _LSB = 0;
    _in = _out = 0;
    
    
}

uint8_t Communication::getByte(uint8_t communication_type)
{
    uint8_t _x = 0;
    if(communication_type == 0)
    {
        _x = buffer_IntelToMbed[out_IntelToMbed++];
        if(out_IntelToMbed == BUFFER_SIZE-1)
        {
            out_IntelToMbed &= 0x0000;
        }
    }
    else if(communication_type == 1)
    {
        _x = buffer_MbedToArduino[out_MbedToArduino++];
        if(out_MbedToArduino == BUFFER_SIZE-1)
        {
            out_MbedToArduino &= 0x0000;
        }
    }
     else if(communication_type == 2)
    {
        _x = buffer_compass[_out++];
        if(_out == BUFFER_SIZE-1)
        {
            _out &= 0x0000;
        }
    }
 
    return _x;
}

uint16_t Communication::get2Bytes(uint8_t communication_type)
{
    uint8_t byte1 = getByte(communication_type);
    uint8_t byte2 = getByte(communication_type);
    return uint16_t((byte1 << 8) | byte2);
}

void Communication::putByte(uint8_t _x, uint8_t _i)
{
    //Serial.write(_x);//For Arduino
     //For Mbed
    if(_i == 0) //_DEBUG
    {
        _DEBUG->putc(_x);
    }
    else if(_i == 1) //IntelToMbed
    {
        _IntelToMbed->putc(_x);
    }
    else if(_i == 2) //MbedToArduino
    {
        _MbedToArduino->putc(_x);
    }
   
}

void Communication::put2Bytes(uint16_t _x, uint8_t _i)
{
    putByte(uint8_t(_x >> 8), _i);
    putByte(uint8_t(_x & 0x0f), _i);
}

void Communication::putToBuffer(uint8_t _x, uint8_t communication_type)
{
    if(communication_type == 0)
    {
        buffer_IntelToMbed[in_IntelToMbed++] = _x;
        if(in_IntelToMbed == BUFFER_SIZE-1)
        {
            in_IntelToMbed &= 0x0000;
        }
    }
    else if(communication_type == 1)
    {
        buffer_MbedToArduino[in_MbedToArduino++] = _x;
        if(in_MbedToArduino == BUFFER_SIZE-1)
        {
            in_MbedToArduino &= 0x0000;
        }
    }
    else if(communication_type == 2)
    {
        
            buffer_compass[_in++] = _x;
            if(_in == BUFFER_SIZE-1)
            {
                _in &= 0x0000;
            }
    }
}

void Communication::parseMessage()
{
    if(in_IntelToMbed != out_IntelToMbed)
    {
        uint8_t _x = getByte(0);
        switch(state_IntelToMbed)
        {
            case 0: //checking starter
            {
                if(DEBUG_ON)
                {
                    _DEBUG->printf("Communication::parseMessage(). Checking STARTER...\r\n");
                }
                check_sum = 0;

                if(_x == STARTER || _x == COMPASS_STARTER || _x == BUZZER_STARTER)
                {
                    state_IntelToMbed++;
                    forward_msg_buffer[0] = _x;
                }
                else
                {
                    state_IntelToMbed = 0;
                    if(DEBUG_ON)
                    {
                        _DEBUG->printf("Communication::parseMessage(). ERROR when checking STARTER: %x.\r\n", _x);
                    }
                }
                break;
            }

            case 1: //checking action_type
            {
                if(DEBUG_ON)
                {
                    _DEBUG->printf("Communication::parseMessage(). Checking ACTION_TYPE...\r\n");
                }
                check_sum += _x;
                action_type = _x;
                if(action_type == 0 || action_type == 1 || action_type == 2 || action_type == 3 || action_type == 4)
                {
                    state_IntelToMbed++;
                    forward_msg_buffer[1] = _x;
                }
                else
                {
                    state_IntelToMbed = 0;
                    if(DEBUG_ON)
                    {
                        _DEBUG->printf("Communication::parseMessage(). ERROR when checking ACTION_TYPE: %x.\r\n", action_type);
                    }
                }
                break;
            }

            case 2: //move_dis upper 4 bits
            {
                if(DEBUG_ON)
                {
                    _DEBUG->printf("Communication::parseMessage(). Checking MOVE_DIS upper 4 bits...\r\n");
                }
                check_sum += _x;
                move_dis = _x << 8;
                state_IntelToMbed++;
                forward_msg_buffer[2] = _x;
                break;
            }

            case 3: //move_dis lower 4 bits
            {
                if(DEBUG_ON)
                {
                    _DEBUG->printf("Communication::parseMessage(). Checking MOVE_DIS lower 4 bits...\r\n");
                }
                check_sum += _x;
                move_dis |=  _x;
                state_IntelToMbed++;
                forward_msg_buffer[3] = _x;
                break;
            }

            case 4: //move_dir
            {
                if(DEBUG_ON)
                {
                    _DEBUG->printf("Communication::parseMessage(). Checking MOVE_DIR...\r\n");
                }
                check_sum += _x;
                move_dir = _x;
                if((action_type == 0 && (move_dir == 0 || move_dir == 1 || move_dir == 2 || move_dir == 3)) || (action_type == 1 && (move_dir == 0 || move_dir == 2)) || action_type == 2 || action_type == 3 || action_type == 4)
                {
                    state_IntelToMbed++;
                    forward_msg_buffer[4] = _x;
                    buzzer_type = _x;
                }
                else
                {
                    state_IntelToMbed = 0;
                    if(DEBUG_ON)
                    {
                        _DEBUG->printf("Communication::parseMessage(). ERROR when checking MOVE_DIR: %x.\r\n", move_dir);
                    }
                }
                break;
            }

            case 5: //rotate_dis upper 4 bits
            {
                if(DEBUG_ON)
                {
                    _DEBUG->printf("Communication::parseMessage(). Checking ROTATE_DIS upper 4 bits...\r\n");
                }
                check_sum += _x;
                rotate_dis = _x << 8;
                state_IntelToMbed++;
                forward_msg_buffer[5] = _x;
                break;
            }

            case 6: //rotate_dis lower 4 bits
            {
                if(DEBUG_ON)
                {
                    _DEBUG->printf("Communication::parseMessage(). Checking ROTATE_DIS lower 4 bits...\r\n");
                }
                check_sum += _x;
                rotate_dis |= _x;
                state_IntelToMbed++;
                forward_msg_buffer[6] = _x;
                break;
            }

            case 7: //rotate_dir
            {
                if(DEBUG_ON)
                {
                    _DEBUG->printf("Communication::parseMessage(). Checking MOVE_DIR...\r\n");
                }
                check_sum += _x;
                rotate_dir = _x;
                if(action_type == 3 || action_type == 4 || (action_type == 1 && ((rotate_dir >> 6) == 0)) || ((action_type == 0 || action_type == 2) && ((rotate_dir >> 6) == 3)))
                {
                    state_IntelToMbed++;
                    forward_msg_buffer[7] = _x;
                }
                else
                {
                    state_IntelToMbed = 0;
                    if(DEBUG_ON)
                    {
                        _DEBUG->printf("Communication::parseMessage(). ERROR when checking ROTATE_DIR: %x.\r\n", rotate_dir);
                    }
                }
                break;
            }

            case 8: //check_sum
            {
                if(DEBUG_ON)
                {
                    _DEBUG->printf("Communication::parseMessage(). Checking CHECK_SUM...\r\n");
                }
                if(check_sum == _x)
                {
                    forward_msg_buffer[8] = _x;
                    switch(action_type)
                    {
                        case 0: //car movement
                        info_ok_IntelToMbed = 1;
                        break;

                        case 1: //lifter
                        info_ok_IntelToMbed = 2;
                        break;

                        case 2: //camera platform
                        info_ok_IntelToMbed = 3;
                        break;
                        
                        case 3: //compass
                        info_ok_IntelToMbed = 4;
                        break;
                        
                        case 4: //buzzer
                        info_ok_IntelToMbed = 5;
                        break;

                        default:
                        info_ok_IntelToMbed = 0; //not ok
                        break;
                    }
                }
                else
                {
                    if(DEBUG_ON)
                    {
                        _DEBUG->printf("Communication::parseMessage(). ERROR when checking CHECK_SUM: %x.\r\n", check_sum);
                    }
                }
                state_IntelToMbed  = 0;
                break;
            }

            default:
            {
                state_IntelToMbed = 0;
                break;
            }
        }
    }
}

void Communication::forwardMessage()
{
    //message structure is defined in source/motion_platform/intel_board/lib/message.h
    putByte(forward_msg_buffer[0], 2); //starter, 2 means MbedToArduino
    putByte(forward_msg_buffer[1], 2); //action_type
    putByte(forward_msg_buffer[2], 2); //move_dis
    putByte(forward_msg_buffer[3], 2);
    putByte(forward_msg_buffer[4], 2); //move_dir
    putByte(forward_msg_buffer[5], 2); //rotate_dis
    putByte(forward_msg_buffer[6], 2);
    putByte(forward_msg_buffer[7], 2); //rotate_dir
    putByte(forward_msg_buffer[8], 2); //checksum
}

void Communication::ACK(Lifter* lifter, Camera_platform* camera_platform)
{
    if(action_type == 0) //car movement
    {
        for(int i = 0; i < 9; i++)
        {
            uint8_t _y = getByte(1);
            printf("Communication::ACK(). Get byte: %x\r\n", _y);
        }
        
        while(info_ok_MbedToArduino != 1)
        {
            if(in_MbedToArduino != out_MbedToArduino)
            {
                uint8_t _x = getByte(1);
                switch(state_MbedToArduino)
                {
                    case 0: //checking starter
                    {
                        if(DEBUG_ON)
                        {
                            _DEBUG->printf("Communication::ACK(). Checking SARTER...\r\n");
                        }
                        
                        if(_x == STARTER)
                        {
                            state_MbedToArduino++;
                        }
                        else
                        {
                            if(DEBUG_ON)
                            {
                                _DEBUG->printf("Communication::ACK(). ERROR when checking SARTER: %x\r\n", _x);
                            }
                            state_MbedToArduino = 0;
                        }
                        break;
                    }

                    case 1: //checking 'O'
                    {
                        if(DEBUG_ON)
                        {
                            _DEBUG->printf("Communication::ACK(). Checking O...\r\n");
                        }
                        
                        if(_x == 0x4f) //O
                        {
                            state_MbedToArduino++;
                        }
                        else
                        {
                            if(DEBUG_ON)
                            {
                                _DEBUG->printf("Communication::ACK(). ERROR when checking O: %x\r\n", _x);
                            }
                            state_MbedToArduino = 0;
                        }
                        break;
                    }

                    case 2: //checking 'K'
                    {
                        if(DEBUG_ON)
                        {
                            _DEBUG->printf("Communication::ACK(). Checking K...\r\n");
                        }
                        
                        if(_x == 0x4b) //K
                        {
                            state_MbedToArduino++;
                        }
                        else
                        {
                            if(DEBUG_ON)
                            {
                                _DEBUG->printf("Communication::ACK(). ERROR when checking K: %x\r\n", _x);
                            }
                            state_MbedToArduino = 0;
                        }
                        break;
                    }

                    case 3: //checking check_sum_MbedToArduino
                    {
                        if(DEBUG_ON)
                        {
                            _DEBUG->printf("Communication::ACK(). Checking CHECK_SUM...\r\n");
                        }
                        
                        if(_x == 0x9a) //checksum
                        {
                            info_ok_MbedToArduino = 1;
                        }
                        else
                        {
                            if(DEBUG_ON)
                            {
                                _DEBUG->printf("Communication::ACK(). ERROR when checking CHECK_SUM: %x\r\n", _x);
                            }
                        }
                        
                        state_MbedToArduino = 0;
                        break;
                    }

                    default:
                    {
                        state_MbedToArduino = 0;
                        info_ok_MbedToArduino = 0;
                        break;
                    }
                }
            }
        }
    }
    
    else if(action_type == 1) //lifter
    {
        uint32_t pulseCountOld = 0;
        uint32_t pulseCountNew = 0;
        while(!lifter->isStopped())
        {
            pulseCountOld = lifter->pulseCount;
            wait_ms(50);
            pulseCountNew = lifter->pulseCount;
            if(pulseCountOld == pulseCountNew)
            {
                break;
            }
        }
    }
    else if(action_type == 3)
    {
        
            printf("Entering Compass::read()...\r\n");
            
            stop();
            clearBuffer();
            
           
            printf("0\r\n");
            resume();
            if(buffer_compass[_out++] == ACK_RESUME_MSB && buffer_compass[_out++] == ACK_RESUME_LSB)
            {
            }
            else
            {
             clearBuffer();
            }
            
            printf("1\r\n");
            run();
            
            if(buffer_compass[_out++] == ACK_RUN_MSB && buffer_compass[_out++] == ACK_RUN_LSB)
            {
            }
            else
            {
                clearBuffer();
            }
            
            printf("2\r\n");
            _MSB = buffer_compass[_out++];
            _LSB = buffer_compass[_out++];
            
             if( 0xa0==( (_MSB) & 0xe0))
            { 
                    printf("match !!!!!\r\n");
                    uint8_t digits= _LSB & 0x0f;
                    uint8_t tens= (_LSB>>4) & 0x0f;
                    uint8_t hundreds= _MSB & 0x07;
                    campass_degree=100*hundreds+10*tens+digits;
            }
            
            else
            {
            printf("Error data !!!!!!!!!!!\r\n");
            campass_degree=0;
            }

            printf("_MSB: %x, _LSB: %x\r\n", _MSB, _LSB);
                        
            printf("3\r\n");
            stop();
            
            uint16_t index1=_in - 1;
            uint16_t index2=_in;
            
            if(_in == 0)
            {
                index1=BUFFER_SIZE - 1;
                index2=0;
            }
            else
            {
                index1=_in - 1;
                index2=_in;
            }
            
            if(buffer_compass[index1] == ACK_STOP_MSB && buffer_compass[index2] == ACK_STOP_LSB)
            {
            }
            else
            {
            }
            _out = _in;
            clearBuffer();
            
            printf("Exiting Compass::read()...\r\n");

              campass_degree+=90;
               
               if(campass_degree>=360)
               campass_degree-=360;
              
                   printf("campass_degree: %d\r\n",campass_degree);
              printf("----------------------\r\n");
                    
        
                    
        uint8_t temp1,temp2;
        temp1 = campass_degree;
        temp2 = campass_degree>>8;
        putByte(COMPASS_STARTER ,1); //1 means IntelToMbed
        putByte(temp1 ,1); //O
        putByte(temp2 ,1); //K
        putByte(0x9a ,1); //check_sum = 0xaf + 0x4b = 0x9a
    
        return;
    } 
     else if(action_type == 4)
    {
        
        
        
        
        putByte(BUZZER_STARTER ,1); //1 means IntelToMbed
        putByte(0,1); //O
        putByte(0,1); //K
        putByte(0,1); //check_sum = 0xaf + 0x4b = 0x9a
    
        return;
    } 
    
    putByte(STARTER ,1); //1 means IntelToMbed
    putByte(0x4f ,1); //O
    putByte(0x4b ,1); //K
    putByte(0x9a ,1); //check_sum = 0xaf + 0x4b = 0x9a
}

uint8_t Communication::getInfoOK(uint8_t communication_type)
{
    if(communication_type == 0)
    {
        return info_ok_IntelToMbed;
    }
    else if(communication_type == 1)
    {
        return info_ok_MbedToArduino;
    }
    return 0; //error
}

void Communication::resetInfoOK(uint8_t communication_type)
{
    if(communication_type == 0)
    {
        info_ok_IntelToMbed = 0;
    }
    else if(communication_type == 1)
    {
        info_ok_MbedToArduino = 0;
    }
}

uint16_t Communication::getMoveDis()
{
    return move_dis;
}

uint16_t Communication::getRotateDis()
{
    return rotate_dis;
}

uint8_t Communication::getMoveDir()
{
    return move_dir;
}

uint8_t Communication::getRotateDir()
{
    return rotate_dir;
}



void Communication::run() 
{
    printf("Entering Compass::run()...\r\n");
    write2Bytes(RUN_MSB, RUN_LSB);
}

void Communication::stop() 
{
    printf("Entering Compass::stop()...\r\n");
    write2Bytes(STOP_MSB, STOP_LSB);
}

void Communication::resume() 
{
    printf("Entering Compass::resume()...\r\n");
    write2Bytes(RESUME_MSB, RESUME_LSB);
}

void Communication::reset() 
{
    printf("Entering Compass::reset()...\r\n");
    write2Bytes(RST_MSB, RST_LSB);
}

void Communication::write2Bytes(char msb, char lsb) 
{
    printf("before writable...\r\n");
    if(CompassData->writeable())
    {
        CompassData->putc(msb);
    }
    printf("_MSB = %x wrote...\r\n", msb);
    if(CompassData->writeable())
    {
        CompassData->putc(lsb);
    }
    printf("_LSB = %x wrote...\r\n", lsb);
    wait(0.1);
}

void Communication::clearBuffer()
{
    _in = _out = 0;
    memset(buffer_compass,0,BUFFER_SIZE);
}