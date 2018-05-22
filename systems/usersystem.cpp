#include "engine.hpp"
#include "usersystem.hpp"

#include "components\body.hpp"
#include "components\colorable.hpp"
#include "components\renderable.hpp"
#include "components\lerpable.hpp"
#include "components\holdable.hpp"

#include "utils.hpp"

using namespace GameComponent;

UserSystem::UserSystem()
{
	
}

void UserSystem::onInit()
{
	eventDispatcher->sink<GameEvent::SpawnBall>().connect(this);
	eventDispatcher->sink<GameEvent::FreeArea>().connect(this);
	eventDispatcher->sink<GameEvent::EscapeFromArea>().connect(this);
}


sf::Vector2f UserSystem::generateRandomDirection()
{
	sf::Vector2f dir((std::rand() % 200) - 100, (std::rand() % 200) - 100);

	return utils::normalize(dir);
}

sf::FloatRect UserSystem::generateRect(const sf::Vector2f& position)
{
	const float size = 200;
	sf::FloatRect rect(position, sf::Vector2f(size, size));
	rect.top -= size * 0.5f;
	rect.left -= size * 0.5f;

	return rect;
}

void UserSystem::receive(const GameEvent::SpawnBall &event)
{
	auto wsize = engine->getWindow().getSize();

	auto position = event.position;
	auto direction = event.direction;

	if (event.position == sf::Vector2f())
	{
		position = sf::Vector2f(std::rand() % wsize.x, std::rand() % wsize.y);
		direction = generateRandomDirection();
	}

	float size = Body::MIN_START_SIZE + static_cast<float>(std::rand() % 2);

	auto entity = registry->create();
	registry->assign<Body>(entity, position, direction * static_cast<float>(Body::MAX_SPEED), size);
	registry->assign<Colorable>(entity);
	registry->assign<Renderable>(entity, position);
}

void UserSystem::receive(const GameEvent::FreeArea &event)
{
	auto border = generateRect(event.position);
	
	registry->view<Body, Colorable>().each([&](auto entity, Body &body, Colorable& colorable)
	{ 
		if (border.contains(body.position))
			if (event.collapse)
			{
				colorable.color = sf::Color(123, 212, 80);
				colorable.decay = sf::seconds(3);
				colorable.timer = sf::Time::Zero;

				registry->assign<Lerpable>(entity, event.position);
			}
			else
				registry->destroy(entity);
	});
}

void UserSystem::receive(const GameEvent::EscapeFromArea &event)
{
	static bool ready = true; // todo remove this dirty flag

	if (!event.hold)
	{
		auto view = registry->view<Body, Colorable, Holdable>();
		for (auto entity : view)
		{
			auto &body = view.get<Body>(entity);
			auto &colorable = view.get<Colorable>(entity);

			body.direction = utils::normalize(body.position - event.position) * static_cast<float>(Body::MAX_SPEED) * 2.f;

			colorable.decay = sf::seconds(3);
			colorable.timer = sf::Time::Zero;
		}

		registry->reset<Holdable>();
		ready = true;
	}
	else if (ready == true)
	{
		auto border = generateRect(event.position);

		registry->view<Body, Colorable>().each([&](auto entity, Body &body, Colorable& colorable)
		{
			if (border.contains(body.position))
			{
				colorable.color = sf::Color(194, 177, 128);
				colorable.decay = sf::seconds(2048);

				registry->assign<Holdable>(entity);
			}

		});

		ready = false;
	}


}