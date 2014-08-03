/**********************************************************
This part is added by project ESDC2014 of CUHK team.
All the code with this header are under GPL open source license.
**********************************************************/



#ifndef _CAMERA_H
#define _CAMERA_H

#include <string>
#include <iostream>
#include <cstdio>

#include "../macro.h"

/*
To play the mjpeg file
mplayer -demuxer lavf /home/terry/movie.mjpg
mplayer  lavf /home/terry/movie.mjpg

*/

class Camera
{
public:
	Camera();
	~Camera();

	void setip(std::string ip_address);

	void set_mode(int mode);
	void init_canon();
	void init_phone();

	std::string photo_af();
	std::string photo_frame();

	
	void save_photo_af();
	void save_photo();

	void zoom(float scaler);// (scaler>1 in) (scaler< 1 out)

	void flash_open();
	void flash_close();


	// void af_open();
	// void af_close();
	// std::string take_photo_af();
	// std::string photo();


	int test_connection();

	void set_qingxin();
	void set_fugu();


	int count_temp_photo;

	void start_video();
	void close_video();


private:
	FILE *fp;

	std::string ip;

	std::string path_temp;
	

	std::string path_capture;

	int count_capture_photo;

	int mode;

	std::string gphoto_cmd;


	void My_popen(std::string cmd);

	int pid;
	
};

#endif
/*********************************************************/
