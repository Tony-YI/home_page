#ifndef _MACRO_H
#define _MACRO_H

//#define _USE_OPENCL

#include "stdint.h"
#include "math.h"
#define PI 3.14159265359

#define DEG_TO_RAD(degree) (degree) * PI / 180  

#define WINNAME_LENGTH 32
#define IMG_SOURCE_CELLPHONE 0
#define IMG_SOURCE_WEBCAM 1

#define FILENAME_LENGTH 128


#define LBP_FACE_CASCADE "opencv/lbpcascade_frontalface.xml"
#define HAAR_FACE_CASCADE "opencv/haarcascade_frontalface_default.xml"
#define HAAR_FACE_CASCADE1 "opencv/haarcascade_frontalface_alt.xml"
#define HAAR_FACE_CASCADE2 "opencv/haarcascade_frontalface_alt2.xml"

#define PATH_TO_FACE_CASCADE HAAR_FACE_CASCADE
#define PATH_TO_EYES_CASCADE "haarcascade_eye_tree_eyeglasses.xml"

#define IP "192.168.1.101"
#define PORT "8080"
#define IP_PORT "192.168.1.101:8080"

extern char *glo_PATH_TEMP;
extern unsigned char continuity;
extern uint8_t glo_source_mode;

#define PATH_CAPTURE "../../Photos/Capture"

#define PHONE 0
#define CANON 1

/* enum type definition START */
/* enum type definition START */
/* enum type definition START */
enum command_type{
	undefined, connect_request, start_movement, confirm_picture,
	pattern_1, pattern_2, pattern_3, pattern_4, pattern_5,
	pattern_6, pattern_7, pattern_8, pattern_9, pattern_10,
	pattern_diy, pattern_video,
	car_forward, car_backward, car_left, car_right,
	camera_forward, camera_backward, camera_left, camera_right,
	lift_up, lift_down,
	set_waist_shot};

enum PHOTO_MODE{
	SINGLE_PHOTO,
	DOUBLE_PHOTO,
	MULTI_PHOTO
};

enum ROBOT_STATE {
	ROBOT_INIT, ROBOT_READY, //actually waiting for command
	ROBOT_NO_TARGET, ROBOT_FIND_TARGET,
	ROBOT_EVALUATE_IMAGE, ROBOT_ANALYZE_IMAGE,
	ROBOT_EVALUATE_MOVEMENT, ROBOT_WAIT_FOR_ADJUSTMENT,
};

enum ROBOT_MODE{
	AUTO_MODE, DEBUG_MODE, MULTI_MODE
};

enum Direction{
	UPPER_LEFT,
	LOWER_LEFT,
	UPPER_RIGHT,
	LOWER_RIGHT,
	null
};
/* enum type definition END */
/* enum type definition END */
/* enum type definition END */

#define AUTOFOCUS

#define ENABLE_BODY_DETECT 0x01
#define ENABLE_FACE_DETECT 0x02
#define ENABLE_SIDE_FILTERING 0x04
#define ENABLE_SIZE_FILTERING_SMALL 0x08
#define ENABLE_SIZE_FILTERING_LARGE 0x10
#define ENABLE_CURRENT_FRAME 0x20

#define PIXEL_HEIGHT 127/480 //127mm for a 480 pixel height
#define PIXEL_WIDTH PIXEL_HEIGHT //consider PIXEL as a square
#define FOCAL_LENGTH 43 //43 mm for 


#define F_NUMBER 2.4
#define SENSOR_DIAGONAL 5.68
#define SENSOR_WIDTH 4.54
#define SENSOR_HEIGHT 3.42



//IMAGE PROCESSING THRESHOLD
#define IMG_ORI_WIDTH 640
#define IMG_ORI_HEIGHT 480

#define IMG_WIDTH 640
#define IMG_HEIGHT 480
#define IMG_CENTER_X IMG_WIDTH/2
#define IMG_CENTER_Y IMG_HEIGHT/2

#define IMG_HORI_THRESHOLD 40
#define IMG_VERT_THRESHOLD 40
#define IMG_BODY_FACE_RATIO 7.5

#define IMG_BODY_ACTUAL_HEIGHT 1700 //mm -> 170cm
#define IMG_EXP_HEIGHT 350
#define IMG_EXP_WIDTH 180

#define GOLDEN_RATIO 0.618

//face related
#define IMG_FACE_ACTUAL_HEIGHT 230 //mm
#define IMG_fACE_ACTUAL_WIDTH IMG_FACE_ACTUAL_HEIGHT //mm


#define IMG_EXP_FACE_HEIGHT (IMG_HEIGHT / 5)
#define IMG_EXP_FACE_WIDTH IMG_EXP_FACE_HEIGHT

#define IMG_EXP_FACE_HEIGHT_FULL 40 
#define IMG_EXP_FACE_WIDTH_FULL 40

#define IMG_FACE_WIDTH_MIN 35
#define IMG_FACE_WIDTH_MAX (IMG_HEIGHT / 4)
#define IMG_FACE_HEIGHT_MIN IMG_FACE_WIDTH_MIN
#define IMG_FACE_HEIGHT_MAX IMG_FACE_WIDTH_MAX


#define IMG_EXP_FACE_POS_X ((uint16_t) IMG_WIDTH * (GOLDEN_RATIO + 0.20))
#define IMG_EXP_FACE_POS_Y (IMG_HEIGHT - ((uint16_t) (IMG_HEIGHT * (GOLDEN_RATIO + 0.15))))

#define IMG_EXP_FACE_POS2_X (IMG_WIDTH - ((uint16_t) IMG_WIDTH * (GOLDEN_RATIO + 0.1)))
#define IMG_EXP_FACE_POS2_Y IMG_EXP_FACE_POS_Y

#define IMG_EXP_FACE_POS3_X IMG_EXP_FACE_POS_X
#define IMG_EXP_FACE_POS3_Y IMG_HEIGHT * (1 - GOLDEN_RATIO)

#define IMG_EXP_FACE_POS4_X IMG_EXP_FACE_POS2_X
#define IMG_EXP_FACE_POS4_Y IMG_EXP_FACE_POS3_Y

#define IMG_HORI_THRESHOLD_FACE 30
#define IMG_VERT_THRESHOLD_FACE 25
#define IMG_TRACKING_THRESHOLD 7
#define IMG_Y_MIN 0
#define IMG_Y_MAX 255
#define IMG_Cr_MIN 133
#define IMG_Cr_MAX 173
#define IMG_Cb_MIN 77
#define IMG_Cb_MAX 137

#define FACE_HEIGHT_DIFF_THRESHOLD_LARGE 10
#define FACE_HEIGHT_DIFF_THRESHOLD_SMALL 5 
//body related
#define IMG_EXP_DIS 4887.5



#define IMG_EXP_POS1_X ((uint16_t) IMG_WIDTH * (GOLDEN_RATIO + 0.12))
#define IMG_EXP_POS1_Y IMG_HEIGHT - ((uint16_t) (IMG_HEIGHT * (GOLDEN_RATIO + 0.15)))

#define IMG_EXP_POS2_X IMG_WIDTH - ((uint16_t) IMG_WIDTH * (GOLDEN_RATIO + 0.07))
#define IMG_EXP_POS2_Y IMG_HEIGHT - ((uint16_t) (IMG_HEIGHT * (GOLDEN_RATIO + 0.15)))

#define IMG_EXP_POS3_X IMG_CENTER_X
#define IMG_EXP_POS3_Y IMG_HEIGHT - ((uint16_t) (IMG_HEIGHT * (GOLDEN_RATIO)))

#define IMG_EXP_POS4_X IMG_WIDTH - 20
#define IMG_EXP_POS4_Y IMG_EXP_POS3_Y



//MOVEMENT RELATED
#define DEFAULT_DIS 400
#define DEFAULT_DIS_LARGE 800
#define DEFAULT_DIS_SMALL 275
#define DEFAULT_DIS_TINY 150
#define DEFAULT_DEGREE 30
#define CAR_FORWARD 0
#define CAR_BACKWARD 1
#define CAR_LEFT 2
#define CAR_RIGHT 3

#define CAR_ROTATE_RIGHT 0
#define CAR_ROTATE_LEFT 1

#define CAR_MOVEMENT_THRESHOLD 75
#define LIFTER_UP 0
#define LIFTER_DOWN 1
#define LIFTER_MIN 0 //the minimum height of the lifter in mm
#define LIFTER_MAX 700 //the maximun height of the lifter in mm
#define LIFTER_INIT_POS 350
#define LIFTER_SEG 50

#define CAM_ROLL_LEFT 0
#define CAM_ROLL_RIGHT 1
#define CAM_PITCH_UP 2
#define CAM_PITCH_DOWN 3
#define CAM_YAW_LEFT 4
#define CAM_YAW_RIGHT 5

#define CAM_HIGH_ANGLE 45

#define ORIENTATION_THRESHOLD 10

#define HAND_SKIN_THRESHOLD 50


#define CONTROLLER_DEGREE_P 0.75
#define CONTROLLER_DEGREE_I 0
#define CONTROLLER_DEGREE_D 0.01

#define MAX_ADJUST_NUM 3

#define CAR_ROTATE_THRESHOLD 10
#endif
// 27828986
