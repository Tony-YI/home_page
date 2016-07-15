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
#include "camera_platform.h"

//public
Camera_platform::Camera_platform(MyPwmOut* _pwmRoll, MyPwmOut* _pwmPitch, MyPwmOut* _pwmYaw)
{
    this->_pwmRoll = _pwmRoll;
    this->_pwmPitch = _pwmPitch;
    this->_pwmYaw = _pwmYaw;
    
    _roll_angle = _pitch_angle = _yaw_angle = 0;
    _roll_current_pulsewidth_us = ROLL_MID;
    _pitch_current_pulsewidth_us = PITCH_MID;
    _yaw_current_pulsewidth_us = YAW_MID;
}

Camera_platform::~Camera_platform()
{
    delete _pwmRoll;
    delete _pwmPitch;
    delete _pwmYaw;
}

void Camera_platform::cameraPlatformMove(uint16_t move_dis, uint8_t move_dir, uint16_t rotate_dis, uint8_t rotate_dir)
{
    float _degree = rotate_dis / 100;
    
    if((rotate_dir & 0xc0) == 0xc0 && (rotate_dir & 0x20) == 0x20) //roll
    {
        if((rotate_dir & 0x10) == 0) //roll left
        {
            setRollLeft(_degree);
        }
        else //roll right
        {
            setRollRight(_degree);
        }
    }
    if((rotate_dir & 0xc0) == 0xc0 && (rotate_dir & 0x08) == 0x08) //pitch
    {
        if((rotate_dir & 0x04) == 0) //pitch down
        {
            setPitchDown(_degree);
        }
        else //pitch up
        {
            setPitchUp(_degree);
        }
    }
    if((rotate_dir & 0xc0) == 0xc0 && (rotate_dir & 0x02) == 0x02) //yaw
    {
        if((rotate_dir & 0x01) == 0) //yaw counter clockwise
        {
            setYawCClock(_degree);
        }
        else //yaw right
        {
            setYawClock(_degree);
        }
    }
}  
void Camera_platform::setRollLeft(float _degree)
{
    setPWM(computePwmValue(_degree, 0, ROLL), ROLL);
}
void Camera_platform::setRollRight(float _degree)
{
    setPWM(computePwmValue(_degree, 1, ROLL), ROLL);
}
void Camera_platform::setPitchUp(float _degree)
{
    setPWM(computePwmValue(_degree, 1, PITCH), PITCH);
}
void Camera_platform::setPitchDown(float _degree)
{
    setPWM(computePwmValue(_degree, 0, PITCH), PITCH);
}
void Camera_platform::setYawClock(float _degree)
{
    setPWM(computePwmValue(_degree, 1, YAW), YAW);
}
void Camera_platform::setYawCClock(float _degree)
{
    setPWM(computePwmValue(_degree, 0, YAW), YAW);
}
void Camera_platform::resetCameraPlatform()
{
    setPWM(ROLL_MID, ROLL);
    setPWM(PITCH_MID, PITCH);
    setPWM(YAW_MID, YAW);
}

//private
void Camera_platform::setPWM(uint16_t _pwm_value_us, uint8_t _pwm_channel) //0 is roll, 1 is pitch, 2 is yaw
{
    switch(_pwm_channel)
    {
        case ROLL:
        for(int i = 0; i < abs(_pwm_value_us - _roll_current_pulsewidth_us); i++)
        {
            _pwmRoll->pulsewidth_us(_roll_current_pulsewidth_us + i);
            wait_ms(MSPPWM);
        }
        _roll_current_pulsewidth_us = _pwm_value_us;
        break;
        case PITCH:
        for(int i = 0; i < abs(_pwm_value_us - _pitch_current_pulsewidth_us); i++)
        {
            _pwmPitch->pulsewidth_us(_pitch_current_pulsewidth_us + i);
            wait_ms(MSPPWM);
        }
        _pitch_current_pulsewidth_us = _pwm_value_us;
        break;
        case YAW:
        for(int i = 0; i < abs(_pwm_value_us - _yaw_current_pulsewidth_us); i++)
        {
            _pwmYaw->pulsewidth_us(_yaw_current_pulsewidth_us + i);
            wait_ms(MSPPWM);
        }
        _yaw_current_pulsewidth_us = _pwm_value_us;
        break;
        default:
        break;
    }
    wait_ms(100);
}
uint16_t Camera_platform::computePwmValue(float _degree, uint8_t _dir, uint8_t _pwm_channel) //0 is left/up/clock, 1 is right/down/cclock
{
    uint16_t return_value = 0;
    
    switch(_pwm_channel)
    {
        case ROLL:
        if(_dir == 0)
        {
            _roll_angle -= _degree;
            if(_roll_angle < ROLL_ANGLE_MIN)
            {
                _roll_angle = ROLL_ANGLE_MIN;
            }
        }
        else if(_dir == 1)
        {
            _roll_angle += _degree;
            if(_roll_angle > ROLL_ANGLE_MAX)
            {
                _roll_angle = ROLL_ANGLE_MAX;
            }
        }
        return_value = (uint16_t)(ROLL_MID + _roll_angle * ROLL_USPD);
        break;
        case PITCH:
        if(_dir == 1)
        {
            _pitch_angle -= _degree;
            if(_pitch_angle < PITCH_ANGLE_MIN)
            {
                _pitch_angle = PITCH_ANGLE_MIN;
            }
        }
        else if(_dir == 0)
        {
            _pitch_angle += _degree;
            if(_pitch_angle > PITCH_ANGLE_MAX)
            {
                _pitch_angle = PITCH_ANGLE_MAX;
            }
        }
        return_value = (uint16_t)(PITCH_MID + _pitch_angle * PITCH_USPD);
        break;
        case YAW:
        if(_dir == 1)
        {
            _yaw_angle += _degree;
            if(_yaw_angle > YAW_ANGLE_MAX)
            {
                _yaw_angle = YAW_ANGLE_MAX;
            }
        }
        else if(_dir == 0)
        {
            _yaw_angle -= _degree;
            if(_yaw_angle < YAW_ANGLE_MIN)
            {
                _yaw_angle = YAW_ANGLE_MIN;
            }
        }
        
        return_value = (uint16_t)(YAW_MID + _yaw_angle * YAW_USPD);
        break;
        default:
        break;
    }
    
    return return_value;
}