#pragma once
#include <unordered_map>
#include <typeindex>
#include <vector>
#include <string>
#include <memory>
#include <format>
#include <functional>
#include <iostream>
#include <tuple>

#include "ecs/base_components.h"
#include "utility/id_pool.h"

#include "services/service_locator.h"
#include "services/log_service.h"

namespace ecs
{
#pragma region Definition
	class ECSWorld;
	class EntityBuilder;

	using EntityId = long;

	class Entity
	{
	public:
		EntityId id = 0;
		ECSWorld* world;
	public:
		~Entity() = default;

		Entity() = default;
		Entity(ECSWorld* world, EntityId id) : id(id), world(world) {};

		Entity(const Entity&) = delete;
		Entity& operator=(const Entity&) = delete;
		Entity(Entity&&) = default;
		Entity& operator=(Entity&&) = default;

		template<typename T>
		Entity& add(T copyObj);

		template<typename T>
		const T* const get();

		template<typename T>
		T* const getMod();
	};

	struct ITypeBucket
	{
		virtual ~ITypeBucket() = default;

		virtual size_t size() const = 0;
		virtual std::type_index type() = 0;

		virtual void removeById(EntityId id) = 0;

		virtual void* getRawPtr(EntityId id) = 0;
		virtual void forEach(std::function<void(EntityId, void*)> fn) = 0;
	};

	template<typename T>
	struct TypeBucket : ITypeBucket
	{
		~TypeBucket() = default;

		std::unordered_map<EntityId, std::unique_ptr<T>> items;
		size_t size() const override { return items.size(); }
		std::type_index type() override { return typeid(T); }

		void removeById(EntityId id) override;
		void* getRawPtr(EntityId id) override;
		void forEach(std::function<void(EntityId, void*)> fn) override;
	};

	struct ISystem
	{
		virtual ~ISystem() = default;
		virtual void run(ECSWorld& world) = 0;
	};

	template<typename... Ts>
	struct System : ISystem
	{
		using Fn = std::function<void(Entity&, Ts&...)>;
		Fn logic;

		System(Fn logic) : logic(logic) {};

		void run(ECSWorld& world) override;
	};

	class ECSWorld
	{
	private:
		std::unordered_map<EntityId, Entity> entities;
		std::unordered_map<std::string, Entity*> lookupTable;
		std::unordered_map<std::type_index, std::shared_ptr<ITypeBucket>> components;

		std::vector<std::unique_ptr<ISystem>> systems;

		IdPool<EntityId> idPool;
	public:
		ECSWorld() {
			entities.emplace(0, Entity(this, 0));
		};
		~ECSWorld() = default;

		ECSWorld(const ECSWorld&) = delete;
		ECSWorld& operator=(const ECSWorld&) = delete;
	public:
		Entity& create();
		Entity& create(const std::string& handle);

		Entity& entity(EntityId id);
		Entity& entity(std::string handle);

		void destroy(EntityId id);
		void destroy(Entity e);

		void each(std::function<void(Entity&)> fn);
		void process();
		auto types(EntityId id) -> std::vector<std::pair<std::type_index, void*>>;

		void quit();
		void print();
		std::string name(EntityId id);

		template<typename T>
		void addComponent(EntityId id, T copyObj);

		template<typename T>
		void addComponent(EntityId id);

		template<typename T>
		const T* const get(EntityId id);

		template<typename T>
		T* const getMod(EntityId id);

		template<typename... Ts, typename Fn>
		void view(Fn fn);

		template<typename... Ts, typename Fn>
		void system(Fn fn);

		template<typename T>
		T* isType(const std::pair<std::type_index, void*>& entry);

		template<typename T>
		TypeBucket<T>* getType();
	};
#pragma endregion

#pragma region Implementation
	template<typename T>
	inline void TypeBucket<T>::removeById(EntityId id)
	{
		items.erase(id);
	}

	template<typename T>
	inline void* TypeBucket<T>::getRawPtr(EntityId id)
	{
		auto it = items.find(id);
		return it != items.end() ? it->second.get() : nullptr;
	}

	template<typename T>
	inline void TypeBucket<T>::forEach(std::function<void(EntityId, void*)> fn)
	{
		for (auto& [id, ptr] : items)
			fn(id, ptr.get());
	}

	#pragma region Entity
	template<typename T>
	inline Entity& Entity::add(T copyObj)
	{
		world->addComponent<T>(id, copyObj);
		return *this;
	}

	template<typename T>
	const T* const Entity::get()
	{
		return world->get<T>(id);
	}

	template<typename T>
	T* const Entity::getMod()
	{
		return world->getMod<T>(id);
	}
	#pragma endregion

	#pragma region System
	template<typename... Ts>
	inline void System<Ts...>::run(ECSWorld& world)
	{
		world.view<Ts...>(logic);
	}
	#pragma endregion
	
	#pragma region ECS World
	inline Entity& ECSWorld::create()
	{
		EntityId id = idPool.next();
		auto [it, emplaced] = entities.emplace(id, Entity(this, id));

		return it->second;
	}

	inline Entity& ECSWorld::create(const std::string& handle)
	{
		EntityId id = idPool.next();

		auto [it, inserted] = entities.emplace(id, Entity(this, id));
		lookupTable.emplace(handle, &it->second);

		return it->second;
	}

	inline Entity& ECSWorld::entity(EntityId id)
	{
		if (entities.contains(id))
		{
			return entities.at(id);
		}

		std::string msg = std::format("Tried to extract entity by non-existent id {}!", id);
		ServiceLocator::get<ILogger>()->error(CategoryLevel::ECS, msg);
		return entities[0];
	}

	inline Entity& ECSWorld::entity(std::string handle)
	{
		auto it = lookupTable.find(handle);
		if (it != lookupTable.end())
		{
			return entities.at(it->second->id);
		}

		std::string msg = std::format("Tried to extract entity by non-existent handle {}!", handle);
		ServiceLocator::get<ILogger>()->error(CategoryLevel::ECS, msg);
		return entities[0];
	}

	inline void ECSWorld::destroy(EntityId id)
	{
		auto it = std::find_if(lookupTable.begin(), lookupTable.end(), [id](const auto& pair)
		{
			return pair.second->id == id;
		});
		if (it != lookupTable.end()) lookupTable.erase(it);

		entities.erase(id);

		for (auto& [type, bucketPtr] : components)
		{
			bucketPtr->removeById(id);
		}
	}

	inline void ECSWorld::destroy(Entity e)
	{
		destroy(e.id);
	}

	inline void ECSWorld::each(std::function<void(Entity&)> fn)
	{
		for (auto& [_, entity] : entities)
		{
			fn(entity);
		}
	}

	inline void ECSWorld::process()
	{
		for (auto& sys : systems)
		{
			sys->run(*this);
		}
	}

	inline auto ECSWorld::types(EntityId id) -> std::vector<std::pair<std::type_index, void*>>
	{
		std::vector<std::pair<std::type_index, void*>> out;
		for (auto& [type, bucket] : components)
		{
			out.push_back(std::pair(bucket->type(), bucket->getRawPtr(id)));
		}

		return out;
	}

	inline void ECSWorld::quit()
	{
		components.clear();
		lookupTable.clear();
		entities.clear();
	}

	inline void ECSWorld::print()
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

	inline std::string ECSWorld::name(EntityId id)
	{
		for (auto& [name, e] : lookupTable)
		{
			if (e->id == id)
			{
				return name;
			}
		}

		return {};
	}

	template<typename T>
	inline void ECSWorld::addComponent(EntityId id, T copyObj)
	{
		if (id == 0)
		{
			std::string logMsg = std::format("Cannot add component {} to entity with invalid id 0!", typeid(T).name());
			ServiceLocator::get<ILogger>()->error(CategoryLevel::ECS, logMsg);
			return;
		}

		auto [it, _] = components.try_emplace(typeid(T), std::make_unique<TypeBucket<T>>());
		auto* bucket = static_cast<TypeBucket<T>*>(it->second.get());
		bucket->items[id] = std::make_unique<T>(copyObj);
	};

	template<typename T>
	inline void ECSWorld::addComponent(EntityId id)
	{
		if (id == 0)
		{
			std::string logMsg = std::format("Cannot add component {} to entity with invalid id 0!", typeid(T).name());
			ServiceLocator::get<ILogger>()->error(CategoryLevel::ECS, logMsg);
			return;
		}

		auto [it, _] = components.try_emplace(typeid(T), std::make_unique<TypeBucket<T>>());
		auto* bucket = static_cast<TypeBucket<T>*>(it->second.get());
		bucket->items[id] = std::make_unique<T>();
	};

	template<typename T>
	inline const T* const ECSWorld::get(EntityId id)
	{
		auto bucket = std::static_pointer_cast<TypeBucket<T>>(components.at(typeid(T)));
		if (bucket->items.contains(id))
		{
			return bucket->items.at(id).get();
		}

		std::string msg = std::format("Tried to get component {} (for id({}) but it doesn't exist!", typeid(bucket).name(), id);
		ServiceLocator::get<ILogger>()->error(CategoryLevel::ECS, msg);
		return nullptr;
	}

	template<typename T>
	inline T* const ECSWorld::getMod(EntityId id)
	{
		auto bucket = std::static_pointer_cast<TypeBucket<T>>(components.at(typeid(T)));
		if (bucket->items.contains(id))
		{
			return bucket->items.at(id).get();
		}

		std::string msg = std::format("Tried to get component {} (for id({}) but it doesn't exist!", typeid(bucket).name(), id);
		ServiceLocator::get<ILogger>()->error(CategoryLevel::ECS, msg);
		return nullptr;
	}

	template<typename... Ts, typename Fn>
	inline void ECSWorld::view(Fn fn)
	{
		static_assert(std::is_invocable_v<Fn, Entity&, Ts&...>,
			"view<Ts...>: lambda signature must match (Entity&, Ts&...)");

		auto buckets = std::make_tuple(getType<Ts>()...);
		if ((... || (std::get<TypeBucket<Ts>*>(buckets) == nullptr)))
			return;

		auto* first = std::get<0>(buckets);

		for (auto& [id, _] : first->items)
		{
			bool hasAll = (std::get<TypeBucket<Ts>*>(buckets)->items.contains(id) && ...);
			if (!hasAll) continue;

			fn(entity(id), *static_cast<Ts*>(std::get<TypeBucket<Ts>*>(buckets)->items.at(id).get())...);
		}
	}

	template<typename... Ts, typename Fn>
	inline void ECSWorld::system(Fn fn)
	{
		static_assert(std::is_invocable_v<Fn, Entity&, Ts&...>,
			"view<Ts...>: lambda signature must match (Entity&, Ts&...)");
		systems.emplace_back(std::make_unique<System<Ts...>>(fn));
	}

	template<typename T>
	inline T* ECSWorld::isType(const std::pair<std::type_index, void*>& entry)
	{
		if (entry.first == std::type_index(typeid(T)))
			return static_cast<T*>(entry.second);
		return nullptr;
	}

	template<typename T>
	inline TypeBucket<T>* ECSWorld::getType()
	{
		auto it = components.find(typeid(T));
		if (it == components.end()) return nullptr;
		return static_cast<TypeBucket<T>*>(it->second.get());
	}
	#pragma endregion

#pragma endregion
}