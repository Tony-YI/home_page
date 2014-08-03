#include <opencv2/opencv.hpp>
#include "stdio.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
using namespace cv;

int main(int argc, char **argv)
{
	VideoCapture cap(1);
	if(!cap.isOpened())  // check if we succeeded
	{
		printf("Failed to open the device %s\n",argv[1]);
		return -1;
	}
	Mat edges;
	namedWindow("edges",1);

	for(;;)
	{
		Mat frame;
		cap >> frame; // get a new frame from camera
		if(!frame.data)
		{
			printf("Error : no frame data\n");
			continue;
		}
		cvtColor(frame, edges, CV_BGR2GRAY);
		GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
		Canny(edges, edges, 0, 30, 3);
		imshow("edges", edges);
		if(waitKey(30) >= 0) break;
	}
	// the camera will be deinitialized automatically in VideoCapture destructor
	return 0;
}
