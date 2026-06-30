#pragma once

namespace ecs
{
	using EntityId = long;

	class Entity
	{
	public:
		EntityId id = 0;
	public:
		Entity() = default;
		~Entity() = default;
		Entity(EntityId id) : id(id) {};
	};
}