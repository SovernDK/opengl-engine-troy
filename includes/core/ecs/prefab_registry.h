#pragma once
#include <string>
#include <unordered_map>

class PrefabRegistry
{
private:
	//static inline std::unordered_map<std::string, flecs::entity> prefabs;
public:
	
	/*static inline void registerPrefab(std::string key, flecs::entity prefab)
	{
		prefabs[key] = prefab;
	}

	static inline void unregisterPrefab(std::string key)
	{
		prefabs.erase(key);
	}

	static inline flecs::entity getPrefab(std::string key)
	{
		auto it = prefabs.find(key);
		if (it != prefabs.end())
		{
			return it->second;
		}
		else
		{
			SDL_Log("Prefab with key '%s' not found in registry.", key.c_str());
			return flecs::entity();
		}
	}*/
};