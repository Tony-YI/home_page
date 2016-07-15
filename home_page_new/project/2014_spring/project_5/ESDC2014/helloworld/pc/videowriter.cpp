#include <iostream>
 
// Include standard OpenCV headers
#include "cv.h"
#include "highgui.h"
 
using namespace std;
 
// All the new API is put into "cv" namespace
using namespace cv;
 
int main (int argc, char *argv[])
{
    // Open the default camera
    VideoCapture capture(0);
 
    // Check if the camera was opened
    if(!capture.isOpened())
    {
        cerr << "Could not create capture";
        return -1;
    }
 
    // Get the properties from the camera
    double width = capture.get(CV_CAP_PROP_FRAME_WIDTH);
    double height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
 
    cout << "Camera properties\n";
    cout << "width = " << width << endl <<"height = "<< height << endl;
 
    // Create a matrix to keep the retrieved frame
    Mat frame;
 
    // Create a window to show the image
    namedWindow ("Capture", CV_WINDOW_AUTOSIZE);
 
    // Create the video writer
    printf("Creating a video  writer\n");
    VideoWriter video("capture.avi",-1, 30, cvSize((int)width,(int)height) );
    printf("Creation done\n");
    // Check if the video was opened
    if(!video.isOpened())
    {
        cerr << "Could not create video.";
        return -1;
    }
 
    cout << "Press Esc to stop recording." << endl;
 
    // Get the next frame until the user presses the escape key
    while(true)
    {
        // Get frame from capture
        capture >> frame;
 
        // Check if the frame was retrieved
        if(!frame.data)
        {
            cerr << "Could not retrieve frame.";
            return -1;
        }
 
        // Save frame to video
        video << frame;
 
        // Show image
        imshow("Capture", frame);
 
        // Exit with escape key
        if(waitKey(1) == 27)
            break;
    }
 
    // Exit
    return 0;
}