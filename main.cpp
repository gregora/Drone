#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <math.h>
#include "functions/functions.h"

using namespace std;

int WIDTH = 1900;
int HEIGHT = 1200;

bool ENABLE_AUTO = true;

float sigmoid(float x){
	float exp = pow(2.71, x);
	return exp / (1 + exp);
}

float randFloat() {

	std::random_device rd{};
	std::mt19937 engine{rd()};
	std::uniform_real_distribution<double> dist{0.0, 1.0};
	double ret = dist(engine);
	return ret;

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


		Drone(char * path = "img/drone.png") {
			texture.loadFromFile(path);

			float scale = 0.1;

			sprite.setTexture(texture);
			sprite.setScale(scale, scale);
			sprite.setOrigin(texture.getSize().x / 2, texture.getSize().y / 2);

			square.setSize(sf::Vector2f(100, 100));
			square.setScale(scale, scale);
			square.setOrigin(100 / 2, 100 / 2);

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

			sprite.setPosition(x*10 + WIDTH / 2, - y*10 + HEIGHT / 2);
			sprite.setRotation(angle * 180 / 3.14);

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

		void setColor(int red, int green, int blue){
			square.setFillColor(sf::Color(red, green, blue));
		}


	    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {

	        target.draw(sprite);
			target.draw(square);
	    }

	private:
		sf::Sprite sprite;
		sf::Texture texture;
		sf::RectangleShape square;

		float left_power = 0;
		float right_power = 0;
};


int main(){

	sf::RectangleShape background;
	background.setFillColor(sf::Color(0, 200, 255));
	background.setSize(sf::Vector2f(WIDTH, HEIGHT));

	sf::CircleShape target(10);
	target.setOrigin(10, 10);
	target.setFillColor(sf::Color(255, 0, 0));
	target.setPosition(WIDTH / 2, HEIGHT / 2);

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;


	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "SFML", sf::Style::Default, settings);

	float time_elapsed = 0;
	sf::Clock clock;

	Drone drone;
	drone.setColor(120, 120, 120);
	//drone.x = (float) WIDTH * randFloat() / 10 - WIDTH / 20;
	//drone.y = (float) HEIGHT * randFloat() / 10 - HEIGHT / 20;

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
		window.draw(background);
		window.draw(target);
		window.draw(drone);
        window.display();
    }

    return 0;
}
