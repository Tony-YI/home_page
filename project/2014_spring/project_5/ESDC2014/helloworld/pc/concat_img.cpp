#include <opencv2/opencv.hpp>
#include <stdio.h>

#define FILENAME_LENGTH 256


cv::Mat edge_detection(const cv::Mat &img)
{
	cv::Mat gray_img,result;
	cv::cvtColor(img,gray_img,CV_BGR2GRAY);
	uint16_t threadhold1 = 50;
	uint16_t threadhold2 = 150;
	int32_t apertureSize = 3;
	cv::Canny(gray_img,result,threadhold1,threadhold2,apertureSize);

	return result;
}

cv::Mat concat_img(cv::Mat img1, cv::Mat img2)
{
	cv::Mat result(img1.rows,img1.cols + img2.cols,CV_8UC3);
	
	cv::Mat left(result,cv::Rect(0,0,img1.cols,img1.rows));
	img1.copyTo(left);

	cv::Mat right(result,cv::Rect(img1.cols,0,img2.cols,img2.rows));
	img2.copyTo(right);
	return result;
}
int main(int argc, char ** argv)
{	
	if(argc != 3)
		printf("Usage: concat_img filename filename");

	cv::namedWindow("test",CV_WINDOW_AUTOSIZE);
	cv::Mat img1 = cv::imread(argv[1]);
	cv::Mat img2 = cv::imread(argv[2]);
	cv::imshow("test",concat_img(img1,img2));
	cv::waitKey(0);

	cv::imshow("test",edge_detection(img1));
	cv::waitKey(0);

	cv::imshow("test",edge_detection(img2));
	cv::waitKey(0);

	return 0;
}

