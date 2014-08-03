// Written by: Chan Ho Kwan
/*
This is the C++ source code of 4-point algorithm. This algorithm will use the 2D coordinates
of 4 model points as input, and then compute their cooresponding 3D coordinates.
*/
#include "fourPointAlgo.h"

#define J(i, j)		J.at<double>(i, j)
#define JI(i, j)	JI.at<double>(i, j)
#define a(i)	a.row(i)
#define u(i)	u.row(i)
#define l(i)	l.at<double>(i, 0)
#define E(i)	E.at<double>(i, 0)
#define h(i)	h.at<double>(i, 0)

Mat l = Mat::zeros(4, 1, CV_64F);		// distance between the model points and optical center of webcam
double e01, e02, e03, e12, e13, e23, g, g2;		// errors between actual length and estimated length
Mat E = Mat::zeros(7, 1, CV_64F);		// 7x1 Error vector E
Mat J = Mat::zeros(7, 4, CV_64F);		// 7x4 Jacobian matrix J
Mat h = Mat::ones(4, 1, CV_64F);

double length(Mat a){	// find the magnitude (length) of a vector
	CV_Assert(a.size() == Size(3, 1));
	double x = a.at<double>(0,0);
	double y = a.at<double>(0,1);
	double z = a.at<double>(0,2);
	return sqrt(x*x + y*y + z*z);
}

void init4Point(Mat a, double focal, Mat u){
	Mat tmp = Mat::zeros(4, 3, CV_64F);

	// compute the 4 unit vectors
	for (int i = 0; i < 4; i++){
		// first, convert the 2D image point into 3D cooridinates
		// hence, we can get the 3D vector, which is pointing from
		// the origin (0,0) to the 2D point
		tmp.at<double>(i, 0) = a.at<double>(i, 0);		// x = x-coordinate of 2D point
		tmp.at<double>(i, 1) = a.at<double>(i, 1);		// y = y-coordinate of 2D point
		tmp.at<double>(i, 2) = focal;					// z = focal length of webcam

		// then, compute the unit vector of the 3D vector
		u(i) = tmp.row(i)/length(tmp.row(i));
	}
	//cout << "u = " << u << endl;
}

double L(Mat m, int a, int b){	// find the distance between two point
	double xd = m.at<double>(a, 0) - m.at<double>(b, 0);
	double yd = m.at<double>(a, 1) - m.at<double>(b, 1);
	double zd = m.at<double>(a, 2) - m.at<double>(b, 2);
	return sqrt(xd*xd + yd*yd + zd*zd);
}

Mat A(Mat m, int i, int j){		// find the vector A(j to i)
	return m.row(i) - m.row(j);		// A(j to i) = A(i) - A(j)
}

Mat computeError(Mat u, Mat m){
	// compute the errors between actual length and estimated length
	double length = 0.0;
	length = L(m, 0, 1);	e01 = l(0)*l(0) + l(1)*l(1) - 2*l(0)*l(1)*u(0).dot(u(1)) - length * length;
	length = L(m, 0, 2);	e02 = l(0)*l(0) + l(2)*l(2) - 2*l(0)*l(2)*u(0).dot(u(2)) - length * length;
	length = L(m, 0, 3);	e03 = l(0)*l(0) + l(3)*l(3) - 2*l(0)*l(3)*u(0).dot(u(3)) - length * length;
	length = L(m, 1, 2);	e12 = l(1)*l(1) + l(2)*l(2) - 2*l(1)*l(2)*u(1).dot(u(2)) - length * length;
	length = L(m, 1, 3);	e13 = l(1)*l(1) + l(3)*l(3) - 2*l(1)*l(3)*u(1).dot(u(3)) - length * length;
	length = L(m, 2, 3);	e23 = l(2)*l(2) + l(3)*l(3) - 2*l(2)*l(3)*u(2).dot(u(3)) - length * length;
	
	// compute g = ((u1*l1 - u2*l2)x(u0*l0 - u2*l2))*(u3*l3 - u2*l2) - (A21 x A20) * A23
	g = (l(1)*u(1) - l(2)*u(2)).cross(l(0)*u(0) - l(2)*u(2)).dot(l(3)*u(3) - l(2)*u(2))
			- A(m, 1, 2).cross(A(m, 0, 2)).dot(A(m, 3, 2));
	/*g =  l[1]*l[0]*l[3]*((u[1]^u[0])*u[3]) - l[2]*l[0]*l[3]*((u[2]^u[0])*u[3]) - l[1]*l[2]*l[3]*((u[1]^u[2])*u[3])
			- l[1]*l[0]*l[2]*((u[1]^u[0])*u[2]) + l[2]*l[0]*l[2]*((u[2]^u[0])*u[2]) + l[1]*l[2]*l[2]*((u[1]^u[2])*u[2]);*/

	// compute the 7x1 Error vector E
	E(0) = e01;
	E(1) = e02;
	E(2) = e03;
	E(3) = e12;
	E(4) = e13;
	E(5) = e23;
	E(6) = g;

	// compute the 7x4 Jacobian matrix J
	J(0,0) = 2*( l(0) - l(1)*u(0).dot(u(1)) );
	J(0,1) = 2*( l(1) - l(0)*u(0).dot(u(1)) );
	J(0,2) = J(0,3) = 0;

	J(1,0) = 2*( l(0) - l(2)*u(0).dot(u(2)) );
	J(1,1) = J(1,3) = 0;
	J(1,2) = 2*( l(2) - l(0)*u(0).dot(u(2)) );

	J(2,0) = 2*( l(0) - l(3)*u(0).dot(u(3)) );
	J(2,1) = J(2,2) = 0;
	J(2,3) = 2*( l(3) - l(0)*u(0).dot(u(3)) );

	J(3,0) = J(3,3) = 0;
	J(3,1) = 2*( l(1) - l(2)*u(1).dot(u(2)) );
	J(3,2) = 2*( l(2) - l(1)*u(1).dot(u(2)) );

	J(4,0) = J(4,2) = 0;
	J(4,1) = 2*( l(1) - l(3)*u(1).dot(u(3)) );
	J(4,3) = 2*( l(3) - l(1)*u(1).dot(u(3)) );

	J(5,0) = J(5,1) = 0;
	J(5,2) = 2*( l(2) - l(3)*u(2).dot(u(3)) );
	J(5,3) = 2*( l(3) - l(2)*u(2).dot(u(3)) );

	J(6, 0) = l(1)*l(3)* u(1).cross(u(0)).dot(u(3))
				- l(2)*l(3)* u(2).cross(u(0)).dot(u(3))
				- l(1)*l(2)* u(1).cross(u(0)).dot(u(2))
				+ l(2)*l(2)* u(2).cross(u(0)).dot(u(2));
	J(6, 1) = l(0)*l(3)* u(1).cross(u(0)).dot(u(3))
				- l(2)*l(3)* u(1).cross(u(2)).dot(u(3))
				- l(0)*l(2)* u(1).cross(u(0)).dot(u(2))
				+ l(2)*l(2)* u(1).cross(u(2)).dot(u(2));
	J(6, 2) = 2*l(2)*l(0)* u(2).cross(u(0)).dot(u(2))
				+ 2*l(1)*l(2)* u(1).cross(u(2)).dot(u(2))
				- l(0)*l(3)* u(2).cross(u(0)).dot(u(3))
				- l(1)*l(3)* u(1).cross(u(2)).dot(u(3))
				- l(1)*l(0)* u(1).cross(u(0)).dot(u(2));
	J(6, 3) = l(1)*l(0)* u(1).cross(u(0)).dot(u(3))
				- l(2)*l(0)* u(2).cross(u(0)).dot(u(3))
				- l(1)*l(2)* u(1).cross(u(2)).dot(u(3));

	// J				:	7x4 matrix
	// transpose of J	:	4x7 matrix
	// E				:	7x1 matrix
	// so, (transpose of J) * J becomes a 4x4 matrix
	// and, (transpose of J) * E becomes a 4x1 matrix
	return (J.t() * J).inv() * (J.t() * E);		// h = inverse((transpose of J)*J) * ((transpose of J)*E)
}

Mat run4PointAlgo(Mat a, Mat m, double focal, Mat guess, double error, int maxiter){
	// a	 :	2D coordinates of 4 model points on the image plane
	// m	 :	3D coodinates of 4 model points
	// focal :	focal length of webcam (in pixel)
	// guess :	initial value of length l
	// error :	lower bound of the computed error
	//			i.e. when the values of computed error < value of 'error', stop the algorithm
	// maxiter:	upper bound of no. of iteration of the algorithm
	//			i.e. when the no. of iteration < value of 'error', stop the algorithm

	CV_Assert(a.size() == Size(2, 4) && m.size() == Size(3, 4));
	
	int numIter = 0;		// count the no. of iteration
	Mat u = Mat::zeros(4, 3, CV_64F);		// 4 unit vectors of the above vectors
	Mat limit = (Mat_<double>(4,1) << error, error, error, error);
	
	init4Point(a, focal, u);	// initialize the unit vectors
	l = guess;					// initalize l with 'guess'

	// start the iteration of Gauss-Newton method
	h = computeError(u, m);
	while ((sum(abs(h) > limit).val[0] == 4*255) && numIter < maxiter){
		l -= h;					// new l = (old l) - h;
		h = computeError(u, m);
		//cout << "h = " << h << endl;
		numIter++;
	}
	//cout << "l = " << l << endl;
	// compute the 3D coordinates
	for (int i = 0; i < 4; i++)
		u(i) *= l(i);		// 3D coordinates of point(i) = u(i) * l(i)
	guess = l;			// save the values of computed l as the initial value for next time

	return u;
}
