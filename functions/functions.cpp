#include "functions.h"

using namespace std;

//run engines to achieve desired angle
void angle_controller(float angle, float target_angle, float angular_velocity, float * left, float * right){

	float turn_coef = 5;

	*left = 0;
	*right = 0;

	if(-angular_velocity > (angle - target_angle) * turn_coef){
		*left = 1;
	}else{
		*right = 1;
	}
}

//run engines to achieve desired position (x - axis)
void position_controller(float x, float target_x, float speedx, float * target_angle){

	double max_angle = 0.3;

	float move_coef = 0.5;
	float desired_speed = (x-target_x)*move_coef;

	float target_angle_magnitude = min(0.1*abs(desired_speed - speedx), max_angle);

	if(-speedx > desired_speed){
		*target_angle = target_angle_magnitude;
	}else{
		*target_angle = -target_angle_magnitude;
	}

}

//run engines to achieve desired position
void controller(float x, float y, float speedx, float speedy, float angle, float angular_velocity, float * left, float * right){


	*left = 0;
	*right = 0;

	float target_angle = 0;
	position_controller(x, 0, speedx, &target_angle);

	float al, ar;
	angle_controller(angle, target_angle, angular_velocity, &al, &ar);
	if(al == 1){
		*left += 0.3;
		*right -= 0.3;
	}

	if(ar == 1){
		*right += 0.3;
		*left -= 0.3;
	}

	float sum_power = (*left + *right)*cos(angle);

	if(-speedy > y){
		*left += 0.8;
		*right += 0.8;

	}

}
