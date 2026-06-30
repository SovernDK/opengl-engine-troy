#pragma once
#include <string>

namespace ecs
{
	struct Clan
	{
		std::string name = "";
		glm::vec2 coordinate{ 0 };
	};

	struct Person
	{
		std::string name = "";
		int age = 0;
	};

	struct Settlement
	{
		std::string name = "";
	};

	struct WorldMap
	{
		
	};
}