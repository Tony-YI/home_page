#ifndef _UI_H
#define _UI_H
#include "../macro.h"



#define UI_PORT 60000
#define TRANSFER_PORT 12000
#define MAX_MESSAGE_SIZE 255
#define MESSAGELENGTH 2
#define COMPASSLENGTH 

#define BUFFER_SIZE 8192 
#define MAX_SIZE 512 

#include <pthread.h>

extern command_type glo_pattern; 

class UI{

	public:

	//establish connection when class created
	UI();

	//close connection when class destroyed
	~UI();

	//return the updated compass degree
	int get_degree();

	int update_degree();
	
	int update_degree_without_confirm();

	
	//use this when 
	command_type wait_command();
	//if return is pattern_diy, then XXX

	/*

	send_finished_ack  fa

	connect_request cr  1
	start_movement  sm  2 
	confirm_picture cp  3 

	pattern_1 p1  4
	pattern_2 p2  5
	pattern_3 p3  6
	pattern_4 p4  7

	car 				 
	car_forward Up	 8
	car_backward Do	 9
	car_left   Le	 10
	car_right    Ri    11 

	camera 				 
	camera_forward   cU	 12
	camera_backward  cD	 13
	camera_left      cL	 14
	camera_right     cR	 15

	lift 				
	lift_up lu         16
	lift_down ld		 17*/



	//use this when the car finish moving by itself
	void old_send_finished_ack();
	void send_notification();


	
	command_type pattern;

	int ratiox;
	int ratioy;
	int ratiowidth;
	int ratioheight;

	
	void send_finished_ack(char *file_name_parameter);


	char buffer[BUFFER_SIZE]; 
	

	int style_choice;
	int hand_choice;

	int roll_degree;
	int pitch_degree;
	
	private:


	char msg_code[MESSAGELENGTH];
	char content[MESSAGELENGTH];
	char tempBuffer[MAX_MESSAGE_SIZE];


	int server_socket;
	int client_sd;

	int transfer_sd;
	int transfer_client_sd;
	int transfer_port;

	void diy_data();
	int init_server_socket();
	int read_msg();
	void send_msg();

	void connection();

	void send_established();
	void send_start_ack();
	void send_comfrim_ack();

	void send_pattern1_ack();
	void send_pattern2_ack();
	void send_pattern3_ack();
	void send_pattern4_ack();
	void send_pattern5_ack();
	void send_pattern6_ack();
	void send_pattern7_ack();
	void send_pattern8_ack();
	void send_pattern9_ack();
	void send_pattern10_ack();
	void send_patterndiy_ack();
	void send_patternvideo_ack();


	void send_car_forward_ack();
	void send_car_backward_ack();
	void send_car_left_ack();
	void send_car_right_ack();

	void send_camera_forward_ack();
	void send_camera_backward_ack();
	void send_camera_left_ack();
	void send_camera_right_ack();


	void send_lift_up_ack();
	void send_lift_down_ack();
	void send_fetch_degree_notice();
	void send_fetch_degree_without_confirm();

	int degree;

	bool file_transfer_connected;

	

	
};




















#endif
/*********************************************************/
