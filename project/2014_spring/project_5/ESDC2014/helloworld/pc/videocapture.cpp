/*
	This is a hello world sample program to have the camera's image shown on the monitor
*/
#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdio.h>
using namespace std;
using namespace cv;

int main(int argc, char ** argv)
{
	VideoCapture cap("http://192.168.43.1:8080/video");
	printf("haha\n");
	if(!cap.isOpened())
	{
		cout<<"The camera is not opened successfully"<<endl;
		return -1;
	}

	Mat img;
	namedWindow("Video Capture",CV_WINDOW_AUTOSIZE);

	while(true)
	{
		//cap>>img;
		if(!cap.grab())
		{
				printf("Failed to grab frame\n");
				continue;
		}
		if(!cap.retrieve(img))
		{
				printf("Failed to retrieve frame\n");
				continue;
		}
		if(!img.data)
		{
				printf("No IMAGE data available\n");
				continue;
		}
		imshow("Video Capture",img);
		if(waitKey(1000) >= 0)
			break;
	}
	return 0;
}
