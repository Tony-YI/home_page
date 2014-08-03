
#include "fourPointAlgo.h"
#include "camshiftdemo.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <stdio.h>

//for directory operation
#include <sys/types.h>
#include <dirent.h>

using namespace std;
using namespace cv;

/*for face detection*/
void detectAndDisplay( Mat frame );
int faceDetectInit();
/** Global variables */
String face_cascade_name = "lbpcascade_frontalface.xml";
String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
String window_name = "Face Detection";
/*-------------------------------*/

/*
int main(int argc, char ** argv)
{

	// DIR *dir;
	// // "/home/terry/Desktop/testing/0.jpg"
	// struct dirent *ent;
	// if((dir = opendir(argv[1])) == NULL)
	// {
	// 	perror("");
	// 	return EXIT_FAILURE;
	// }

	if(!faceDetectInit())
	{
		printf("Cannot Init face detector\n");
		return -1;
	}

	while(1)
	{
		// if(strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0)
		// 	continue;
		// char filename[64];
		// strcpy(filename,argv[1]);
		// strcat(filename,"/");
		// strcat(filename,ent->d_name);
		// printf("Openning %s\n",filename);

		Mat img = imread("/home/terry/Desktop/testing/0.jpg",CV_LOAD_IMAGE_COLOR);
		namedWindow("Face Detection",CV_WINDOW_AUTOSIZE);
		detectAndDisplay(img);
		if(waitKey(0) == 'n')
		{
			continue;
			destroyWindow("Face Detection");
		}
		else
			break;
	}
	return 0;
}
*/
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
    std::vector<Rect> faces;
    Mat frame_gray;

    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );

    //-- Detect faces
    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0, Size(10, 10) );

    for( size_t i = 0; i < faces.size(); i++ )
    {
        Mat faceROI = frame_gray( faces[i] );
        std::vector<Rect> eyes;
		printf("%lf\n", runCAMShift(faces[i]));	
        //-- In each face, detect eyes
        eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CASCADE_SCALE_IMAGE, Size(5, 5) );
        if( eyes.size() == 2)
        {
            //-- Draw the face
            Point center( faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2 );
            ellipse( frame, center, Size( faces[i].width/2, faces[i].height/2 ), 0, 0, 360, Scalar( 255, 0, 0 ), 2, 8, 0 );

            for( size_t j = 0; j < eyes.size(); j++ )
            { //-- Draw the eyes
                Point eye_center( faces[i].x + eyes[j].x + eyes[j].width/2, faces[i].y + eyes[j].y + eyes[j].height/2 );
                int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
                circle( frame, eye_center, radius, Scalar( 255, 0, 255 ), 3, 8, 0 );
            }
        }
    }
    //-- Show what you got
    imshow( window_name, frame );
}

