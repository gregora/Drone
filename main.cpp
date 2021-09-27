#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <math.h>


using namespace std;

int WIDTH = 800;
int HEIGHT = 400;

bool ENABLE_AUTO = true;

float sigmoid(float x){
	float exp = pow(2.71, x);
	return exp / (1 + exp);
}

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

	float move_coef = 0.5;
	float desired_speed = (x-target_x)*move_coef;

	float target_angle_magnitude = min(0.1*abs(desired_speed - speedx), 0.3);

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

	if(-speedy > y){
		*left = 0.2;
		*right = 0.2;
	}

	float target_angle = 0;
	position_controller(x, 0, speedx, &target_angle);

	float al, ar;
	angle_controller(angle, target_angle, angular_velocity, &al, &ar);
	if(al == 1)
		*left += 0.3;
	if(ar == 1)
		*right += 0.3;



}



class Drone : public sf::Drawable{

	public:
		float x = 0;
		float y = 0;

		float speedx = 0;
		float speedy = 0;

		float angle = 0;
		float angular_velocity = 0;

		float inertia = 0.1;
		float mass = 0.1;
		float engine_dist = 0.2;
		float engine_pow = 3;

		void (*controller)(float x, float y, float speedx, float speedy, float angle, float angular_velocity, float * left, float * right) = nullptr;


		Drone() {
			square.setSize(sf::Vector2f(30, 10));
			square.setOrigin(15, 5);

		}

		void physics(float delta){

			//call controller
			if(controller != nullptr)
				controller(x, y, speedx, speedy, angle, angular_velocity, &left_power, &right_power);

			//apply engine power
			float force = (left_power + right_power)*engine_pow;
			speedx += force*sin(angle) / mass * delta;
			speedy += force*cos(angle) / mass * delta;

			angular_velocity += (left_power - right_power)*engine_pow*engine_dist/inertia*delta;

			//apply physics
			angle += angular_velocity*delta;
			speedy += -10*delta;

			x += speedx*delta;
			y += speedy*delta;

			square.setPosition(x*10 + WIDTH / 2, - y*10 + HEIGHT / 2);
			square.setRotation(angle * 180 / 3.14);

		}

		void setPower(float left, float right){

			if(left > 1)
				left = 1;
			if(left < 0)
				left = 0;

			if(right > 1)
				right = 1;
			if(right < 0)
				right = 0;

			left_power = left;
			right_power = right;
		}

		void debug(){
			std::cout << "x: " << x << ", y:" << y << std::endl;
		}


	    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {

	        target.draw(square);
	    }

	private:
		sf::RectangleShape square;

		float left_power = 0;
		float right_power = 0;
};


int main(){

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;


	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "SFML", sf::Style::Default, settings);

	float time_elapsed = 0;
	sf::Clock clock;

	Drone drone;

	float left = 0;
	float right = 0;
	int mode = 0;

    while (window.isOpen())
    {
		sf::Time dt = clock.restart();
		float delta = dt.asSeconds();
		time_elapsed += delta;


        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
			else if(event.type == sf::Event::KeyPressed){

				if(event.key.code == sf::Keyboard::Left){
					left = 1;
					mode += 1;
				}else if(event.key.code == sf::Keyboard::Right){
					right = 1;
					mode += 1;
				}

			}else if(event.type == sf::Event::KeyReleased){

				if(event.key.code == sf::Keyboard::Left){
					left = 0;
					mode -= 1;
				}else if(event.key.code == sf::Keyboard::Right){
					right = 0;
					mode -= 1;
				}

			}

        }

		if(delta == 0){
			delta = 0.1;
		}

		drone.setPower(left, right);

		if(ENABLE_AUTO){
			if(mode == 0){
				drone.controller = &controller;
			}else{
				drone.controller = nullptr;
			}
		}

		drone.physics(delta);

		window.clear();
		window.draw(drone);
        window.display();
    }

    return 0;
}
