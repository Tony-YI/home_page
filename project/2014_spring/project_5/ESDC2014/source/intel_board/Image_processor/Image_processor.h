/*

*/

#ifndef _IMAGE_PROCESSOR_H
#define _IMAGE_PROCESSOR_H

#include <stdint.h>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include "../macro.h"
#include <vector>
#include <time.h>
#include "../camera/camera.h"
#include <string>
#include <math.h>

#include "camshiftdemo.h"
#include "fourPointAlgo.h"

extern char *glo_PATH_TEMP;
extern cv::Rect glo_prev_face;
extern uint8_t glo_display_enable;
extern uint8_t glo_high_angle_shot;
extern uint8_t glo_hand_gesture;
extern uint8_t glo_tracking;
extern uint8_t glo_autofocus;
extern uint8_t glo_waist_shot;
extern uint8_t glo_debug_msg;
extern uint8_t glo_multi_target;

enum IMAGE_PROCESS_STATE
{
	IMAGE_PROCESS_INIT
};


class Image_processor
{
private:
	//the name of the displayed window
	char winname[WINNAME_LENGTH];
	char skinwin[WINNAME_LENGTH];
	char edgewin[WINNAME_LENGTH];
	//the state of the Image_processor
	IMAGE_PROCESS_STATE state;
	//the boolean variable which indicates whether there exist a window
	bool win_exist;
	
	
	//this variable indicates where the frame for analysis comes from
	//it's either equal to IMG_SOURCE_CELLPHONE, or IMG_SOURCE_WEBCAM
	uint8_t img_source;

	//cap is useful when img_source == IMG_SOURCE_WEBCAM
	cv::VideoCapture *cap;


//-----------------body DETECTION RELATED VARIABLE--------------------------------------------
	//body_detect stores the rectangle of the body detected in the current_img

	//the HOG for body detection
#ifdef _USE_OPENCL
	cv::ocl::HOGDescriptor::HOGDescriptor hog;
#else
	cv::HOGDescriptor hog;
#endif
//------------------FACE DETECTION RELATED VARIABLE------------------------------------------
	//face_detect stores the rectangle of the face detected in the current_img

	std::vector<cv::Rect> eyes_detect;
	cv::CascadeClassifier face_cascade;
	cv::CascadeClassifier eyes_cascade;
	//indicates the path to the Cascade 
	std::string face_cascade_name;
	std::string eyes_cascade_name;
//-------------------------------------------------------------------------------------------------------------------



	//this function will get image from cell phone, and load the image into the current_img
	uint8_t get_image_from_cellphone();
	//this function will get image from camera, and load the image into the current_img
	uint8_t get_image_from_webcam();

	//this function will run analysis on the current_img
	void side_filtering(const cv::Rect &prev_face);
	void size_filtering(const uint8_t &flags, const cv::Rect &prev_face);
	void rotate_img(const cv::Mat &src,cv::Mat &dst);
	cv::Rect body_by_face(const cv::Rect &face);
	uint32_t count_pic;
public:	
	Image_processor(uint8_t img_source);
	~Image_processor();

	//cam is useful when img_source == IMG_SOURCE_CELLPHONE
	Camera *cam;
	char* current_img_path;
	char* analyzed_img_path;
	char* analyzed_filtered_img_path;
	//these variables stores the detection results
	std::vector<cv::Rect> body_detect;
	std::vector<cv::Rect> face_detect;
	std::vector<cv::Rect> final_body_detect;
	std::vector<cv::Rect> final_face_detect;
	//these variables stores the current image that is being processed
	cv::Mat current_img;
	cv::Mat analyzed_img;
	cv::Mat analyzed_img_filtered;
	cv::Mat skin_img;
	cv::Mat edge_img;
	//these variables stores the expected image parameters
	
	cv::Point point;
	cv::Rect expected_region; //
	cv::Rect face_region;
	int32_t diff_x;
	int32_t diff_y;
	
	//load the Cascade Classifier for face detection, and the HOG SVM for body detection
	uint8_t init();

	//this function will get an image from either cam or cap, and store the image into the current_img
	uint8_t capture_image();

	//this function will stored the image into the hard disk,
	//naming it according to the time since Epoch
	uint8_t save_current_image(uint16_t task_counter);
	
	//this function simplily read the image from file to current_img
	uint8_t read_image(const char* filename);
	//this function will concat the input two images horizontally
	cv::Mat concat_image(const cv::Mat &img1, const cv::Mat &img2,uint8_t dir);
	uint8_t analyze_image();
	
	cv::Mat edge_detection(const cv::Mat &img);
	/*
	
		body detection related functions
	
	*/
	uint8_t run_body_detection(const cv::Mat &source_img,std::vector<cv::Rect> &body_detect);
	cv::Mat mark_detected_body(const cv::Mat &source_img,const std::vector<cv::Rect> &body_detect);

	/*
	
		face detection related functions
	
	*/
	uint8_t run_face_detection(const cv::Mat &source_img,std::vector<cv::Rect> &face_detect);
	cv::Scalar getSkin(const cv::Mat &source_img,cv::Mat &dest_img);
	
	cv::Mat mark_detected_face(const cv::Mat &source_img,const std::vector<cv::Rect> &face_detect);


	void mark_exp_region(const cv::Rect &rect);
	/*
	
		filtering related functions
	
	*/
	//this function will do basic filtering, eliminate body detected result without face detection
	uint8_t basic_filter(const int32_t &degree,const int32_t &dir);
	uint8_t basic_filter_default();
	uint8_t basic_filter_with_degree(const int32_t &degree, const int32_t &dir);
	uint8_t basic_filter_with_gesture();

	void skin_filter(const cv::Mat &source_img);
	uint8_t multi_targets_filter(const uint8_t &num);
	//-------------------------------//
	uint8_t find_body_according_to_face(const cv::Mat &source_img,
		const std::vector<cv::Rect> &face_detect);
	uint8_t find_body_in_roi(const cv::Mat &source_img,const cv::Rect roi,std::vector<cv::Rect> &body_detect);
	uint8_t face_body_related(const cv::Rect &body,const cv::Rect &face);
	/**/
	uint8_t show_analyzed_img(const uint16_t &task_counter,uint8_t enable_display = 0);
	uint8_t get_image_and_show();
	IMAGE_PROCESS_STATE get_state();

	/*
	
		Interface for intel board to use

	*/
	//this function will take a picture of the current scope,
	//and analyze whether there is a person/target in the picture.
	int8_t one_target_in_scope(const uint8_t &flags,int32_t degree = 0,int32_t dir = 0);
	int8_t multi_targets_in_scope(const uint8_t &flags,const uint8_t &num);
	cv::Rect get_detection_result();
	cv::Rect get_face_detection_result();
	double get_distance(const cv::Rect &face);

	/*Image processing related functions*/
	bool need_contrast(const cv::Mat &source_img);
	bool need_flash(const cv::Mat &source_img);

	void flash_on();
	void flash_off();

	void camera_take_photo();
};

#endif
