#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

Mat detectPeople(const Mat &people_detect_img,vector<Rect> & found_filtered);
int main(int argc, char **argv)
{
	Mat image = imread(argv[1],CV_LOAD_IMAGE_COLOR);
	namedWindow("haha",CV_WINDOW_AUTOSIZE);
	vector<Rect> found_filtered;
	Mat result_img = detectPeople(image,found_filtered);
	imshow("haha",result_img);
	while(waitKey(0) != 'n')
	{
		;
	}
	printf("image width %d height %d\n",image.cols,image.rows);
	for(size_t count = 0;count < found_filtered.size();count++)
	{
		found_filtered[count].x = max(found_filtered[count].x,0);
		found_filtered[count].y = max(found_filtered[count].y,0);

		if(found_filtered[count].x + found_filtered[count].width > image.cols)
		{
			found_filtered[count].width = image.cols - found_filtered[count].x;
		}
		if(found_filtered[count].y + found_filtered[count].height > image.rows)
		{
			found_filtered[count].height = image.rows - found_filtered[count].y;
		}
		found_filtered[count].width = min(found_filtered[count].width,image.cols);
		found_filtered[count].height = min(found_filtered[count].height,image.rows);
		printf("roi tl x:%d,y:%d,width %d, height %d, x + width %d, y + height %d\n\n",
			found_filtered[count].x,
			found_filtered[count].y,
			found_filtered[count].width,
			found_filtered[count].height,
			found_filtered[count].x + found_filtered[count].width,
			found_filtered[count].y + found_filtered[count].height);

		Mat subImage(image,found_filtered[count]);
		vector<Rect> subImage_found_filtered;
		result_img = detectPeople(subImage,subImage_found_filtered);
		imshow("haha",result_img);
		while(waitKey(0) != 'n')
		{
			;
		}
	}
	return 0;
}

Mat detectPeople(const Mat &people_detect_img,vector<Rect> & found_filtered)
{
	HOGDescriptor hog;
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
	vector<Rect> found;
	hog.detectMultiScale(people_detect_img,found,0,Size(8,8),Size(32,32),1.05,2);
	Mat result_img = people_detect_img.clone();
	size_t i,j;
	//first for loop

	for(i = 0; i < found.size();i++)
	{
		Rect r = found[i];
		for (j=0; j<found.size(); j++)
		{
			if (j!=i && (r & found[j])==r)
				break;
		}
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
		rectangle(result_img,r.tl(),r.br(),cv::Scalar(0,255,0),2);
	}
	return result_img;
}
