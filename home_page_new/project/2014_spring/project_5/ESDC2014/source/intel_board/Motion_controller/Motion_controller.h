/*
 * Copyright (C) Your copyright notice.
 *
 * Author: Edward HUANG@CUHK huangxx_2155@live.com
 *         Tony Yi@CUHK     
 *         Terry Lai@CUHK
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the PG_ORGANIZATION nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY	THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS-IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _MOTION_CONTROLLER_H
#define _MOTION_CONTROLLER_H

//for queue
#include <queue>
//for standard int type
#include <stdint.h>
#include <stdio.h>
#include "Controller_Com.h"
#include <opencv2/opencv.hpp>
#include "../lib/message.h"
#include "../macro.h"
#include "PIDcontroller.h"
#include "../Image_processor/fourPointAlgo.h"
#include "../Image_processor/camshiftdemo.h"

#define EVAL_INIT 0
#define EVAL_CENTERING 1
#define EVAL_ZOOM_IN 2
#define EVAL_ZOOM_OUT 3
#define EVAL_ADJUSTING 4
#define EVAL_COMPLETE 5

extern uint8_t glo_motion_enable;
extern uint8_t glo_high_angle_shot;
extern uint8_t glo_tracking;
extern uint8_t glo_test_filetransfer;
extern uint8_t glo_waist_shot;
extern uint8_t glo_num_target;
extern uint8_t glo_pid;
extern uint8_t glo_debug_msg;
extern uint8_t glo_tracking_by_rotate;

class Motion_controller
{
private:
	//this is the reference rectangle, where the person should be
	std::queue<Message> cmd_queue;
	uint8_t eval_state;
	uint16_t lifter_pos; //unit mm.
	uint16_t car_orientation;//unit degree
	uint16_t car_original_orientation;//unit degree
	uint16_t phone_orientation;//unit degree

	/*
	variable for reference in image evaluation:

		center_x, center y -> the center of the image.
		threshold_x, threshold_y -> the threshold for detected body region horizontally and vertically.
		threshold_face_x, threshold_face_y -> the threshold for detected face region horizontally and vertically

		img_exp_pos_x, img_exp_pos_y -> the expected position of x and y of the detected body (top center point)
		exp_width, exp_height -> the expected height and width of the detected body

		img_exp_face_pos_x, img_exp_face_pos_y -> the expected x and y of the detected body (top center point)
		exp_face_height, exp_face_width -> the expected height and width of the detected face
	*/
	uint16_t threshold_x, threshold_y;
	uint16_t img_exp_pos_x, img_exp_pos_y, exp_width, exp_height;
	uint16_t center_x, center_y;

	uint16_t threshold_face_y, threshold_face_x;
	uint16_t exp_face_height, exp_face_width;
	uint16_t img_exp_face_pos_x, img_exp_face_pos_y;
	uint8_t adjusting_counter;
	//-------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------//

	uint8_t centering(const cv::Rect &detect,const cv::Rect &face);
	uint8_t centering_by_face(const cv::Rect &face);
	uint8_t multi_face_centering(const std::vector<cv::Rect> &faces,const cv::Rect &face_region);
	uint8_t zoom_in_out(const cv::Rect &detect,const double &distance);
	uint8_t adjusting(const cv::Rect &detect);
	uint8_t adjusting_by_face(const cv::Rect &face);

	uint16_t bound_dis(const uint32_t &dis);

	uint8_t zoom_in_out_by_distance(const double &exp_distance, const double &distance);
	uint8_t zoom_in_out_by_default(const cv::Rect &detect,const double &distance);
	uint8_t zoom_in_out_by_face(const cv::Rect &face,const double &distance);
	uint8_t multi_face_zooming(const std::vector<cv::Rect> &faces,const cv::Rect &face_region,const double &distance);
	
	uint8_t state;
public:

	cv::Rect ref;
	cv::Rect face_ref;
	cv::Rect prev_face;

	Controller_Com *Com;
	PIDcontroller controller_x;
	PIDcontroller controller_degree;
	uint8_t need_to_center;
	uint8_t need_to_zoom;
	uint8_t need_to_adjust;
	
	double img_exp_dis;

	Motion_controller();
	~Motion_controller();

	uint8_t init();
	//this function will only be called when a target is detected by the Image processor
	uint8_t evaluate_image(const cv::Rect &detect,const cv::Rect &face,const double &distance);
	uint8_t evaluate_image_multi_targets(const std::vector<cv::Rect> &faces,const cv::Rect &face_region, const double &distance);
	uint8_t evaluate_image_tracking(const cv::Rect &face,const double &distance);

	void set_pattern(uint8_t pattern);
	void set_pattern_diy(int32_t ratiox,int32_t ratioy,int32_t ratiowidth);
	void move(const uint16_t &mm,const uint8_t &dir);
	void rotate(const uint16_t &degree,const uint8_t &dir);


	//lifter related functions
	void reset_lifter();
	void set_lifter(const uint16_t &mm);
	void lift(const uint16_t &mm, const uint8_t &dir);

	//Buzzer related functions
	void buzzer(const uint8_t &type);

	//Platform related functions
	void platform_init();

	void platform(const uint16_t &degree, const uint8_t &action);

	int8_t orientation_adjust(const uint16_t &phone_ori);
	void set_initial_car_orientation(const uint16_t &car_ori);
};


void degree_rotation(int32_t car,int32_t phone,int32_t *degree, int32_t *direction);
#endif
