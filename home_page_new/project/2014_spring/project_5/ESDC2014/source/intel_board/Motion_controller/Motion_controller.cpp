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
 
#include "Motion_controller.h"

Motion_controller::Motion_controller()
{
	printf("Motion_controller(): Constructing new Motion_controller\n");


	//initialize the states value
	this->lifter_pos = 0;
	this->car_orientation = 0;
	this->car_original_orientation = 0;
	this->phone_orientation = 0;
	this->state = EVAL_INIT;
	//initialize threshold value
	this->threshold_x = IMG_HORI_THRESHOLD;
	this->threshold_y = IMG_VERT_THRESHOLD;
	this->threshold_face_x = IMG_HORI_THRESHOLD_FACE;
	this->threshold_face_y = IMG_VERT_THRESHOLD_FACE;
	
	printf("Motion_controller(): Threshold_x %u Threshold_y %u\n",this->threshold_x,this->threshold_y);
	printf("Motion_controller(): Threshold_face_x %u Threshold_face_y %u\n",this->threshold_face_x,this->threshold_face_y);

	this->center_x = IMG_CENTER_X;
	this->center_y = IMG_CENTER_Y;

	//initilize the body reference region
	this->img_exp_pos_x = IMG_EXP_POS1_X; //indicating the center of the region
	this->img_exp_pos_y = IMG_EXP_POS1_Y;
	this->exp_width = IMG_EXP_WIDTH;
	this->exp_height = IMG_EXP_HEIGHT;

	//initilize the face reference region
	this->img_exp_face_pos_x = IMG_EXP_FACE_POS_X; //indicating the center of the region
	this->img_exp_face_pos_y = IMG_EXP_FACE_POS_Y;
	this->exp_face_height = IMG_EXP_FACE_HEIGHT;
	this->exp_face_width = IMG_EXP_FACE_WIDTH;

	this->Com = new Controller_Com("/dev/ttyUSB0");
	

	this->need_to_center = 1;
	this->need_to_zoom = 1;
	this->need_to_adjust = 1;
	this->adjusting_counter = 0;
	//TODO: initilized the reference rectangle

	this->ref = cv::Rect(
		this->img_exp_pos_x - this->exp_width/2,
		this->img_exp_pos_y,
		this->exp_width,
		this->exp_height);
	
	this->face_ref = cv::Rect(
		this->img_exp_face_pos_x - this->exp_face_width/2,
		this->img_exp_pos_y,
		this->exp_face_width,
		this->exp_face_height);
}

Motion_controller::~Motion_controller()
{
	this->set_lifter(LIFTER_MIN);
	delete this->Com;
}

uint8_t Motion_controller::init()
{
	printf("Motion_controller::init() running\n");
	this->reset_lifter();
	printf("Motion_controller::init() returning\n");
	this->controller_degree.setPID(CONTROLLER_DEGREE_P,CONTROLLER_DEGREE_I,CONTROLLER_DEGREE_D);
	return 1;
}


uint8_t Motion_controller::evaluate_image(const cv::Rect &detect,const cv::Rect &face,const double &distance)
{

/*
	detect: the detected body region
	face: the detected face region
	threshold_x, threshold_y: the threshold for largest horizontally and vertically
	center_x, center_y: the center of the image
	img_exp_pos_x, img_exp_pos_y: the expected (top left) position  of the detected region
	exp_width, exp_height: the expected width and height
	diff_x: the difference between the detected region center and the image center -> cause centering
	diff_y: the difference between the actual detected region height and the expected region height -> cause backward and forward
*/
	char line[256];
	strcpy(line,"Motion_controller::evaluate_image() state ");
	switch(this->state)
	{
		case(EVAL_INIT):
			strcat(line,"EVAL_INIT");
		break;
		case(EVAL_CENTERING):
			strcat(line,"EVAL_CENTERING");
		break;
		case(EVAL_ZOOM_IN):
		case(EVAL_ZOOM_OUT):
			strcat(line,"EVAL_ZOOMING");
		break;
		case(EVAL_ADJUSTING):
			strcat(line,"EVAL_ADJUSTING");
		break;
		case(EVAL_COMPLETE):
			strcat(line,"EVAK_COMPLETE");
		break;
		default:
		break;
	}
	printf("%s\n",line);

	if(glo_test_filetransfer)
		return EVAL_COMPLETE;
	
	if(glo_tracking)
	{
		return this->evaluate_image_tracking(face,distance);
	}


	//find out the center of the detected region
	this->prev_face = face;
	cv::Point center(detect.x + detect.width / 2,detect.y + detect.height / 2);
	int32_t diff_x = center.x - this->center_x;
	int32_t diff_y = detect.height - this->ref.height;
	if(glo_debug_msg)
	{
		printf("Motion_controller::evaluate_image center.x %d center_x %d\n",center.x,this->center_x);
		printf("Motion_controller::evaluate_image detect.height %d, exp_height%d\n",detect.height,this->ref.height);
		printf("Motion_controller::evaluate_image the distance is %lf\n",distance);
		printf("Motion_controller::evaluate_image the final exp pos_x is %u\n",this->img_exp_pos_x);
		printf("Motion_controller::evaluate_image the face_height is %d and the face_ref is %u\n",face.height,this->face_ref.height);
		printf("Motion_controller::evaluate_image the y threshold_face_y is %u\n",threshold_face_y);
	}

	uint8_t flag_done_centering = 1;
	uint8_t flag_done_zooming = 0;
	diff_y = face.height - this->face_ref.height;

	if(!(glo_waist_shot))
	{
		diff_y = face.height - this->face_ref.height;
		//if not taking waist shot

		if(abs(diff_x) > threshold_x && this->state < EVAL_ADJUSTING)//need to adjust horizontally to the center
		{
			//doing centering
			this->centering(detect,face);
			flag_done_centering = 0;
		}
		else
			flag_done_centering = 1;	
		

		if(abs(diff_y) > FACE_HEIGHT_DIFF_THRESHOLD_SMALL && this->state < EVAL_ADJUSTING)//the body is too small or too large need to zoom in or zoom out
		{
			//doing zooming 
			this->img_exp_dis = runCAMShift(this->face_ref);
			return (this->state = this->zoom_in_out_by_distance(this->img_exp_dis,distance));
			return (this->state = this->zoom_in_out_by_face(face,distance));
		}
		else if(flag_done_centering == 0 && this->state < EVAL_ADJUSTING)
		{
			this->state = EVAL_CENTERING;
			return EVAL_CENTERING;
		}
		else
			flag_done_zooming = 1;
		
		if(glo_debug_msg)
		{
			printf("Motion_controller::evaluate_image() center.x %d (this->ref.x + this->ref.width / 2) %d, detect.y %d this->ref.y %u\n",center.x,this->ref.x + this->ref.width / 2,detect.y,this->ref.y);
			printf("Motion_controller::evaluate_image() threshold_x %u, threshold_y %u\n",this->threshold_x,this->threshold_y);
			printf("Motion_controller::flag_done_centering %u, flag_done_zooming %u\n",flag_done_centering,flag_done_zooming);
		}

		if((abs(center.x - (this->ref.x + this->ref.width / 2)) > this->threshold_x || abs(detect.y - this->ref.y) > this->threshold_y))
		{
			if(this->adjusting_counter >= MAX_ADJUST_NUM)
			{
				this->adjusting_counter = 0;
				this->state = EVAL_INIT;
				return EVAL_COMPLETE;
			}

			this->adjusting_counter++;
			this->adjusting(detect);
			this->state = EVAL_ADJUSTING;
			return EVAL_ADJUSTING;
		}
		else if(abs(center.x - (this->ref.x + this->ref.width / 2)) < threshold_x && abs(detect.y - this->ref.y) < threshold_y && (flag_done_centering && flag_done_zooming))
		{
			this->adjusting_counter = 0;
			this->state = EVAL_INIT;
			return EVAL_COMPLETE;
		}
	}
	else
	{
		uint8_t flag_done_centering = 1;
		uint8_t flag_done_zooming = 0;
		//if taking waist shot
		diff_y = face.height - this->face_ref.height;

		
		if(abs(diff_x) > threshold_x && need_to_center && this->state < EVAL_ADJUSTING)
		{
			//this->centering(detect,face);
			this->centering_by_face(face);
			flag_done_centering = 0;
		}
		else
			flag_done_centering = 1;
		

		if(abs(diff_y) > FACE_HEIGHT_DIFF_THRESHOLD_LARGE && this->state < EVAL_ADJUSTING)//the face is too small or too large, need to zoom in or zoom out
		{
			//this->need_to_center = 0;
			this->img_exp_dis = runCAMShift(this->face_ref);
			return (this->state = this->zoom_in_out_by_distance(this->img_exp_dis,distance));
			return (this->state = this->zoom_in_out_by_face(face,distance));
		}else if(flag_done_centering == 0&& this->state < EVAL_ADJUSTING )
		{
			this->state = EVAL_CENTERING;
			return EVAL_CENTERING;
		}
		else
			flag_done_zooming = 1;

		diff_x = (face.x + face.width / 2) - (this->face_ref.x + this->face_ref.width / 2); 
		diff_y = (face.y) - this->face_ref.y;
		if((abs(diff_x) > threshold_face_x || abs(diff_y) > threshold_face_y))
		{
			if(this->adjusting_counter >= MAX_ADJUST_NUM)
			{
				this->adjusting_counter = 0;
				this->state = EVAL_INIT;
				return EVAL_COMPLETE;
			}
			this->need_to_center = 1;
			this->adjusting_by_face(face);
			this->state = EVAL_ADJUSTING;
			return EVAL_ADJUSTING;
		}
		else if(abs(diff_x) < threshold_face_x && abs(diff_y) < threshold_face_y && flag_done_centering && flag_done_zooming)
		{
			this->adjusting_counter = 0;
			this->state = EVAL_INIT;
			return EVAL_COMPLETE;
		}
	}
	this->state = EVAL_INIT;
	return EVAL_COMPLETE;
}

uint8_t Motion_controller::evaluate_image_tracking(const cv::Rect &face,const double &distance)
{
	double mm_per_pixel = abs(IMG_FACE_ACTUAL_HEIGHT / (double) face.height);
	int32_t diff_x = (face.x + face.width / 2) - IMG_CENTER_X;
	double diff_x_actual = diff_x * mm_per_pixel;
	if(glo_debug_msg)
	{
		printf("Motion_controller::evaluate_image_tracking running\n");
		printf("Motion_controller::evaluate_image_tracking mm per pixel is %lf\n",mm_per_pixel);
		printf("Motion_controller::evaluate_image_tracking() detected faces at (%u), IMG_CENTER_X is %u\n",face.x,IMG_CENTER_X);
		printf("Motion_controller::evaluate_image_tracking() diff_x ix %d \n",diff_x);
	}

	if(abs(diff_x) > IMG_TRACKING_THRESHOLD)
	{

		uint16_t degree = 0;
		int16_t degree_error = atan(diff_x_actual / distance) / PI * 180;
		int16_t move_degree = (int16_t) this->controller_degree.run(degree_error);

		if(glo_debug_msg)
		{
			printf("Motion_controller::evaluate_image_tracking diff_x_actual is %lf, distance is %lf\n",diff_x_actual,distance);
			printf("Motion_controller::evaluate_image_tracking() original degree_error is %d, calculated degree is %d\n",degree_error,move_degree);
		}
		uint8_t dir = CAM_YAW_LEFT;
		if(glo_pid)
		{
			if(glo_tracking_by_rotate)
			{
				if(move_degree > 0)
					dir = CAR_ROTATE_RIGHT;
				else
					dir = CAR_ROTATE_LEFT;
			}
			else
			{
				if(move_degree > 0)
					dir = CAM_YAW_RIGHT;
				else
					dir = CAM_YAW_LEFT;
			}
			degree = (uint16_t) abs(move_degree);
		}
		else
		{
			if(glo_tracking_by_rotate)
			{
				if(degree_error > 0)
					dir = CAR_ROTATE_RIGHT;
				else
					dir = CAR_ROTATE_LEFT;
			}
			else
			{
				if(degree_error > 0)
					dir = CAM_YAW_RIGHT;
				else
					dir = CAM_YAW_LEFT;
			}
			degree = (uint16_t) abs(degree_error);
		}
		printf("Motion_controller::evaluate_image_tracking -> degree is %u\n",degree);
		
		if(glo_tracking_by_rotate && degree > CAR_ROTATE_THRESHOLD)
			this->rotate(degree,dir);
		else
			this->platform(degree,dir);
	}
	printf("Motion_controller::evaluate_image_tracking exiting\n");
	return EVAL_CENTERING;
}

uint8_t Motion_controller::evaluate_image_multi_targets(const std::vector<cv::Rect> &faces,const cv::Rect &face_region, const double &distance)
{
	uint8_t centering_done = this->multi_face_centering(faces,face_region);
	uint8_t zooming_done = this->multi_face_zooming(faces,face_region,distance);

	if(centering_done == EVAL_COMPLETE &&  zooming_done == EVAL_COMPLETE)
		return EVAL_COMPLETE;
	else if(zooming_done != EVAL_COMPLETE)
		return zooming_done;
	else if(centering_done != EVAL_COMPLETE)
		return centering_done;
	else
		return EVAL_INIT;//case should not happen
}
/*CENTERING FUNCTION BEGIN*/
/*CENTERING FUNCTION BEGIN*/
/*CENTERING FUNCTION BEGIN*/
uint8_t Motion_controller::multi_face_centering(const std::vector<cv::Rect> &faces,const cv::Rect &face_region)
{
	double mm_per_pixel = IMG_FACE_ACTUAL_HEIGHT/(double)faces[0].height;
	int32_t diff_x = face_region.x + face_region.width / 2 - IMG_CENTER_X;
	if(glo_debug_msg)
		printf("Motion_controller::multi_face_centering() length per pixel is %f\n",mm_per_pixel);
	uint16_t move_x = 0;
	double factor;

	if(glo_num_target == 2)
		factor = 1.4;
	else if(glo_num_target == 3)
		factor = 1.8;
	double current_error_x = diff_x * mm_per_pixel;
	double action = controller_x.run(current_error_x);
	if(glo_debug_msg)
		printf("Motion_controller::multi_face_centering() raw error is %lf, PID calculated error is %lf\n",current_error_x,action);

	if(abs(diff_x) > threshold_face_x * factor)
	{
		if(glo_debug_msg)
			printf("Motion_controller multi_face_centering(): diff_x is %d\n",diff_x);
		if(glo_pid)
		{
			move_x = (uint16_t) abs(action);
			if(action < 0)
			{
				if(glo_debug_msg)
					printf("\n\n\nMotion_controller multi_face_centering(): moving left %d mm\n\n\n",move_x);
				this->move(move_x,CAR_LEFT);
			}
			else
			{
				if(glo_debug_msg)
					printf("\n\n\nMotion_controller multi_face_centering(): moving right %d mm\n\n\n",move_x);
				this->move(move_x,CAR_RIGHT);
			}
		}
		else
		{
			move_x = this->bound_dis(ceil(abs(diff_x) * mm_per_pixel));
			if(diff_x < 0)
			{	
				if(glo_debug_msg)
					printf("\n\n\nMotion_controller multi_face_centering(): moving left %d mm\n\n\n",move_x);
				this->move(move_x,CAR_LEFT);
			}
			else
			{
				if(glo_debug_msg)
					printf("\n\n\nMotion_controller multi_face_centering(): moving right %d mm\n\n\n",move_x);
				this->move(move_x,CAR_RIGHT);
			}
		}
		return EVAL_CENTERING;
	}
	return EVAL_COMPLETE;
}

uint8_t Motion_controller::centering(const cv::Rect &detect,const cv::Rect &face)
{
	if(glo_debug_msg)
		printf("\nMotion_controller::centering() running\n");
	uint8_t okay_image = 1;

	cv::Point center(detect.x + detect.width / 2,detect.y + detect.height / 2);

	int32_t diff_x = center.x - this->center_x;
	int32_t diff_y = center.y - this->center_y;
	//compute length per pixel 
	double p = (double) IMG_BODY_ACTUAL_HEIGHT / (double) detect.height;
	if(glo_debug_msg)
		printf("\n\nMotion_controller::centering() length per pixel is %f.\n",p);
	uint16_t move_x = 0;

	if(abs(diff_x) > threshold_x)
	{
		okay_image = 0;
		//movement to right or left
		
		double current_error_x = diff_x * p;
		double action = controller_x.run(current_error_x);
		if(glo_debug_msg)
		{
			printf("Motion_controller centering(): diff_x is %d\n",diff_x);
			printf("Motion_controller centering(): raw error is %lf, PID calculated error is %lf\n",current_error_x,action);
		}

		if(glo_pid)
		{
			move_x = (uint16_t) abs(action);
			//using pid 
			if(action < 0)
			{
				printf("\n\n\nMotion_controller centering(): moving left %u mm\n\n\n",move_x);
				this->move(move_x,CAR_LEFT);
			}
			else
			{
				printf("\n\n\nMotion_controller centering(): moving right %u mm\n\n\n",move_x);
				this->move(move_x,CAR_RIGHT);
			}
		}
		else
		{
			//not using pid
			if(diff_x < 0)
			{	
				//should move left
				move_x = this->bound_dis(ceil(abs(diff_x) * p));
				//move_x = (uint16_t) action;
				if(glo_debug_msg)
					printf("\n\n\nMotion_controller centering(): moving left %u mm\n\n\n",move_x);
				this->move(move_x,CAR_LEFT);
			}
			else
			{
				//should move right
				move_x = this->bound_dis(ceil(abs(diff_x) * p));
				//move_x = (uint16_t) action;
				if(glo_debug_msg)
					printf("\n\n\nMotion_controller centering(): moving right %u mm\n\n\n",move_x);
				this->move(move_x,CAR_RIGHT);
			}
		}

	}

	diff_y = detect.y - img_exp_pos_y;

	uint16_t move_y = ceil(abs(diff_y) * p);
	if(glo_debug_msg)
	{
		printf("Motion_controller centering(): detect.y is %d\n",detect.y);
		printf("Motion_controller centering(): img_exp_pos_y is %d\n",img_exp_pos_y);
		printf("Motion_controller centering(): length per pixel is %f, diff_y is %d\n",p,diff_y);
	}
	if(diff_y > 0)
	{
		//moving down
		if(glo_debug_msg)
			printf("\n\n\nMotion_controller centering(): moving down %d mm\n\n\n",move_y);
		this->lift(move_y,LIFTER_DOWN);
	}
	else
	{
		//moving up
		if(glo_debug_msg)
			printf("\n\n\nMotion_controller centering(): moving up %d mm\n\n\n",move_y);
		this->lift(move_y,LIFTER_UP);
	}
	return okay_image;
}

uint8_t Motion_controller::centering_by_face(const cv::Rect &face)
{
	double p = (double) IMG_FACE_ACTUAL_HEIGHT / (double) face.height;// mm per pixel

	int32_t diff_x = (face.x + face.width / 2) - this->center_x;
	uint16_t move_x = abs(ceil(p * diff_x));

	double current_error_x = diff_x * p;
	double action = controller_x.run(current_error_x);
	if(glo_debug_msg)
	{
		printf("Motion_controller::centering_by_face() raw error is %lf, PID calculated error is %lf\n",current_error_x,action);
		printf("Motion_controller::centering_by_face(): diff_x is %d\n",diff_x);
		printf("Motion_controller::centering_by_face(): mm per pixel is %f\n",p);
	}

	if(glo_pid)
	{
		move_x = (uint16_t) abs(action);
		if(action > 0)
		{
			if(glo_debug_msg)
			printf("Motion_controller::centering_by_face(): moving right by %u mm\n",move_x);
			this->move(move_x,CAR_RIGHT);
		}
		else
		{
			if(glo_debug_msg)
			printf("Motion_controller::centering_by_face(): moving left by %u mm\n",move_x);
			this->move(move_x,CAR_LEFT);
		}
	}
	else
	{
		if(diff_x > 0)
		{
			if(glo_debug_msg)
			printf("Motion_controller::centering_by_face(): moving right by %u mm\n",move_x);
			this->move(move_x,CAR_RIGHT);
		}
		else
		{
			if(glo_debug_msg)
			printf("Motion_controller::centering_by_face() : moving left by %u mm\n",move_x);
			this->move(move_x,CAR_LEFT);
		}
	}
	return EVAL_CENTERING;
}

/*CENTERING FUNCTION END*/
/*CENTERING FUNCTION END*/
/*CENTERING FUNCTION END*/

uint16_t Motion_controller::bound_dis(const uint32_t &dis)
{
	if(dis > 0x0000ffff)
		return 0xffff;
	else
		return (uint16_t) dis;
}

/*ZOOM IN OUT FUNCTION IMPLEMENTATION START*/
/*ZOOM IN OUT FUNCTION IMPLEMENTATION START*/
/*ZOOM IN OUT FUNCTION IMPLEMENTATION START*/

uint8_t Motion_controller::zoom_in_out(const cv::Rect &detect,const double &distance)
{
	if(glo_debug_msg)
	{
		printf("\nMotion_controller::zoom_in_out() running\n");
		printf("Motion_controller::zoom_in_out() the target distance is %lf\n",distance);
	}
	//return this->zoom_in_out_by_default(detect,distance);
	this->img_exp_dis = runCAMShift(this->face_ref);
	return this->zoom_in_out_by_distance(this->img_exp_dis,distance);
}

uint8_t Motion_controller::zoom_in_out_by_default(const cv::Rect &detect,const double &distance)
{
	//the car need to adjust the position according to the detection result
	int32_t diff_y = detect.height - this->ref.height;
	uint16_t move_z = 0;
	if(distance > 3000)
		move_z = DEFAULT_DIS_LARGE * (distance / 3000);
	else if(distance > 1500)
		move_z = DEFAULT_DIS;
	else if(distance > 1000)
		move_z = DEFAULT_DIS_SMALL * (distance / 1500);
	else 
		move_z = DEFAULT_DIS_TINY * (distance / 1000);

	if(diff_y < 0)
	{
		//the height too small, need to zoom in
		//to zoom in, move forward
		printf("Motion_controller::zoom_in_out() moving forward %u mm\n",move_z);
		this->move(move_z,CAR_FORWARD);
		return EVAL_ZOOM_IN;
	}
	else
	{
		//the height too large, need to zoom out
		//to zoom out, move backward
		printf("Motion_controller::zoom_in_out() moving backward %u mm\n",move_z);
		this->move(move_z,CAR_BACKWARD);
		return EVAL_ZOOM_OUT;
	}
}

uint8_t Motion_controller::zoom_in_out_by_distance(const double &exp_distance,const double &distance)
{
	printf("Motion_controller::zoom_in_out_by_distance() running\n");
	if(glo_debug_msg)
	{
		printf("Motion_controller::zoom_in_out_by_distance() the target distance is %lf\n",distance);
		printf("Motion_controller::zoom_in_out_by_distance() the exp_distance is %lf\n",exp_distance);
		printf("Motion_controller::zoom_in_out_by_distance() the diff is %lf\n",distance - img_exp_dis);
		printf("Motion_controller::zoom_in_out_by_distance() the abs diff is %lf\n", abs(distance - img_exp_dis));
		printf("Motion_controller::zoom_in_out_by_distance() the ceil abs diff is %lf\n",ceil(abs(distance - img_exp_dis)));
	}
	if(distance > exp_distance)
	{
		//the target too far away from the camera
		uint16_t move_z = ceil(abs(distance - exp_distance));
		if(glo_debug_msg)
		printf("Motion_controller::zoom_in_out() moving forward %u\n",move_z);
		this->move(move_z,CAR_FORWARD);
		return EVAL_ZOOM_IN;
	}
	else
	{
		//the target too close to the camera
		uint16_t move_z = ceil(abs(distance - exp_distance));
		if(glo_debug_msg)
		printf("Motion_controller::zoom_in_out() moving backward %u\n",move_z);
		this->move(move_z,CAR_BACKWARD);
		return EVAL_ZOOM_OUT;
	}
}

uint8_t Motion_controller::zoom_in_out_by_face(const cv::Rect &face,const double &distance)
{
	int32_t diff_y = face.height - this->face_ref.height;
	uint16_t move_z = DEFAULT_DIS;
	printf("Motion_controller::zoom_in_out_by_face() running\n");
	
	if(distance > 3000)
		move_z = DEFAULT_DIS_LARGE * (distance / 3000);
	else if(distance > 1500)
		move_z = DEFAULT_DIS;
	else if(distance > 1000)
		move_z = DEFAULT_DIS_SMALL * (distance / 1500);
	else
		move_z = DEFAULT_DIS_TINY * (distance / 1000);

	if(diff_y > 0)
	{
		//the face is too large
		printf("Motion_controller::zoom_in_out_by_face: moving backward %u mm, face detect size(%d,%d)\n",move_z,face.width,face.height);
		this->move(move_z,CAR_BACKWARD);
		printf("Motion_controller::zoom_in_out_by_face exiting\n");
		return EVAL_ZOOM_IN;
	}
	else
	{
		//the face is too small
		printf("Motion_controller::zoom_in_out_by_face: moving forward %u mm, face detect size(%d,%d)\n",move_z,face.width,face.height);
		this->move(move_z,CAR_FORWARD);
		printf("Motion_controller::zoom_in_out_by_face exiting\n");
		return EVAL_ZOOM_OUT;
	}
}

uint8_t Motion_controller::multi_face_zooming(const std::vector<cv::Rect> &faces,const cv::Rect &face_region,const double &distance)
{
	uint16_t move_z = DEFAULT_DIS;
	if(glo_waist_shot)
	{
		cv::Rect standard(faces[0].x,faces[0].y,IMG_EXP_FACE_WIDTH,IMG_EXP_FACE_HEIGHT);	
		if(abs(faces[0].height - standard.height) > threshold_face_y)
		{
			this->img_exp_dis = runCAMShift(standard);
			return this->zoom_in_out_by_distance(this->img_exp_dis,distance);
		}
		else
			return EVAL_COMPLETE;
	}
	else
	{
		cv::Rect standard(faces[0].x,faces[0].y,IMG_EXP_FACE_WIDTH_FULL,IMG_EXP_FACE_HEIGHT_FULL);
		if(abs(faces[0].height - standard.height) > threshold_face_y)
		{
			this->img_exp_dis = runCAMShift(standard);
			return this->zoom_in_out_by_distance(this->img_exp_dis,distance);
		}
		else
			return EVAL_COMPLETE;
	}

	return EVAL_COMPLETE;
}
/*ZOOM IN OUT FUNCTION IMPLEMENTATION END*/
/*ZOOM IN OUT FUNCTION IMPLEMENTATION END*/
/*ZOOM IN OUT FUNCTION IMPLEMENTATION END*/

/*ADJUSTING FUNCTION IMPLEMENTATION START*/
/*ADJUSTING FUNCTION IMPLEMENTATION START*/
/*ADJUSTING FUNCTION IMPLEMENTATION START*/
uint8_t Motion_controller::adjusting(const cv::Rect &detect)
{
/*
	diff_x is the difference between the detected body region center's x coordinate, and the expected position's x coordinate
	diff_y is the difference between the detected body region's top left y coordinate, and the expected position's y coordinate
	p is the length per pixel, assuming that every detected region's height is IMG_BODY_ACTUAL_HEIGHT mm 
*/
	if(glo_debug_msg)
	printf("Motion_controller::adjusting() running\n");
	int32_t diff_x = (detect.x + detect.width/2) - img_exp_pos_x;
	double p = (double) IMG_BODY_ACTUAL_HEIGHT / (double) detect.height;
	if(diff_x > 0)
	{
		//move right
		uint16_t move_x = this->bound_dis(ceil(abs(diff_x) * p));
		if(glo_debug_msg)
		printf("\n\n\nMotion_controller adjusting(): moving right %d mm\n\n\n",move_x);
		this->move(move_x,CAR_RIGHT);
	}
	else
	{
		//move left
		uint16_t move_x = this->bound_dis(ceil(abs(diff_x) * p));
		if(glo_debug_msg)
		printf("\n\n\nMotion_controller adjusting(): moving left %d mm\n\n\n",move_x);
		this->move(move_x,CAR_LEFT);
	}

	int32_t diff_y = detect.y - img_exp_pos_y;
	uint16_t move_y = ceil(abs(diff_y) * p);
	if(glo_debug_msg)
	{
		printf("Motion_controller adjusting(): detect.y is %d\n",detect.y);
		printf("Motion_controller adjusting(): img_exp_pos_y is %d\n",img_exp_pos_y);
		printf("Motion_controller adjusting(): length per pixel is %f, diff_y is %d\n",p,diff_y);
	}
	if(diff_y > 0 && glo_high_angle_shot == 0)
	{
		//moving down
		if(glo_debug_msg)
		printf("\n\n\nMotion_controller adjusting(): moving down %d mm\n\n\n",move_y);
		this->lift(move_y,LIFTER_DOWN);
	}
	else if(glo_high_angle_shot == 0 && diff_y <= 0)
	{
		//moving up
		if(glo_debug_msg)
		printf("\n\n\nMotion_controller adjusting(): moving up %d mm\n\n\n",move_y);
		this->lift(move_y,LIFTER_UP);
	}
	return 1;
}

uint8_t Motion_controller::adjusting_by_face(const cv::Rect &face)
{
	printf("Motion_controller::adjusting_by_face() running\n");
	cv::Point face_top(face.x + face.width / 2, face.y);
	double p = (double) IMG_FACE_ACTUAL_HEIGHT / (double)face.height; // mm per pixel


	//move horizontally
	//diff_x is the difference between the center of the face and the center of the ref region
	int32_t diff_x = face_top.x - (this->face_ref.x + this->face_ref.width / 2);
	if(glo_debug_msg)
	{
		//printf("Motion_controller::adjusting_by_face() running\n");
		printf("Motion_controller::adjusting_by_face() face.height is %u mm per pixel is %lf\n",face.height,p);
		printf("Motion_controller::adjusting_by_face() face_top x is %u\n",face_top.x);
		printf("Motion_controller::adjusting_by_face() face_ref center x is %u\n",this->face_ref.x + this->face_ref.width / 2);
	}
	uint16_t move_x , move_y;
	
	move_x = abs(ceil(p * diff_x)) * 0.8;
	if(diff_x > 0 && abs(diff_x) > threshold_face_x)
	{
		//the target is on the right w.r.t the expected region
		//move right
		if(glo_debug_msg)
		printf("Motion_controller::adjusting_by_face() moving right %u mm\n",move_x);
		this->move(move_x,CAR_RIGHT);
	}
	else if(abs(diff_x) > threshold_face_x)
	{
		//the target is on the left w.r.t the expected region
		//move left
		if(glo_debug_msg)
		printf("Motion_controller::adjusting_by_face() moving left %u mm\n",move_x);
		this->move(move_x,CAR_LEFT);
	}
	
	//move vertically
	int32_t diff_y = face_top.y - this->face_ref.y;
	if(glo_debug_msg)
	{
		printf("Motion_controller::adjusting_by_face() face_top y is %u\n",face_top.y);
		printf("Motion_controller::adjusting_by_face() face_ref y is %u\n",this->face_ref.y);
	}
	move_y = abs(ceil(p* diff_y)) * 0.8;
	if(diff_y > 0 && abs(diff_y) > this->threshold_face_y && glo_high_angle_shot == 0)
	{
		//the target is too low w.r.t the expected region
		//lower camera
		if(glo_debug_msg)
		printf("Motion_controller::adjusting_by_face() lowering lifter\n");
		this->lift(move_y,LIFTER_DOWN);
	}
	else if(abs(diff_y) > this->threshold_face_y && glo_high_angle_shot == 0)
	{
		//the target is too high w.r.t the expected region
		//raising camera
		if(glo_debug_msg)
		printf("Motion_controller::adjusting_by_face() raising lifter\n");
		this->lift(move_y,LIFTER_UP);
	}

	if(glo_high_angle_shot)
	{
		this->lift((uint16_t) LIFTER_MAX,LIFTER_UP);
	}	
	if(glo_debug_msg)
	printf("Motion_controller::adjusting_by_face() exiting\n");
	return 1;
}

/*ADJUSTING FUNCTION IMPLEMENTATION END*/
/*ADJUSTING FUNCTION IMPLEMENTATION END*/
/*ADJUSTING FUNCTION IMPLEMENTATION END*/

void Motion_controller::set_pattern(uint8_t pattern)
{
	printf("Motion_controller::set_pattern() running\n");
	printf("Motion_controller::set_pattern() pattern is %u\n",pattern);
	switch(pattern)
	{
		case(1):
			this->img_exp_pos_x = this->img_exp_face_pos_x = IMG_EXP_POS1_X;
			this->img_exp_pos_y = this->img_exp_face_pos_y = IMG_EXP_POS1_Y;
			this->exp_face_width = this->exp_face_height = IMG_EXP_FACE_WIDTH_FULL;
		break;
		case(2):
			this->img_exp_pos_x = this->img_exp_face_pos_x = IMG_EXP_POS2_X;
			this->img_exp_pos_y = this->img_exp_face_pos_y = IMG_EXP_POS2_Y;
			this->exp_face_width = this->exp_face_height = IMG_EXP_FACE_WIDTH_FULL;
		break;
		case(3):
			this->img_exp_pos_x = IMG_EXP_POS3_X;
			this->img_exp_pos_y = IMG_EXP_POS3_Y;
			this->img_exp_face_pos_x = IMG_EXP_FACE_POS_X + 10;
			this->img_exp_face_pos_y = IMG_EXP_FACE_POS_Y;
			this->exp_face_width = this->exp_face_height = (IMG_EXP_FACE_WIDTH + 10);
		break;
		case(4):
			this->img_exp_pos_x = IMG_EXP_POS4_X;
			this->img_exp_pos_y = IMG_EXP_POS4_Y;
			this->img_exp_face_pos_x = IMG_EXP_FACE_POS2_X;
			this->img_exp_face_pos_y = IMG_EXP_FACE_POS2_Y;
			this->exp_face_width = this->exp_face_height = (IMG_EXP_FACE_WIDTH + 20);
		break;
		default:
			printf("Motion_controller::set_pattern() pattern undefined\n");
		break;
	}

	if(!(glo_waist_shot))
		printf("Motion_controller::set_pattern() the img_exp_pos_x is %u, img_exp_pos_y is %u\n",this->img_exp_pos_x,this->img_exp_pos_y);
	else
		printf("Motion_controller::set_pattern() the img_exp_face_pos_x is %u,img_exp_face_pos_y is %u\n",this->img_exp_face_pos_x,this->img_exp_face_pos_y);
	

	this->ref = cv::Rect(this->img_exp_pos_x - this->exp_width / 2,
			this->img_exp_pos_y,
			this->exp_width,
			this->exp_height);

	this->face_ref = cv::Rect(this->img_exp_face_pos_x - this->exp_face_width / 2,
			this->img_exp_face_pos_y,
			this->exp_face_width,
			this->exp_face_height);

	this->img_exp_dis = runCAMShift(this->face_ref);
	printf("Motion_controller::set_pattern() -> img exp dis %lf\n",this->img_exp_dis);
	printf("Motion_controller::set_pattern() exiting\n");
	return ;
}

void Motion_controller::set_pattern_diy(int32_t ratiox,int32_t ratioy,int32_t ratiowidth)
{
	printf("Motion_controller::set_pattern_diy() running\n");
	printf("Motion_controller::set_pattern_diy() parameters %d %d %d\n",ratiox,ratioy,ratiowidth);

	printf("Motion_controller::set_pattern_diy setting at (%u,%u,%u,%u)\n",(uint16_t) ratiox, (uint16_t) ratioy, (uint16_t) ratiowidth,(uint16_t) ratiowidth);
	this->face_ref = cv::Rect((uint16_t) ratiox,
		(uint16_t) ratioy,
		(uint16_t) ratiowidth,
		(uint16_t) ratiowidth);
	printf("Motion_controller::set_pattern_diy() exiting\n");
}

/*MOVEMENT FUNCTION START*/
/*MOVEMENT FUNCTION START*/
/*MOVEMENT FUNCTION START*/

void Motion_controller::reset_lifter()
{
	printf("Motion_controller::reset_lifter() running\n");
	Message msg;
	msg.LifterMoveDownMM(LIFTER_MAX);
	if(glo_motion_enable)
		msg.safe_sendMessage(this->Com->fd);

	msg.LifterMoveUpMM(LIFTER_INIT_POS);
	if(glo_motion_enable)
	msg.safe_sendMessage(this->Com->fd);

	this->lifter_pos = LIFTER_INIT_POS;
	printf("Motion_controller::reset_lifter() exiting\n");
}

void Motion_controller::lift(const uint16_t &mm, const uint8_t &dir)
{
	Message msg;
	if(glo_debug_msg)
	printf("Motion_controller::lift current at %u\n",this->lifter_pos);
	if(dir == LIFTER_UP)
	{
		if(this->lifter_pos + mm > LIFTER_MAX)
			this->lifter_pos = LIFTER_MAX;
		else
			this->lifter_pos += mm;
		if(glo_debug_msg)
		printf("Motion_controller::lift moving up to %u\n",this->lifter_pos);
		msg.LifterMoveUpMM(mm);
	}
	else if(dir == LIFTER_DOWN)
	{
		if(this->lifter_pos - mm < LIFTER_MIN)
			this->lifter_pos = LIFTER_MIN;
		else
			this->lifter_pos -= mm;
		if(glo_debug_msg)
		printf("Motion_controller::lift down up to %u\n",this->lifter_pos);
		msg.LifterMoveDownMM(mm);
	}
	if(glo_motion_enable)
		msg.safe_sendMessage(this->Com->fd);
}

void Motion_controller::set_lifter(const uint16_t &mm)
{
	if(mm > this->lifter_pos)
	{
		this->lift(mm - this->lifter_pos,LIFTER_UP);
	}
	else
	{
		this->lift(this->lifter_pos - mm,LIFTER_DOWN);
	}
}

void Motion_controller::move(const uint16_t &mm,const uint8_t &dir)
{
	uint16_t segment = 1500;
	Message msg;
	uint16_t dis = 0;
	uint8_t count = mm / segment;
	uint8_t count_msg = 0;
	if(mm < CAR_MOVEMENT_THRESHOLD)
	{
		if(glo_debug_msg)
		printf("Motion_controller::move() -> the distance is too small\n");
		return ;
	}
	switch(dir)
	{
		case CAR_FORWARD://forward
			if(mm > segment)
			{
				for(count_msg = 0;count_msg < count;count_msg++)
				{
					msg.CarMoveUpMM(segment);
					if(glo_motion_enable)
						msg.safe_sendMessage(this->Com->fd);
				}
				dis = mm - count * segment;
				msg.CarMoveUpMM(dis);
				if(glo_motion_enable)
					msg.safe_sendMessage(this->Com->fd);
			}
			else
			{
				msg.CarMoveUpMM(mm);
				if(glo_motion_enable)
					msg.safe_sendMessage(this->Com->fd);
			}
		break;
			
		case CAR_BACKWARD://backward
			if(mm > segment)
			{
				for(count_msg = 0;count_msg < count;count_msg++)
				{
					msg.CarMoveDownMM(segment);
					if(glo_motion_enable)
						msg.safe_sendMessage(this->Com->fd);
				}
				dis = mm - count * segment;
				msg.CarMoveDownMM(dis);
				if(glo_motion_enable)
					msg.safe_sendMessage(this->Com->fd);
			}
			else
			{
				msg.CarMoveDownMM(mm);
				if(glo_motion_enable)
					msg.safe_sendMessage(this->Com->fd);
			}
		break;

		case CAR_LEFT://left
			if(mm > segment)
			{
				for(count_msg = 0;count_msg < count;count_msg++)
				{
					msg.CarMoveLeftMM(segment);
					if(glo_motion_enable)
						msg.safe_sendMessage(this->Com->fd);
				}
				dis = mm - count * segment;
				msg.CarMoveLeftMM(dis);
				if(glo_motion_enable)
					msg.safe_sendMessage(this->Com->fd);
			}
			else
			{
				msg.CarMoveLeftMM(mm);
				if(glo_motion_enable)
					msg.safe_sendMessage(this->Com->fd);
			}
		break;

		case CAR_RIGHT://right
			if(mm > segment)
			{
				for(count_msg = 0;count_msg < count;count_msg++)
				{
					msg.CarMoveRightMM(segment);
					if(glo_motion_enable)
						msg.safe_sendMessage(this->Com->fd);
				}
				dis = mm - count * segment;
				msg.CarMoveRightMM(dis);
				if(glo_motion_enable)
					msg.safe_sendMessage(this->Com->fd);
			}
			else
			{
				msg.CarMoveRightMM(mm);
				if(glo_motion_enable)
					msg.safe_sendMessage(this->Com->fd);
			}
		break;

		default:
		break;
	}
	return ;
}

int8_t Motion_controller::orientation_adjust(const uint16_t &phone_ori)
{
	uint16_t exp_orientation = phone_ori + 180;
	
	if(exp_orientation >= 360)
	{
		exp_orientation-=360;
	}

	int32_t car_degree = (int32_t) this->car_orientation;
	int32_t phone_degree = (int32_t) phone_ori;
	int32_t degree = 0, dir = 0;
	degree_rotation(car_degree,phone_degree,&degree,&dir);

	if(degree > ORIENTATION_THRESHOLD)
	{	
		if(dir > 0)
			this->rotate((uint16_t)degree,CAR_ROTATE_RIGHT);
		else
			this->rotate((uint16_t)degree,CAR_ROTATE_LEFT);
		return 1;
	}
	else
		return 0;
}

void Motion_controller::set_initial_car_orientation(const uint16_t &car_ori)
{
	printf("Motion_controller::setting initial car orientation as %u\n",car_ori);
	uint16_t initial_ori = car_ori + 180;
	if(initial_ori >= 360)
	{
		initial_ori-=360;
	}

	this->car_orientation = initial_ori;
	this->car_original_orientation = initial_ori;
}

void Motion_controller::rotate(const uint16_t &degree,const uint8_t &dir)
{
	int16_t target_degree = dir ? this->car_orientation - degree : this->car_orientation + degree;
	printf("Motion_controller::rotate start with degree %u, rotate %u, ",this->car_orientation,degree);

	if(target_degree >= 360)
		this->car_orientation = target_degree - 360;
	else if(target_degree < 0)
		this->car_orientation = target_degree + 360;
	else 
		this->car_orientation = target_degree;

	Message msg;
	if(dir == CAR_ROTATE_RIGHT)
	{
		printf("right ");
		msg.CarRotateRightDegree(degree);
	}
	else if(dir == CAR_ROTATE_LEFT)
	{
		printf("left ");
		msg.CarRotateLeftDegree(degree);
	}
	printf("to be %u\n",this->car_orientation);

	if(glo_motion_enable)
		msg.safe_sendMessage(this->Com->fd);
	return ;
}

/*MOVEMENT FUNCTION END*/
/*MOVEMENT FUNCTION END*/
/*MOVEMENT FUNCTION END*/

void Motion_controller::buzzer(const uint8_t &type)
{
	Message msg;
	msg.BuzzerRequest(type);
	msg.safe_sendMessage(this->Com->fd);
	return ;
}

void Motion_controller::platform_init()
{
	return ;
}

void Motion_controller::platform(const uint16_t &degree, const uint8_t &action)
{
	Message msg;
	switch(action)
	{
		case CAM_ROLL_LEFT:
			msg.CameraPlatformRollLeft(degree);
		break;
		case CAM_ROLL_RIGHT:
			msg.CameraPlatformRollRight(degree);
		break;
		case CAM_PITCH_UP:
			msg.CameraPlatformPitchUp(degree);
		break;
		case CAM_PITCH_DOWN:
			msg.CameraPlatformPitchDown(degree);
		break;
		case CAM_YAW_LEFT:
			msg.CameraPlatformYawCounterClk(degree);
		break;
		case CAM_YAW_RIGHT:
			msg.CameraPlatformYawClk(degree);
		break;
	}
	if(glo_motion_enable)
		msg.safe_sendMessage(this->Com->fd);
	return ;
}

void degree_rotation(int32_t car,int32_t phone,int32_t *degree,int32_t *direction)
{
	Direction phone_angle=null;
	Direction car_angle=null;

	int32_t rotate_degree=0;//postive means clockwise

	if(phone<=90)
		phone_angle=LOWER_LEFT;
	else if(phone<=180)
		phone_angle=UPPER_LEFT;
	else if(phone<=270)
		phone_angle=UPPER_RIGHT;
	else 
		phone_angle=LOWER_RIGHT;


	if(car<=90)
		car_angle=LOWER_LEFT;
	else if(car<=180)
		car_angle=UPPER_LEFT;
	else if(car<=270)
		car_angle=UPPER_RIGHT;
	else 
		car_angle=LOWER_RIGHT;


		switch(car_angle)
		{
			case UPPER_LEFT:
			if(phone_angle==LOWER_RIGHT)
			{
				int32_t temp_car=180-car;
				int32_t temp_phone=360-phone;
				rotate_degree=temp_car-temp_phone;
				break;
			}
			else if(phone_angle==LOWER_LEFT)
			{

				rotate_degree=(180-car)+phone;
				break;
			}

			else if(phone_angle==UPPER_LEFT)
			{

				rotate_degree=360-((180-phone)+car);
				break;
			}
			else
			{

				rotate_degree=360-(car-(phone-180));
				break;
			}

			case UPPER_RIGHT:

			if(phone_angle==LOWER_LEFT)
			{
				int32_t temp_car=car-180;
				int32_t temp_phone=phone;
				rotate_degree=temp_phone-temp_car;
				break;
			}
			else if(phone_angle==UPPER_LEFT)
			{

				int32_t temp_car=270-car;
				int32_t temp_phone=phone-90;
				rotate_degree=temp_phone+temp_car;
				break;
			}
			else if(phone_angle==UPPER_RIGHT)
			{

				int32_t temp_car=360-car;
				int32_t temp_phone=phone-180;/*int32_t temp_phone=phone is wrong*/
				rotate_degree=temp_phone+temp_car;
				break;
			}
			else
			{

				int32_t temp_car=car-180;
				int32_t temp_phone=360-phone;
				rotate_degree=360-(temp_phone+temp_car);
				break;
			}

			case LOWER_LEFT:
			if(phone_angle==UPPER_RIGHT)
			{
				int32_t temp_car=car;
				int32_t temp_phone=phone-180;
				rotate_degree=temp_phone-temp_car;
				break;
			}
			else if(phone_angle==LOWER_RIGHT)
			{
				int32_t temp_car=car;
				int32_t temp_phone=180-(360-phone);
				rotate_degree=temp_phone-temp_car;
				break;
			}
			else if(phone_angle==LOWER_LEFT)
			{
				int32_t temp_car=car;
				int32_t temp_phone=180+phone;
				rotate_degree=temp_phone-temp_car;
				break;
			}

			else 
			{
				int32_t temp_car=car;
				int32_t temp_phone=180-phone;
				rotate_degree=360-(temp_phone+temp_car);
				break;
			}

			case LOWER_RIGHT:
			if(phone_angle==UPPER_LEFT)
			{
				int32_t temp_car=360-car;
				int32_t temp_phone=180-phone;
				rotate_degree=temp_car-temp_phone;
				break;
			}
			else if(phone_angle==UPPER_RIGHT)
			{
				int32_t temp_car=360-car;
				int32_t temp_phone=phone-180;
				rotate_degree=temp_car+temp_phone;
				break;
			}
			else if(phone_angle==LOWER_RIGHT)
			{
				int32_t temp_car=360-car;
				int32_t temp_phone=180-(360-phone);
				rotate_degree=temp_car+temp_phone;
				break;
			}
			else
			{
				int32_t temp_car=360-car;
				int32_t temp_phone=phone+180;
				rotate_degree=temp_car+temp_phone;
				break;
			}


		}//end switch


	if((rotate_degree>=0)&&(rotate_degree<=180))
 	{
 			*degree=rotate_degree;
 			*direction=1;
 	}
 	else if((rotate_degree>=180)&&(rotate_degree<=360))
 	{
 		*degree=360-rotate_degree;
 		*direction=-1;
 	}
 	else if((rotate_degree>=-360)&&(rotate_degree<=-180))
 	{
 		*degree=360-abs(rotate_degree);
 		*direction=1;
 	}
 	else if((rotate_degree>=-180)&&(rotate_degree<=0))
 	{
 		*degree=abs(rotate_degree);
 		*direction=-1;
 	}


/*
 		int temp=180-car+phone;
 		if(temp<0)
 		{
 			*degree=abs(temp);
 			*direction=-1;
 		}
 		else if (temp<180)
 		{
 			*degree=temp;
 			*direction=1;
 		}
 		else if (temp<360)
 		{
 			*degree=360-temp;
 			*direction=-1;
 		}
 		else if (temp<540)
 		{
 			*degree=temp-360;
 			*direction=1;
 		}
*/

}//end void


