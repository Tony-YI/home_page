#ifndef _PIDCONTROLLER_H
#define _PIDCONTROLLER_H

class PIDcontroller
{
	double error_I;
	double error_D;
	double P,I,D;
	double prev_error;

public:
	PIDcontroller();
	void setPID(double P,double I,double D);
	double run(const double &current_error);
	void reset();
};

#endif