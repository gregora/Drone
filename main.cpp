#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <math.h>
#include <string.h>
#include "src/drone.h"

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

int main(int argc, char* argv[]){

	sf::Text text;
	sf::Font font;
	font.loadFromFile("/usr/share/fonts/truetype/freefont/FreeSerif.ttf");
	text.setFont(font);
	text.setColor(sf::Color(0, 0, 0));
	text.setPosition(WIDTH - 100, 10);

	bool record = false;
	bool noscreen = false;
	bool show_fps = false;

	for(int i = 0; i < argc; i++){
		if(strcmp(argv[i], "-record") == 0){
			record = true;
		}else if(strcmp(argv[i], "-noscreen") == 0){
			noscreen = true;
		}else if(strcmp(argv[i], "-fps") == 0){
			show_fps = true;
		}
	}

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
	sf::RenderTexture* renderTexture = new sf::RenderTexture();
	renderTexture->create(WIDTH, HEIGHT);

	float time_elapsed = 0;
	sf::Clock clock;

	Drone drone;
	drone.setColor(120, 120, 120);
	drone.setScale(10, 10);
	drone.x = (float) WIDTH * randFloat() / 10 - WIDTH / 20;
	drone.y = (float) HEIGHT * randFloat() / 10 - HEIGHT / 20;


	int DRONE_NUMBER = 5;
	Drone drones[DRONE_NUMBER];

	for(int i = 0; i < DRONE_NUMBER; i++){
		Drone * d = new Drone();
		d -> setScale(10, 10);
		d -> setColor(i*307 %  255, i*353 % 255, i*397  % 255);
		d -> x = (float) WIDTH * randFloat() / 10 - WIDTH / 20;
		d -> y = (float) HEIGHT * randFloat() / 10 - HEIGHT / 20;
		d -> controller = &controller2;

		drones[i] = *d;
	}


	float left = 0;
	float right = 0;
	int mode = 0;
	int frame = 0;

	while (window.isOpen())
	{
		sf::Time dt = clock.restart();
		float delta = dt.asSeconds();
		if(record){
			delta = 0.0166;
		}
		if(frame == 0){
			delta = 0.0166;
		}
		time_elapsed += delta;
		frame++;


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
				drone.controller = &controller2;
			}else{
				drone.controller = nullptr;
			}
		}

		renderTexture -> clear();
		renderTexture -> draw(background);
		renderTexture -> draw(target);

		drone.physics(delta, false);
		drone.setPosition(WIDTH/2 + drone.x*10, HEIGHT/2 - drone.y*10);
		drone.setRotation(drone.angle * 180/3.14);
		renderTexture -> draw(drone);

		for(int i = 0; i < DRONE_NUMBER; i++){
			drones[i].physics(delta);
			drones[i].setPosition(WIDTH/2 + drones[i].x*10, HEIGHT/2 - drones[i].y*10);
			drones[i].setRotation(drones[i].angle * 180/3.14);
			renderTexture -> draw(drones[i]);
		}

		if(show_fps){
			if(frame % 20 == 1){
				text.setString(to_string((int) (1 / delta)) + " FPS");
			}
			renderTexture -> draw(text);
		}

		renderTexture -> display();
		sf::Texture screen_texture = renderTexture -> getTexture();

		if(record){
			renderTexture -> getTexture().copyToImage().saveToFile("frames/" + to_string(frame) + ".png");
			cout << "Frame " + to_string(frame) + " rendered!" << endl;
		}

		window.clear();
		window.draw(sf::Sprite(renderTexture -> getTexture()));
		window.display();
	}

	return 0;
}
