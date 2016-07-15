#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>
using namespace cv;
using namespace std;

int main(int argc, char** argv) {

    cv::Mat image = imread(argv[1]);
    cv::namedWindow("test",CV_WINDOW_AUTOSIZE);
    cv::imshow("test",image);
    waitKey(3000);
    return 1;
}