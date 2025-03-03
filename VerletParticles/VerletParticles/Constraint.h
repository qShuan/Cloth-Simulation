#pragma once
#include "Particle.h"

class Constraint {

public:

	bool broken;
	float initial_length;
	float max_stretch_factor; //Lower the value - more difficult to tear

	Particle* p1;
	Particle* p2;

	Constraint(Particle* p1, Particle* p2) : p1(p1), p2(p2), broken(false), max_stretch_factor(0.25f) {

		sf::Vector2f delta = p2->position - p1->position;
		initial_length = std::sqrt(delta.x * delta.x + delta.y * delta.y);
	}

	// https://www.cs.cmu.edu/afs/cs/academic/class/15462-s13/www/lec_slides/Jakobsen.pdf
	void Update() {

		if (broken) return;

		sf::Vector2f delta = p2->position - p1->position;
		float delta_length = std::sqrt(delta.x * delta.x + delta.y * delta.y);

		if (delta_length > initial_length) {

			if (delta_length * max_stretch_factor > initial_length)
				broken = true;

			const float diff = (delta_length - initial_length) / delta_length;

			if (!p1->pinned)
				p1->position += delta * (1.f / p1->mass) * diff;
			if (!p2->pinned)
				p2->position -= delta * (1.f / p2->mass) * diff;
		}
	}
};