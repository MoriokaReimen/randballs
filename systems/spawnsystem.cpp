#include "engine.hpp"
#include "spawnsystem.hpp"

#include "components\body.hpp"
#include "components\colorable.hpp"
#include "components\renderable.hpp"

using namespace GameComponent;

SpawnSystem::SpawnSystem()
{
	
}

void SpawnSystem::onInit()
{
	eventDispatcher->sink<GameEvent::SpawnBall>().connect(this);
}


sf::Vector2f SpawnSystem::generateRandomDirection()
{
	float x = (std::rand() % 200) - 100;
	float y = (std::rand() % 200) - 100;

	if (x != 0 || y != 0)
	{
		float len = sqrt((x * x) + (y * y));
		x /= len;
		y /= len;
	}

	return sf::Vector2f(x, y);
}

void SpawnSystem::receive(const GameEvent::SpawnBall &event)
{
	auto size = engine->getWindow().getSize();
	sf::Vector2f position = sf::Vector2f(std::rand() % size.x, std::rand() % size.y);

	auto entity = registry->create();

	registry->assign<Body>(entity, position, generateRandomDirection());
	registry->assign<Colorable>(entity);
	registry->assign<Renderable>(entity, position);
}
