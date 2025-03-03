#pragma once
#include "SFML/Graphics.hpp"

class Particle {

public:

	bool pinned;
	float mass = 10.f; // Mass or Radius

	sf::Vector2f position;
	sf::Vector2f last_position;
	sf::Vector2f acceleration;

	Particle(sf::Vector2f position = { 0, 0 }, float mass = 10.f, bool pinned = false)
		: mass(mass), 
		position(position), 
		last_position(position), 
		pinned(pinned),
		acceleration({0.f, 0.f})
	{
		//Simulation explodes on lower mass
		if (mass < 3.f)
			mass = 3.f;
	}


	void Update(float dt) {

		if (pinned) return;

		sf::Vector2f velocity = position - last_position;
		last_position = position;
		position = position + velocity + acceleration * (dt * dt);

		acceleration = {};
	}
	
	void Accelerate(sf::Vector2f force) {

		if(!pinned)
			acceleration += force;
	}

	void SetVelocity(sf::Vector2f velocity, float velocity_loss) {

		last_position = position - (velocity * velocity_loss);
	}

	sf::Vector2f GetVelocity() {

		return position - last_position;
	}
};