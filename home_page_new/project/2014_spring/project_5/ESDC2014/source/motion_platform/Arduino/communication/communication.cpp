/******************************************************

***┏┓           ┏┓
**┏┛┻━━━━━━━━━━━┛┻┓
**┃               ┃
**┃      ━━━      ┃
**┃  ┳┛       ┗┳  ┃
**┃               ┃
**┃ '''   ┻   ''' ┃
**┃               ┃
**┗━━━┓       ┏━━━┛
******┃       ┃
******┃       ┃
******┃       ┃
******┃       ┗━━━━━━━━━━━━┓
******┃                    ┃━━┓
******┃      NO BUG        ┏━━┛
******┃                    ┃
******┗━┓  ┓  ┏━━━━┏━━┓  ━━┛
********┃  ┛  ┛    ┃  ┛  ┛
********┃  ┃  ┃    ┃  ┃  ┃
********┗━━┛━━┛    ┗━━┛━━┛

This part is added by project ESDC2014 of CUHK team.
All the code with this header are under GPL open source license.
******************************************************/
#include <Arduino.h> //Arduino sucks
#include <communication.h>
#include <Omni4WD.h>

Communication::Communication()
{
	init();
}

Communication::~Communication()
{
	delete[] buffer;
}

void Communication::init()
{
	buffer = new uint8_t[BUFFER_SIZE];
	in = 0;
	out = 0;
	state = 0;
	check_sum = 0;
	info_ok = 0;
}

uint8_t Communication::getByte()
{
	uint8_t _x = buffer[out++];
	if(out == BUFFER_SIZE)
	{
		out &= 0x0000;
	}
	return _x;
}

uint16_t Communication::get2Bytes()
{
	uint8_t byte1 = getByte();
	uint8_t byte2 = getByte();
	return uint16_t((byte1 << 8) | byte2);
}

void Communication::putByte(uint8_t _x)
{
	Serial.write(_x);//For Arduino
}

void Communication::put2Bytes(uint16_t _x)
{
	putByte(uint8_t(_x >> 8));
	putByte(uint8_t(_x & 0x0f));
}

void Communication::putToBuffer(uint8_t _x)
{
	buffer[in++] = _x;
	if(in == BUFFER_SIZE)
	{
		in &= 0x0000;
	}
}

void Communication::parseMessage()
{
	if(in != out)
	{
		uint8_t _x = getByte();
		switch(state)
		{
			case 0: //checking starter
			{
				putByte(0);
				check_sum = 0;

				if(_x == STARTER)
				{
					state++;
				}
				else
				{
					state = 0;
				}
				break;
			}

			case 1: //checking action_type
			{
				putByte(1);
				check_sum += _x;
				action_type = _x;
				if(action_type == 0 || action_type == 1 || action_type == 2)
				{
					state++;
				}
				else
				{
					state = 0;
				}
				break;
			}

			case 2: //move_dis upper 4 bits
			{
				putByte(2);
				check_sum += _x;
				move_dis = _x << 8;
				state++;
				break;
			}

			case 3: //move_dis lower 4 bits
			{
				putByte(3);
				check_sum += _x;
				move_dis |=  _x;
				state++;
				break;
			}

			case 4: //move_dir
			{
				putByte(4);
				check_sum += _x;
				move_dir = _x;
				if(action_type == 0 && (move_dir == 0 || move_dir == 1 || move_dir == 2 || move_dir == 3))
				{
					state++;
				}
				else
				{
					state = 0;
				}
				break;
			}

			case 5: //rotate_dis upper 4 bits
			{
				putByte(5);
				check_sum += _x;
				rotate_dis = _x << 8;
				state++;
				break;
			}

			case 6: //rotate_dis lower 4 bits
			{
				putByte(6);
				check_sum += _x;
				rotate_dis |= _x;
				state++;
				break;
			}

			case 7: //rotate_dir
			{
				putByte(7);
				check_sum += _x;
				rotate_dir = _x;
				if((action_type == 1 && ((rotate_dir >> 6) == 0)) || ((action_type == 0 || action_type == 2) && ((rotate_dir >> 6) == 3)))
				{
					state++;
				}
				else
				{
					state = 0;
				}
				break;
			}

			case 8: //check_sum
			{
				putByte(8);
				if(check_sum == _x)
				{
					switch(action_type)
					{
						case 0: //car movement
						info_ok = 1;
						break;

						case 1: //lifter
						info_ok = 2;
						break;

						case 2: //camera platform
						info_ok = 3;
						break;

						default:
						info_ok = 0; //not ok
						break;
					}
				}
				state  = 0;
				break;
			}

			default:
			{
				state = 0;
				break;
			}
		}
	}
}

void Communication::ACK()
{
	putByte(STARTER);
    putByte(0x4f); //O
    putByte(0x4b); //K
    putByte(0x9a); //check_sum = 0xaf + 0x4b = 0x9a
}

uint8_t Communication::getInfoOK()
{
	return info_ok;
}

void Communication::resetInfoOK()
{
	info_ok = 0;
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