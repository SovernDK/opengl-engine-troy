#pragma once
#include "ecs/ecs.h"
#include "SDL3/SDL_log.h"

using namespace ecs;

// later move impl. back to header
EntityBuilder ECSWorld::create()
{
	EntityId id = idPool.getId();
	entities[id] = Entity{ id };

	return EntityBuilder(id, *this);
}

EntityBuilder ECSWorld::create(const std::string& handle)
{
	EntityId id = idPool.getId();
	lookupTable[handle] = entities[id] = Entity{ id };

	return EntityBuilder(id, *this);
}

Entity& ECSWorld::entity(EntityId id)
{
	if (entities.contains(id))
	{
		return entities.at(id);
	}
	
	std::string msg = std::format("Tried to extract entity by non-existent id {}!", id);
	ServiceLocator::get<ILogger>()->error(CategoryLevel::ECS, msg);
	return entities[0];
}

Entity& ECSWorld::entity(std::string handle)
{
	if (lookupTable.contains(handle))
	{
		return lookupTable.at(handle);
	}

	std::string msg = std::format("Tried to extract entity by non-existent handle {}!", handle);
	ServiceLocator::get<ILogger>()->error(CategoryLevel::ECS, msg);
	return entities[0];
}

void ECSWorld::destroy(EntityId id)
{
	entities.erase(id);
	auto it = std::find_if(lookupTable.begin(), lookupTable.end(), [id](const auto& pair)
	{
		return pair.second.id == id;
	});
	if (it != lookupTable.end()) lookupTable.erase(it);
	
	//erase components
	for (auto& [type, bucketPtr] : components)
	{
		bucketPtr->removeById(id);
	}
}

void ECSWorld::destroy(Entity e)
{
	destroy(e.id);
}

void ECSWorld::each(std::function<void(Entity&)> fn)
{
	for (auto& [_, entity] : entities)
	{
		fn(entity);
	}
}

void ECSWorld::process()
{
	for (auto& sys : systems)
	{
		sys->run(*this);
	}
}

std::vector<std::pair<std::type_index, std::shared_ptr<void>>> ECSWorld::types(EntityId id)
{
	std::vector<std::pair<std::type_index, std::shared_ptr<void>>> out;
	for (auto& [type, bucket] : components)
	{
		out.push_back(std::pair(bucket->type(), bucket->getRaw(id)));
	}

	return out;
}

void ECSWorld::print()
{
	SDL_Log("--- Entities ---");
	for (auto& [id, entity] : entities)
	{
		SDL_Log("Entity: %d", id);
	}
	SDL_Log("--- END ---");

	SDL_Log("--- Components ---");
	for (auto& [type, bucket] : components)
	{
		SDL_Log("Type: %s, size: %d", type.name(), bucket->size());
	}

	SDL_Log("--- Systems ---");
	SDL_Log("size: %d", systems.size());
	SDL_Log("--- END ---");
}

// ---- Entity Builder ----- //
EntityBuilder& EntityBuilder::childOf(EntityId)
{
	return *this;
}

Entity& EntityBuilder::entity()
{
	return world.entity(id);
}