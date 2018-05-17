#pragma once

#include "basesystem.hpp"

#include "events\spawnball.hpp"

class SpawnSystem : public BaseSystem
{
	void onInit() override;

	sf::Vector2f generateRandomDirection();
public:
	SpawnSystem();

	void receive(const GameEvent::SpawnBall &);
};