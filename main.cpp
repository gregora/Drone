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

float SCALE = 1; //texture scale
float PPM = 10; //pixels per meter

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
	font.loadFromFile("/usr/share/fonts/truetype/tlwg/Garuda.ttf");
	text.setFont(font);
	text.setFillColor(sf::Color(0, 0, 0));
	text.setPosition(WIDTH - 100, 10);

	sf::Text ltext;
	ltext.setFont(font);
	ltext.setFillColor(sf::Color(255, 255, 255));
	ltext.setPosition(WIDTH / 2 - 80, HEIGHT / 2 - 100);

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
	target.setOrigin(10*SCALE, 10*SCALE);
	target.setFillColor(sf::Color(255, 0, 0));
	target.setPosition(WIDTH / 2, HEIGHT / 2);

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;


	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "SFML", sf::Style::Default, settings);
	sf::RenderTexture* renderTexture = new sf::RenderTexture();
	renderTexture->create(WIDTH, HEIGHT);

	window.clear();
	ltext.setString("Loading 0%");
	window.draw(ltext);
	window.display();

	float time_elapsed = 0;
	sf::Clock clock;



	int DRONE_NUMBER = 1000;
	Drone drones[DRONE_NUMBER];
	for(int i = 0; i < DRONE_NUMBER; i++){
		window.clear();
		ltext.setString("Loading " + to_string(i*100 / DRONE_NUMBER) + "%");
		window.draw(ltext);
		window.display();

		Drone * d = new Drone();
		d -> setScale(SCALE, SCALE);
		d -> setColor(i*307 %  255, i*353 % 255, i*397  % 255);
		d -> x = (float) WIDTH * randFloat() / (SCALE*PPM) - WIDTH / (2*SCALE*PPM);
		d -> y = (float) HEIGHT * randFloat() / (SCALE*PPM) - HEIGHT / (2*SCALE*PPM);

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

		drones[0].setPower(left, right);


		renderTexture -> clear();
		renderTexture -> draw(background);
		renderTexture -> draw(target);

		for(int i = 0; i < DRONE_NUMBER; i++){

			if(i >= 1 || !ENABLE_AUTO || (left ==  0 && right == 0)){
				float r,l;
				controller2(drones[i].x, drones[i].y, drones[i].speedx, drones[i].speedy, drones[i].angle, drones[i].angular_velocity, &l, &r);
				drones[i].setPower(l, r);
			}

			drones[i].physics(delta);
			drones[i].setPosition(WIDTH/2 + drones[i].x*SCALE*PPM, HEIGHT/2 - drones[i].y*SCALE*PPM);
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
