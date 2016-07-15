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

/*
int main()
{

// Rect a1(364,18,35,35);
// Rect a2(382,58,30,30);
// Rect a3(365,79,29,29);
// Rect a4(374,93,28,28);
// Rect a5(363,108,27,27);
Rect a6(370,135,26,26);
// Rect a7(373,126,25,25);



// printf("Expect 2000m,%lf\n", runCAMShift(a1));	
// printf("Expect 2500m,%lf\n", runCAMShift(a2));	
// printf("Expect 3000m,%lf\n", runCAMShift(a3));	
// printf("Expect 3500m,%lf\n", runCAMShift(a4));	
// printf("Expect 4000m,%lf\n", runCAMShift(a5));	
printf("Expect 4500m,%lf\n", runCAMShift(a6));	
// printf("Expect 5000m,%lf\n", runCAMShift(a7));	

	return 0;
}
*/
