#include "PIDcontroller.h"

PIDcontroller::PIDcontroller()
{
	this->P = 1;
	this->I = 0;
	this->D = 0;

	this->error_I = 0;
	this->error_D = 0;
	this->prev_error = 0;
}

void PIDcontroller::setPID(double P,double I,double D)
{
	this->P = P;
	this->I = I;
	this->D = D;
}

double PIDcontroller::run(const double &current_error)
{
	double result = 0;
	//current_error > 0 -> target on the right side
	//else -> target on the left side

	result += current_error * this-> P;
	this->error_I += current_error;
	this->error_D = this->prev_error - current_error;

	result += this->error_I * this->I;
	result += this->error_D * this->D;
	
	return result;
}

void PIDcontroller::reset()
{
	this->error_I = 0;
	this->error_D = 0;
}