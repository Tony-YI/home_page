#include <opencv2/opencv.hpp>
#include <stdio.h>
#include "SkinDetector.h"
#include <signal.h>
#include <string>

using namespace std;
using namespace cv;

//VideoCapture capture(1);
void detectAndDisplay( Mat frame );
int faceDetectInit();
/** Global variables */
String face_cascade_name = "lbpcascade_frontalface.xml";
String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
String window_name = "Capture - Face detection";
std::vector<Rect> faces;

void exit_routine(int args)
{
	printf("Executing exit_routine()\n");
	exit(0);
	return ;
}

SkinDetector mySkinDetector;

int main(int argc, char **argv)
{
	signal(SIGTERM,exit_routine);
	signal(SIGINT,exit_routine);
	//open capture object at location zero (default location for webcam)
	if(!faceDetectInit())
	{
		printf("Cannot Init face detector\n");
		return -1;
	}

	VideoCapture capture(atoi(argv[1]));

	if(!capture.isOpened())
	{
		printf("failed to open the device\n");
		exit(-1);
	}

	//set height and width of capture frame
	//capture.set(CV_CAP_PROP_FRAME_WIDTH,320);
	//capture.set(CV_CAP_PROP_FRAME_HEIGHT,480);

	Mat cameraFeed;



	Mat skinMat;

	//start an infinite loop where webcam feed is copied to cameraFeed matrix
	//all of our operations will be performed within this loop
	while(1){
		//store image to matrix
		capture.read(cameraFeed);
	   imshow("Orig Image",cameraFeed);
		//detectAndDisplay(cameraFeed);
		skinMat = mySkinDetector.getSkin(cameraFeed);
		imshow("Skin Image",skinMat);
		waitKey(30);
	}
return 0;
}


int faceDetectInit()
{
	//-- 1. Load the cascade
	if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading face cascade\n"); return -1; };
	if( !eyes_cascade.load( eyes_cascade_name ) ){ printf("--(!)Error loading eyes cascade\n"); return -1; };
	return 1;
}
/**
 * @function detectAndDisplay
 */
void detectAndDisplay( Mat frame )
{

    faces.clear();
    Mat frame_gray;

    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );

    //-- Detect faces
    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0, Size(80, 80) );

    for( size_t i = 0; i < faces.size(); i++ )
    {
        Mat faceROI = frame_gray( faces[i] );
        std::vector<Rect> eyes;
        Mat roi = frame;
        cv::Mat skinMat = mySkinDetector.getSkin(roi);

        imshow("Skin Image",skinMat);
        Scalar value = mean(skinMat);
		  printf("v0 %lf\n",value[0]);
        waitKey(30);
        //-- In each face, detect eyes
        eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CASCADE_SCALE_IMAGE, Size(30, 30) );
        if( eyes.size() == 2)
        {
            //-- Draw the face
            Point center( faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2 );
            ellipse( frame, center, Size( faces[i].width/2, faces[i].height/2 ), 0, 0, 360, Scalar( 255, 0, 0 ), 2, 8, 0 );
		/*
            for( size_t j = 0; j < eyes.size(); j++ )
            { //-- Draw the eyes
                Point eye_center( faces[i].x + eyes[j].x + eyes[j].width/2, faces[i].y + eyes[j].y + eyes[j].height/2 );
                int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
                circle( frame, eye_center, radius, Scalar( 255, 0, 255 ), 3, 8, 0 );
            }*/
        }
    }
    //-- Show what you got
    imshow( window_name, frame );
}
