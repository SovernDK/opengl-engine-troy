#pragma once
#include <deque>
#include <variant>
#include <vector>
#include <string>

#include "glm/glm.hpp"

class Mesh;
class Material;
class MaterialInstance;
class Texture2D;

class IRenderer;

struct PrimitiveData
{
	float depth = 0;
	float lineWidth = 1.0f;
};

struct TextData
{
	std::string text; //std::string_view
	int fontSize = 0;
	glm::vec2 origin = glm::vec2(0.0f);
};

using RenderData = std::variant<PrimitiveData, TextData>;

enum class CommandType
{
	Sprite, Text, Triangle, Rect, Line, Polygon
};

struct RenderCommand
{
	CommandType type			= CommandType::Sprite;
	float depth					= 0;
	Mesh* mesh					= nullptr;
	MaterialInstance* instance	= nullptr;
	RenderData _data;

	union
	{
		struct // PRIMITIVE
		{
			float lineWidth = 0;

		} primitive;

		struct //Scissors
		{
			bool isClipping = false;
			glm::vec4 view = glm::vec4(0.0f);

		} clip;
	};
};

class RenderQueue
{
public:
	std::deque<RenderCommand> buffer;
public:
	void submit(RenderCommand cmd);
	void sort();
	void clear();
};