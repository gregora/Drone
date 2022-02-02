#include "functions.h"
#include "stdio.h"

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


float sign(float x){
	return -(-(x > 0) + 0.5)*2;
}

float pow(float x, int n){
	float ret = 1;
	for(int i = 0; i < n; i++){
		ret*=x;
	}
	return ret;
}

void controller2(float x, float y, float speedx, float speedy, float angle, float angular_velocity, float * left, float * right){

	//constant for converting deg to rad
	float deg2rad = 3.14/180;

	//calculate desired vertical power
	float dp_vertical;
	if(-speedy > min(max(y, -10.0f), 10.0f)){
		dp_vertical = 0.8;
	}else{
		dp_vertical = 0;
	}

	float vertical_dp = max(dp_vertical / cos(angle), 1.0f);


	//calculate desired angle
	float dp_horizontal_l, dp_horizontal_r;

	float break_bias_a = 0.65;
	float break_bias_b = 2;
	float break_space = abs(speedx*speedx)/(6*cos(20*deg2rad)) + break_bias_a*abs(speedx) + break_bias_b;

	float desired_angle;
	if(x >= 3){
		if(speedx >= 0){
			desired_angle = -20*deg2rad;
		}else{
			if(break_space > x){
				desired_angle = 20*deg2rad;
			}else{
				desired_angle = -20*deg2rad;
			}
		}
	}else if(x <= - 3){
		if(speedx <= 0){
			desired_angle = 20*deg2rad;
		}else{
			if(break_space > abs(x)){
				desired_angle = -20*deg2rad;
			}else{
				desired_angle = 20*deg2rad;
			}
		}
	}else {
		desired_angle = max(min(-(x + speedx)*0.1f, 20.0f*deg2rad), -20.0f*deg2rad);
	}

	//get engine powers from desired angle
	angle_controller(angle, desired_angle, angular_velocity, &dp_horizontal_l, &dp_horizontal_r);
	dp_horizontal_l /= 2;
	dp_horizontal_r /= 2;

	//calculate actual engine powers from desired horizontal and vertical powers
	float max_bias = min(1.0f - dp_horizontal_l, 1.0f - dp_horizontal_r);
	float min_bias = - min(dp_horizontal_l, dp_horizontal_r);

	float c_vertical = (dp_horizontal_l + dp_horizontal_r) / 2;
	float bias = min(max(dp_vertical - c_vertical, min_bias), max_bias);

	//apply power
	*left = dp_horizontal_l + bias;
	*right = dp_horizontal_r + bias;

}
