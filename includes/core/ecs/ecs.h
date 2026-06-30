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

#include "ecs/entity.h"
#include "ecs/base_components.h"
#include "utility/id_pool.h"

#include "services/service_locator.h"
#include "services/log_service.h"


namespace ecs
{
	class ECSWorld;
	class EntityBuilder;

	struct ITypeBucket
	{
		virtual ~ITypeBucket() = default;

		virtual size_t size() const = 0;
		virtual std::type_index type() = 0;

		/*virtual void add(EntityId id) = 0;
		virtual void add(EntityId id, std::shared_ptr<void> value) = 0;*/

		virtual void removeById(EntityId id) = 0;

		virtual std::shared_ptr<void> getRaw(EntityId id) = 0;
		virtual void forEach(std::function<void(EntityId, std::shared_ptr<void>)> fn) = 0;
	};

	template<typename T>
	struct TypeBucket : ITypeBucket
	{
		~TypeBucket() = default;
		//Change to unique pointers and return raw ponters for opertions
		std::unordered_map<EntityId, std::shared_ptr<T>> items;
		size_t size() const override { return items.size(); }
		std::type_index type() override { return typeid(T); }

		void removeById(EntityId id) override
		{
			items.erase(id);
		}

		std::shared_ptr<void> getRaw(EntityId id) override
		{
			auto it = items.find(id);
			return it != items.end() ? it->second : nullptr;
		}

		void forEach(std::function<void(EntityId, std::shared_ptr<void>)> fn) override
		{
			for (auto& [id, ptr] : items)
				fn(id, ptr);
		}
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
		std::unordered_map<std::string, Entity> lookupTable;
		std::unordered_map<std::type_index, std::shared_ptr<ITypeBucket>> components;

		//systems
		std::vector<std::unique_ptr<ISystem>> systems;

		IdPool<EntityId> idPool;
	public:
		ECSWorld() {
			entities[0] = Entity();
		};
		~ECSWorld() = default;

		ECSWorld(const ECSWorld&) = delete;
		ECSWorld& operator=(const ECSWorld&) = delete;
	public:
		EntityBuilder create();
		EntityBuilder create(const std::string& handle);

		Entity& entity(EntityId id);
		Entity& entity(std::string handle);

		void destroy(EntityId id);
		void destroy(Entity e);

		void each(std::function<void(Entity&)> fn);
		void process();
		std::vector<std::pair<std::type_index, std::shared_ptr<void>>> types(EntityId id);

		void quit()
		{
			components.clear();
			lookupTable.clear();
			entities.clear();
		}

		void print();

		template<typename T>
		void addComponent(EntityId id, T copyObj)
		{
			if (id == 0)
			{
				std::string logMsg = std::format("Cannot add component {} to entity with invalid id 0!", typeid(T).name());
				ServiceLocator::get<ILogger>()->error(CategoryLevel::ECS, logMsg);
				return;
			}

			auto [it, _] = components.try_emplace(typeid(T), std::make_unique<TypeBucket<T>>());
			auto* bucket = static_cast<TypeBucket<T>*>(it->second.get());
			bucket->items[id] = std::make_shared<T>(copyObj);
		};

		template<typename T>
		void addComponent(EntityId id)
		{
			if (id == 0)
			{
				std::string logMsg = std::format("Cannot add component {} to entity with invalid id 0!", typeid(T).name());
				ServiceLocator::get<ILogger>()->error(CategoryLevel::ECS, logMsg);
				return;
			}

			auto [it, _] = components.try_emplace(typeid(T), std::make_unique<TypeBucket<T>>());
			auto* bucket = static_cast<TypeBucket<T>*>(it->second.get());
			bucket->items[id] = std::make_shared<T>();
		};

		template<typename T>
		const std::shared_ptr<const  T> get(EntityId id)
		{
			auto bucket = std::static_pointer_cast<TypeBucket<T>>(components.at(typeid(T)));
			if (bucket->items.contains(id))
			{
				return bucket->items.at(id);
			}

			std::string msg = std::format("Tried to get component {} (for id({}) but it doesn't exist!", typeid(bucket).name(), id);
			ServiceLocator::get<ILogger>()->error(CategoryLevel::ECS, msg);
			return std::make_shared<T>();
		}

		template<typename T>
		const std::shared_ptr<T> getMut(EntityId id)
		{
			auto bucket = std::static_pointer_cast<TypeBucket<T>>(components.at(typeid(T)));
			if (bucket->items.contains(id))
			{
				return bucket->items.at(id);
			}

			std::string msg = std::format("Tried to get component {} (for id({}) but it doesn't exist!", typeid(bucket).name(), id);
			ServiceLocator::get<ILogger>()->error(CategoryLevel::ECS, msg);
			return std::make_shared<T>();
		}

		template<typename... Ts, typename Fn>
		void view(Fn fn)
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
		void system(Fn fn)
		{
			static_assert(std::is_invocable_v<Fn, Entity&, Ts&...>,
				"view<Ts...>: lambda signature must match (Entity&, Ts&...)");
			systems.emplace_back(std::make_unique<System<Ts...>>(fn));
		}

		template<typename T>
		std::shared_ptr<T> isType(const std::pair<std::type_index, std::shared_ptr<void>>& entry)
		{
			if (entry.first == std::type_index(typeid(T)))
				return std::static_pointer_cast<T>(entry.second);
			return nullptr;
		}

		template<typename T>
		TypeBucket<T>* getType()
		{
			auto it = components.find(typeid(T));
			if(it == components.end()) return nullptr;
			return static_cast<TypeBucket<T>*>(it->second.get());
		}
	};

	class EntityBuilder
	{
	private:
		EntityId id;
		ECSWorld& world;
	public:
		EntityBuilder(EntityId id, ECSWorld& world) : id(id), world(world) {};

		template<typename T>
		EntityBuilder& addComponent()
		{
			world.addComponent<T>(id);
			return *this;
		}

		template<typename T>
		EntityBuilder& addComponent(T copyObj)
		{
			world.addComponent<T>(id, copyObj);
			return *this;
		}

		EntityBuilder& childOf(EntityId);
		Entity& entity();
	};

	template<typename... Ts>
	void System<Ts...>::run(ECSWorld& world)
	{
		world.view<Ts...>(logic);
	}
}