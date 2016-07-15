// Written by: Chan Ho Kwan
/*
This is the header file of the part of camshift
*/

#ifndef _CAMSHIFTDEMO_H
#define _CAMSHIFTDEMO_H

#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <ctype.h>
#include "../macro.h"

using namespace std;
using namespace cv;
// CvRect r
double runCAMShift(Rect r);

// double runCAMShift();
#endif