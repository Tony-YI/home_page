#include <opencv2/opencv.hpp>
#include <stdio.h>

//for directory operation
#include <sys/types.h>
#include <dirent.h>
int main(int argc, char ** argv)
{

	DIR *dir;
	struct dirent *ent;
	if((dir = opendir(argv[1])) == NULL)
	{
		perror("");
		return EXIT_FAILURE;
	}

	cv::namedWindow("haha",CV_WINDOW_AUTOSIZE);
	while((ent = readdir(dir)) != NULL)
	{
		//skip the directory 
		if(strcmp(".",ent->d_name) == 0 || strcmp("..",ent->d_name) == 0)
			continue;
		char filename[64];
		strcpy(filename,argv[1]);
		strcat(filename,"/");
		strcat(filename,ent->d_name);
		cv::Mat image;
		printf("filename is %s\n",filename);
		image = cv::imread(filename,CV_LOAD_IMAGE_COLOR);
		cv::imshow("haha",image);
		char k = cv::waitKey(500);
		printf("You have pressed %c\n",k);
	}
	return 0;
}
