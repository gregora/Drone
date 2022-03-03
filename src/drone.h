#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>


class Drone : public sf::Transformable, public sf::Drawable {

	public:
		float x = 0;
		float y = 0;

		float speedx = 0;
		float speedy = 0;

		float angle = 0;
		float angular_velocity = 0;

		float inertia = 0.1;
		float mass = 0.3;
		float engine_dist = 0.2;
		float engine_pow = 3;

		Drone(const char * path = "img/drone.png");

		void physics(float delta, bool debug = false);
		void setPower(float left, float right);
		void setColor(int red, int green, int blue);

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	private:
		sf::Sprite sprite;
		sf::Texture texture;
		sf::RectangleShape square;

		float left_power = 0;
		float right_power = 0;
};
