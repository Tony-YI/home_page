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

#include <iostream>
#include "intel_board/intel_board.h"
#include "intel_board/macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
using namespace std;

void exit_routine(int32_t arg);
void generate_dir();
void print_usage_and_exit();
void parameter_setting(int32_t argc,char **argv);

/************************/
char *glo_PATH_TEMP = NULL;
char *glo_DIR_NAME = NULL;
cv::Rect glo_prev_face;
unsigned char continuity = 1;
uint8_t glo_source_mode;

uint8_t glo_test_mbed = 0;

uint8_t glo_motion_enable = 1;
uint8_t glo_display_enable = 1;
uint8_t glo_multi_target = 0;
uint8_t glo_num_target = 1;
uint8_t glo_high_angle_shot = 0;
uint8_t glo_hand_gesture = 0;
uint8_t glo_tracking = 0;
uint8_t glo_test_filetransfer = 0;
uint8_t glo_autofocus = 0;
uint8_t glo_waist_shot = 0;
uint8_t glo_pid = 0;
uint8_t glo_debug_msg = 0;

uint8_t glo_tracking_by_rotate = 0;

double glo_exp_dis = 0;

command_type glo_pattern;
int32_t glo_argc;
/************************/
intel_board *robot;

int main(int32_t argc, char ** argv) 
{
	/* prints Hello World */
	glo_argc = argc;
	if(argc < 2)
	{
		print_usage_and_exit();
	}

	/*use phone or canon or canon as video stream*/
	glo_source_mode = atoi(argv[1]);

	cout << "Hello Intel ESDC" << endl;
	/*
	   mode, default value MANUAL MODE
	   img_source, default value using Cellphone 
	*/
	uint8_t mode = 1;
	uint8_t img_source = 0;

	//use WebCam to get image for motion planing
	if(glo_source_mode == 2 || glo_source_mode == 3)
		img_source = IMG_SOURCE_WEBCAM;

	glo_PATH_TEMP = (char *) malloc(sizeof(char) * FILENAME_LENGTH);
	glo_DIR_NAME = (char *) malloc(sizeof(char) * FILENAME_LENGTH);
	
	generate_dir();
	signal(SIGTERM,exit_routine);
	signal(SIGINT,exit_routine);
	if(argc >= 3)//the user has set the mode
	{
		if(atoi(argv[2]) == 0)
			mode = 1;
		else
			mode = atoi(argv[2]);
		
		printf("mode is %u\n",mode);
		if(mode == 2)
		{
			printf("The robot is going to launch as mbed debug mode\n");
			glo_test_mbed = 1;
		}

		if(mode == 3)
		{
			printf("The robot is going to launch as filetransfer debug mode\n");
			glo_test_filetransfer = 1;
		}
		parameter_setting(argc,argv);
	}
    else // the user has not set the mode
    	printf("The robot is going to initiate in default mode\n");


    robot = new intel_board(mode,img_source);
    robot->main_function();
	exit_routine(0);
	return 0;
}

void exit_routine(int arg)
{
        printf("\n\n\n\nExecuting the pre-registered exit routine\n");
        delete robot;
        free(glo_PATH_TEMP);
        exit(0);
}


void generate_dir()
{
	printf("generate_dir running\n");
	char *filename =(char *) malloc(sizeof(char) * FILENAME_LENGTH);
	time_t timestamp = time(NULL);
	struct tm *current_time = gmtime(&timestamp);
	//get the filename in format of month-day_hour:minute:second
	sprintf(filename,"/home/intelcup/Desktop/%d_%d_%d_%d_%d",
		current_time->tm_mon,
		current_time->tm_mday,
		current_time->tm_hour,
		current_time->tm_min,
		current_time->tm_sec);
	strcpy(glo_PATH_TEMP,filename);
	strcpy(glo_DIR_NAME,glo_PATH_TEMP);
	free(filename);
	strcat(glo_PATH_TEMP,"/");
}


void print_usage_and_exit()
{
	printf("Usage: 1-> ./ESDC 0 [mode] [arguments] for PHONE\n");
	printf("Usage: 2-> ./ESDC 1 [mode] [arguments] for CANON\n");
	printf("Usage: 3-> ./ESDC 2 [mode] [arguments] for WebCam device /dev/video0 \n");
	printf("Usage: 4-> ./ESDC 4 [mode] [arguments] for WebCam and CANON mode\n");
	printf("[mode] = 1 or non specified -> normal mode\n");
	printf("[mode] = 2 -> mbed debugging\n");
	printf("[mode] = 3 -> file transfer debug mode\n");
	printf("-dd -> disabled display\n");
	printf("-em [num] -> enable multi targets, with [num] targets\n");
	printf("-dm -> disabled motion\n");
	printf("-help -> for help\n");
	printf("-hg -> enable hand gesture filtering\n");
	printf("-t -> tracking only\n");
	exit_routine(glo_argc);
}


void parameter_setting(int32_t argc,char **argv)
{
	for(int32_t count = 0;count < argc;count++)
	{
		if(strcmp(argv[count],"-dd") == 0)
			glo_display_enable = 0;
			
		if(strcmp(argv[count],"-em") == 0)
		{
			glo_multi_target = 1;
			if(count + 1 < argc)
				glo_num_target = atoi(argv[count + 1]);
			else
				glo_num_target = 2;
		}

		if(strcmp(argv[count],"-debug") == 0)
			glo_debug_msg = 1;

		if(strcmp(argv[count],"-dm") == 0)
			glo_motion_enable = 0;
		if(strcmp(argv[count],"-hg") == 0)
			glo_hand_gesture = 1;
		if(strcmp(argv[count],"-t") == 0)
			glo_tracking = 1;

		if(strcmp(argv[count],"-af") == 0)
			glo_autofocus = 1;

		if(strcmp(argv[count],"-pause") == 0)
			continuity = 0;

		if(strcmp(argv[count],"-pid") == 0)
			glo_pid = 1;

		if(strcmp(argv[count],"-help") == 0)
			print_usage_and_exit();
	}

	if(glo_motion_enable)
		printf("SETTING: enable motion\n");
	else
		printf("SETTING: disable motion\n");

	if(glo_multi_target)
		printf("SETTING: enable multi targets\n");
	else
		printf("SETTING: disable multi targets\n");

	if(glo_display_enable)
		printf("SETTING: enable display\n");
	else
		printf("SETTING: disable display\n");

	if(glo_hand_gesture)
		printf("SETTING: enable hand gesture filtering\n");
	else
		printf("SETTING: disbale hand gesture filtering\n");

	if(glo_tracking)
		printf("SETTING: enable tracking\n");
	else
		printf("SETTING: disable tracking\n");

	if(continuity == 0)
		printf("SETTING: not continuous\n");
	
	if(glo_pid == 1)
		printf("SETTING: enable PID\n");

	if(glo_debug_msg)
		printf("SETTING: enable debug msg\n");

}
