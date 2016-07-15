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
String window_name = "Pedestrian Detection";
/*-------------------------------*/


int main(int argc, char ** argv)
{

	DIR *dir;
	struct dirent *ent;
	if((dir = opendir(argv[1])) == NULL)
	{
		perror("");
		return EXIT_FAILURE;
	}

	if(!faceDetectInit())
	{
		printf("Cannot Init face detector\n");
		return -1;
	}
	while((ent = readdir(dir)) != NULL)
	{
		if(strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0)
			continue;
		char filename[64];
		strcpy(filename,argv[1]);
		strcat(filename,"/");
		strcat(filename,ent->d_name);
		printf("Openning %s\n",filename);

		Mat img = imread(filename,CV_LOAD_IMAGE_COLOR);
		
		Mat people_detect_img = img.clone();
		Mat face_detect_img = img.clone();

		HOGDescriptor hog;
		//use the default People Detector
		cout<<"Setting the default people detector"<<endl;
		hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
		cout<<"Setting done"<<endl;
		cout<<"Creatting a Window"<<endl;
		namedWindow("Pedestrian Detection",CV_WINDOW_AUTOSIZE);
		cout<<"Creation done"<<endl;
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
		
		struct timespec tstart={0,0}, tend={0,0};
		clock_gettime(CLOCK_MONOTONIC, &tstart);
		hog.detectMultiScale(people_detect_img,found,0,Size(8,8),Size(32,32),1.05,2);
		clock_gettime(CLOCK_MONOTONIC, &tend);
		printf("some_long_computation took about %.5f seconds\n",
			((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - 
			((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec));

    	size_t i,j;
		//first for loop

		for(i = 0; i < found.size();i++)
		{
			Rect r = found[i];
			for (j=0; j<found.size(); j++)
				if (j!=i && (r & found[j])==r)
					break;
			if (j==found.size())
				found_filtered.push_back(r);
		}

		//second for loop
		for(i = 0; i < found_filtered.size();i++)
		{
			Rect r = found_filtered[i];
			r.x += cvRound(r.width * 0.1);
			r.width = cvRound(r.width * 0.8);
			r.y += cvRound(r.height * 0.06);
			r.height = cvRound(r.height * 0.9);
			//draw a rectangle for every detected region
			rectangle(people_detect_img,r.tl(),r.br(),cv::Scalar(0,255,0),2);
		}

		imshow("Pedestrian Detection",people_detect_img);
		if(waitKey(0) == 'n')
		{
			destroyWindow("Pedestrian Detection");
		}

		namedWindow("Pedestrian Detection",CV_WINDOW_AUTOSIZE);
		detectAndDisplay(face_detect_img);
		if(waitKey(0) == 'n')
		{
			continue;
			destroyWindow("Pedestrian Detection");
		}
		else
			break;
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
