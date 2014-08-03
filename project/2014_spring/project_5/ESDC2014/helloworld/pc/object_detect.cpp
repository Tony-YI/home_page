#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <stdio.h>

using namespace std;
using namespace cv;

int main(int argc, char ** argv)
{
	VideoCapture cap(0);

	//set the width and the height of frame captured from this camera
	//cap.set(CV_CAP_PROP_FRAME_WIDTH,320);
	//cap.set(CV_CAP_PROP_FRAME_HEIGHT,240);
	if(!cap.isOpened())
	{
		cout<<"Error: Failed to open camera"<<endl;
		return -1;
	}

	Mat img;
	HOGDescriptor hog;
	//use the default People Detector
	cout<<"Setting the default people detector"<<endl;
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
	cout<<"Setting done"<<endl;
	
	cout<<"Creatting a Window"<<endl;
	namedWindow("Video Capture",CV_WINDOW_AUTOSIZE);
	cout<<"Creation done"<<endl;

	while(true)
	{
		printf("Getting an image\n");
		for(unsigned char count = 0; count < 10;count++)
		{
			//printf("Grabbing Frame from file\n");
			if(!cap.grab())
			{
					printf("Error: Cannot grab frame from file\n");
					count--;
					continue;
			}
			//printf("Retrieving Frame into Mat\n");
			if(!cap.retrieve(img))
			{
					printf("Error: Cannot retrieve frame into Mat\n");
					count--;
					continue;
			}
		}
		printf("Image got\n");
		if(!img.data)
		{
			cout<<"Currently no valid data"<<endl;
			continue;
		}
		cout<<"Capture valid frame"<<endl;
		vector<Rect> found, found_filtered;
		/*
		 *
		 *void HOGDescriptor::detectMultiScale(
		 *						const Mat& img, vector<Rect>& foundLocations,
		 *						double hitThreshold, Size winStride, Size padding,
		 *						double scale0, double finalThreshold, bool useMeanshiftGrouping) const
		 *
		 */
		hog.detectMultiScale(img,found,0,Size(8,8),Size(32,32),1.05,2);

		size_t i,j;
		//first for loop
		cout<<"Entering first for loop"<<endl;
		for(i = 0; i < found.size();i++)
		{
			Rect r = found[i];
			for (j=0; j<found.size(); j++)
				if (j!=i && (r & found[j])==r)
					break;
			if (j==found.size())
				found_filtered.push_back(r);
		}
		cout<<"Entering second for loop"<<endl;
		//second for loop
		for(i = 0; i < found_filtered.size();i++)
		{
			Rect r = found_filtered[i];
			r.x += cvRound(r.width * 0.1);
			r.width = cvRound(r.width * 0.8);
			r.y += cvRound(r.height * 0.06);
			r.height = cvRound(r.height * 0.9);
			rectangle(img,r.tl(),r.br(),cv::Scalar(0,255,0),2);
		}
		imshow("Video Capture",img);
		if(waitKey(20) >= 0)
			break;
	}
	return 0;
}	
