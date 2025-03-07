#include "Solver.h"

#include <iostream>

#define ROWS 100
#define COLS 150

#define DISTANCE_BETWEEN_PARTICLES 6
#define PULL_RADIUS 50

void Init(Solver& solver) {

	for (int y = 0; y < ROWS; y++) {
		for (int x = 0; x < COLS; x++) {

			solver.AddParticle({ (float)((WINDOW_WIDTH / 4.5f) + x * DISTANCE_BETWEEN_PARTICLES), (float)(y * DISTANCE_BETWEEN_PARTICLES) }, 3.f, y == 0);
		}
	}

	for (int y = 0; y < ROWS; y++) {
		for (int x = 0; x < COLS; x++) {

			if (x < COLS - 1)
				solver.AddConstraint(&solver.particles[COLS * y + x], &solver.particles[COLS * y + x + 1]);
			if (y < ROWS - 1)
				solver.AddConstraint(&solver.particles[COLS * y + x], &solver.particles[COLS * (y + 1) + x]);
		}
	}

	solver.UpdateVASize();
	solver.InitVAColors();
}

void ClearSolver(Solver& solver) {

	solver.GetConstraints().clear();
	solver.GetParticles().clear();
	solver.GetVA().clear();
}

void Reset(Solver& solver) {

	ClearSolver(solver);

	Init(solver);
}

void MousePull(std::vector<Particle*>& particles, std::vector<sf::Vector2f>& directions) {

	for (int i = 0; i < particles.size(); i++) {

		Particle* particle = particles[i];
		sf::Vector2f& direction = directions[i];

		particle->Accelerate(direction * 1000.f);
	}
}

int main(){

	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Cloth Simulation", sf::Style::Titlebar | sf::Style::Close);

	window.setFramerateLimit(60);

	Solver solver;
	Init(solver);

	std::vector<Particle*> pulledParticles;
	std::vector<sf::Vector2f> pulledDirections;

	sf::Font font;
	if (!font.loadFromFile("Fonts/arial.ttf"))
	{
		std::cerr << "Couldn't load the font file." << std::endl;
		return -1;
	}

	sf::Text text;
	text.setFont(font);
	text.setString("LMB - Tear\nRMB - Pull\nR - Reset\nP - Pause");

	bool isLeftClick = false, isRightClick = false;

	while (window.isOpen()) {

		sf::Event e;

		while (window.pollEvent(e)) {

			if (e.type == e.Closed()) {
				window.close();
			}
			else if (e.type == e.KeyPressed) {

				if (e.key.code == sf::Keyboard::R) {

					Reset(solver);
				}
				else if (e.key.code == sf::Keyboard::P) {

					solver.SetPaused(solver.GetPaused() ? false : true);
				}
			}
			else if (e.type == e.MouseButtonPressed) {

				if (e.key.code == sf::Mouse::Right) {

					isRightClick = true;

					sf::Vector2f localPosition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));

					for (auto& particle : solver.GetParticles()) {
			
						sf::Vector2f dir = localPosition - particle.position;
						float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

						if (dist <= PULL_RADIUS) {

							pulledParticles.push_back(&particle);
							pulledDirections.push_back(dir);
						}
					}
				}

				if (e.key.code == sf::Mouse::Left) {

					isLeftClick = true;
				}
			}
			else if (e.type == e.MouseButtonReleased) {

				if (e.key.code == sf::Mouse::Right) {

					isRightClick = false;

					pulledParticles.clear();
					pulledDirections.clear();
				}

				if (e.key.code == sf::Mouse::Left) {

					isLeftClick = false;
				}
			}
		}

		if (isRightClick)
		{
			sf::Vector2f localPosition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));

			for (int i = 0; i < pulledParticles.size(); i++) {

				pulledDirections[i] = localPosition - pulledParticles[i]->position;
			}

			MousePull(pulledParticles, pulledDirections);
		}

		if (isLeftClick)
		{
			sf::Vector2f localPosition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));

			solver.Tear(localPosition, solver.GetConstraints());
		}

		solver.UpdateSolver();

		//Clear
		window.clear();

		window.draw(text);

		//Draw
		solver.Render(&window);

		window.display();

	}
	return 0;
}

