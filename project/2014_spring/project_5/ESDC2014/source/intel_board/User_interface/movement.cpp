enum Direction{
	UPPER_LEFT,
	LOWER_LEFT,
	UPPER_RIGHT,
	LOWER_RIGHT,
	null
};



int absolute(int num)
{
	if(num<0)
		num=0-num;

	return num;
}

 /*******************************************************
						usage:
  degree_rotation(car_angle,phone_angle,&degree,&direction)

 car->car_angle
 phone->phone_angle
 create two int variable to store the information

 for car_angle,phone_angle,degree, they are postive values

 for direction 1 stands for rotate right
 			  -1 stands for rotate left

********************************************************/





