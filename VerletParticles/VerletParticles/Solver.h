#pragma once
#include "Constraint.h"
#include <vector>

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900

class Solver {

private:

	bool paused = false;
	float m_dt = 1.f / 60.f;
	int sub_steps = 8;

	sf::Vector2f gravity = { 0.f, 500.f };
	sf::VertexArray va;
	std::vector<Particle> particles;
	std::vector<Constraint> constraints;

	Constraint* FindClosestConstraint(sf::Vector2f mouse_pos, const std::vector<Constraint>& constraints) {

		Constraint* closest = nullptr;

		float min_distance = 10.f;

		for (auto& constraint : constraints) {

			float dst = PointSegmentDistance(mouse_pos, constraint.p1->position, constraint.p2->position);

			if (dst < min_distance) {

				min_distance = dst;
				closest = (Constraint*)(&constraint);
			}
		}

		return closest;
	}

	// https://www.youtube.com/watch?v=egmZJU-1zPU
	float PointSegmentDistance(sf::Vector2f mouse_pos, sf::Vector2f p1_pos, sf::Vector2f p2_pos) {

		sf::Vector2f AB = p2_pos - p1_pos;
		sf::Vector2f AC = mouse_pos - p1_pos;

		float segmentLengthSquared = AB.x * AB.x + AB.y * AB.y;
		if (segmentLengthSquared == 0.0f) return std::sqrt(AC.x * AC.x + AC.y * AC.y); // p1 and p2 are the same point

		// Projection factor
		float proj = (AC.x * AB.x + AC.y * AB.y) / segmentLengthSquared;
		proj = std::max(0.0f, std::min(1.0f, proj)); // Clamp to segment

		// Find the closest point on the segment
		sf::Vector2f closestPoint = p1_pos + AB * proj;

		// Return distance to the closest point
		sf::Vector2f diff = mouse_pos - closestPoint;
		return std::sqrt(diff.x * diff.x + diff.y * diff.y);
	}

	void ApplyGravity() {

		for (auto& particle : particles)
			particle.Accelerate(gravity * particle.mass);
	}

	void ApplyBorder() {

		for (auto& particle : particles) {

			const float velocity_loss_factor = 0.25f;
			const float dampening = 0.6f;
			const sf::Vector2f position = particle.position;


			//Horizontal
			if (position.x < particle.mass || position.x + particle.mass > WINDOW_WIDTH) {

				particle.position.x = position.x < particle.mass ? particle.mass : WINDOW_WIDTH - particle.mass;
				particle.SetVelocity({ -particle.GetVelocity().x, particle.GetVelocity().y * dampening }, velocity_loss_factor);
			}

			//Vertical
			if (position.y < particle.mass || position.y + particle.mass > WINDOW_HEIGHT) {

				particle.position.y = position.y < particle.mass ? particle.mass : WINDOW_HEIGHT - particle.mass;
				particle.SetVelocity({ particle.GetVelocity().x * dampening, -particle.GetVelocity().y }, velocity_loss_factor);
			}
		}
	}

	void ApplyAirResistance() {

		for (auto& particle : particles) {

			particle.acceleration -= particle.GetVelocity() * 500.f;
		}
	}

	void UpdateObjects(float dt) {

		for (auto& particle : particles)
			particle.Update(dt);
	}

	void UpdateConstraints() {

		for (auto& constraint : constraints)
			constraint.Update();
	}

	void RemoveConstraint(int index) {

		constraints.erase(constraints.begin() + index);
	}

	void RemoveBrokenConstraints() {

		for (int i = 0; i < constraints.size(); i++) {

			if (constraints[i].broken)
				RemoveConstraint(i);
		}
	}

public:

	Solver() : va(sf::Lines) {}

	void SetPaused() {

		paused = !paused;
	}

	void AddParticle(sf::Vector2f position, float mass, bool is_pinned = false) {

		Particle particle(position, mass, is_pinned);

		particles.push_back(particle);
	}

	void AddConstraint(Particle* p1, Particle* p2) {

		constraints.emplace_back(p1, p2);
	}

	void Tear(sf::Vector2f mouse_pos, const std::vector<Constraint>& constraints) {

		Constraint* closest = FindClosestConstraint(mouse_pos, constraints);

		if (!closest) return;

		closest->broken = true;
	}

	void UpdateSolver() {

		if (paused) return;

		const float sub_dt = m_dt / static_cast<float>(sub_steps);
		for (int i = 0; i < sub_steps; i++) {
			ApplyGravity();
			ApplyAirResistance();
			UpdateObjects(sub_dt);
			UpdateConstraints();
			ApplyBorder();
		}

		RemoveBrokenConstraints();
	}

	int UpdateVASize() {

		const int number_of_constraints = static_cast<int>(constraints.size());
		va.resize(2 * number_of_constraints);

		return number_of_constraints;
	}

	void InitVAColors() {

		sf::Color cloth_color = sf::Color(166, 138, 119);

		const int number_of_constraints = static_cast<int>(constraints.size());

		for (int i = 0; i < number_of_constraints; i++) {

			va[2 * i].color = cloth_color;
			va[2 * i + 1].color = cloth_color;
		}
	}

	void UpdateVA() {

		const int number_of_constraints = UpdateVASize();

		for (int i = 0; i < number_of_constraints; i++) {

			Constraint& current_constraint = constraints[i];

			va[2 * i].position = current_constraint.p1->position;
			va[2 * i + 1].position = current_constraint.p2->position;
		}
	}

	void Render(sf::RenderWindow* window) {

		UpdateVA();

		window->draw(va);
	}

	std::vector<Constraint>& GetConstraints() {

		return constraints;
	}

	std::vector<Particle>& GetParticles() {

		return particles;
	}

	sf::VertexArray& GetVA() {

		return va;
	}
};