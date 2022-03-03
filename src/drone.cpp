#include "drone.h"

Drone::Drone(const char * path) {
	texture.loadFromFile(path);

	float scale = 0.1;

	sprite.setTexture(texture);
	sprite.setScale(scale, scale);
	sprite.setOrigin(texture.getSize().x / 2, texture.getSize().y / 2);

	square.setSize(sf::Vector2f(100, 100));
	square.setScale(scale, scale);
	square.setOrigin(100 / 2, 100 / 2);

}

void Drone::physics(float delta, bool debug){

	//apply engine power
	float force = (left_power + right_power)*engine_pow;
	speedx += force*sin(angle) / mass * delta;
	speedy += force*cos(angle) / mass * delta;

	angular_velocity += (left_power - right_power)*engine_pow*engine_dist/inertia*delta; //calculate angular acceleration

	//apply physics
	angle += angular_velocity*delta;
	speedy += -10*delta;

	x += speedx*delta;
	y += speedy*delta;


	if(debug){
		std::cout << "x: " << (int) x << ", y:" << (int) y << ", L: " << left_power << ", R: " << right_power << std::endl;
	}
}

void Drone::setPower(float left, float right){

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

void Drone::setColor(int red, int green, int blue){
	square.setFillColor(sf::Color(red, green, blue));
}


void Drone::draw(sf::RenderTarget& target, sf::RenderStates states) const {

	target.draw(sprite, states.transform*getTransform());
	target.draw(square, states.transform*getTransform());
}
