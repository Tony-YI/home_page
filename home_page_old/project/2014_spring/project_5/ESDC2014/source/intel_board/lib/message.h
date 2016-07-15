
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
#ifndef _MESSAGE_H
#define _MESSAGE_H

#define CAR_ACTION 0
#define LIFTER_ACTION 1
#define CAM_PLATFORM_ACTION 2

#define COMPASS_REQUEST 3

#define BUZZER_REQUEST 4

#define STARTER 0x7E
#define COMPASS_STARTER 0x7D

#define BUZZER_STARTER 0x7C
#define BUZZER_TARGET_NOT_FOUND 0x01
#define BUZZER_TAKE_PHOTO 0x02
#define BUZZER_STAND_BY 0x03

#define MSG_MAX_ATTEMPT 5
#define TIMEOUT 10000
#define LIFTER_TIMEOUT 25000 //timeout as 10 second
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

//for poll() function
#include <poll.h>
//

struct IntelCarCmd
{
	/*
	starter indicates the start of the message
	*/
	uint8_t starter;

	/*
	0 indicates car movement, with rotation
	1 indicates lifter movement. only up and down
	2 indicates camera platform movement, with roll/pitch/yaw
	3 indicates compass request
	*/
	uint8_t action_type;
	 
	/*
	move distance
	only meaningful when action_type = 0 or action_type = 1.
    distance is measured in mm.
	*/
	uint16_t move_dis;
    
	/*
	move direction
	only meaningful when action_type = 0 or action_type = 1.
    for car, directions are up/right/down/left w.r.t 0/1/2/3
    for lifter, directions are up/down w.r.t 0/2
    */
	uint8_t move_dir;
	
	/*
	rotate angle
	this field only meaningful when action_type = 0 or action_type = 2.
	(degree * 100,say, if you are rotating 36.5, the rotate_degree would be 3650)
	*/
	uint16_t rotate_dis;
	
	/*
	rotate direction
	this field only meaningful when action_type = 0 or action_type = 2.
	ratate_dir = (MSB)XXXXXXXX(LSB).
	when action_type = 1, all bits are 0s.
	when action_type = 0 or 2, first 2 bits are 1s.

	when third bit is 1 and fourth bit is 0, means roll left. when second bit is 1, means roll right.
	when fiveth bit is 1 and sixth bit is 0, means pitch down. when second bit is 1, means pitch up.
	when sixth bit is 1 and last bit is 0, means yaw counterclockwise. when second bit is 1, means yaw clockwise.
	*/
	uint8_t rotate_dir;

	/*
	check_sum = action_type + (move_dis >> 8) + (move_dis & 0xff) + move_dir + (rotate_dis >> 8) + (rotate_dis & 0xff) + rotate_dir
	*/
	uint8_t check_sum;
};

struct ACK
{
	/*
	starter indicates the start of the message
	*/
	uint8_t starter;
	uint8_t O; //0x4f
	uint8_t K; //0x4B
	uint8_t check_sum; //0x9A
};

class Message
{
public:
	Message();
	~Message();
	int32_t sendMessage(int fd);
	int32_t safe_sendMessage(int fd);
	int receiveACK(int fd);

	void CarMoveUpMM(uint16_t _mm);
	void CarMoveDownMM(uint16_t _mm);
	void CarMoveLeftMM(uint16_t _mm);
	void CarMoveRightMM(uint16_t _mm);

	void CarRotateLeftDegree(uint16_t _degree);
	void CarRotateRightDegree(uint16_t _degree);

	void LifterMoveUpMM(uint16_t _mm);
	void LifterMoveDownMM(uint16_t _mm);

	void CameraPlatformRollLeft(uint16_t _degree);
	void CameraPlatformRollRight(uint16_t _degree);
	void CameraPlatformPitchUp(uint16_t _degree);
	void CameraPlatformPitchDown(uint16_t _degree);
	void CameraPlatformYawClk(uint16_t _degree);
	void CameraPlatformYawCounterClk(uint16_t _degree);

	void CompassRequest();
	void BuzzerRequest(uint8_t type);

	int32_t car_degree;

private:
	struct IntelCarCmd* _IntelCarCmd;
	struct ACK* _ACK;
	void setCarMove(uint8_t move_dir, uint16_t move_dis);
	void setCarRotate(uint8_t rotate_dir, uint16_t rotate_dis);
	void setLifterMove(uint8_t move_dir, uint16_t move_dis);
	void setCameraPlatformRotate(uint8_t rotate_dir, uint16_t rotate_dis);

	void calCheckSum();
	void resetStruct();
};

#endif
