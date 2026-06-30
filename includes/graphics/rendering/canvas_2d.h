#pragma once
#include "SDL3/SDL.h"

#include "graphics/graphics.h"
#include "graphics/texture.h"

#include "ecs/base_components.h"
#include "glm/glm.hpp"

#include "core/memory/arena.h"
#include "services/service_locator.h"
#include "services/log_service.h"

#include <memory>
#include <string>

class IRenderer;
class Mesh;
class MaterialInstance;

class Canvas2D
{
private:
	static std::shared_ptr<IRenderer> renderer;
	static Arena* arena;

	static glm::vec4 clip;
	static bool isClipping;
	static SDL_Color color;
	static float depth;
public:
	static void init(std::shared_ptr<IRenderer> rendererPtr, Arena* frameArena);

	static void drawTest();

	static void drawLine(glm::vec2 pointA, glm::vec2 pointB, SDL_Color color = { 255,255,255,255 }, float lineWidth = 1);
	static void drawRect(glm::vec2 origin, glm::vec2 size, SDL_Color color = { 255,255,255,255 }, float lineWidth = 1);
	static void drawWireRect(glm::vec2 topLeft, glm::vec2 bottomRight, SDL_Color color = { 255,255,255,255 }, float lineWidth = 1);
	static void drawWirePoly(std::vector<glm::vec2> points, SDL_Color color = { 255,255,255,255 }, float lineWidth = 1);
	static void drawSprite(ecs::Sprite& sprite, ecs::Transform2D& transform);

	static void drawImage(Mesh* mesh, MaterialInstance* const instance);
	static void drawImage(const std::string& handle, ecs::Transform2D transform);

	static void drawQuad(const ecs::Transform2D& transform, glm::vec2 size, MaterialInstance* const mat);
	static void drawQuad(const glm::vec4& rect, MaterialInstance* const mat);

	static void drawText(std::string& text, ecs::Transform2D& transform, int fontSize, SDL_Color color = { 255,255,255,255 });

	static void setClipping(glm::vec4 a_scissor) { clip = a_scissor; isClipping = true; };
	static void setColor(SDL_Color a_color)		 { color = a_color; };
	static void setDepth(float a_depth)			 { depth = a_depth; };
	static void clear()
	{
		clip = glm::vec4(0.0f);
		isClipping = false;
		color = WHITE;
		depth = 1.0f;
	}
private:
	template<typename T, typename... Args>
	static T* loadToBuffer(Args&&... args)
	{
		T* ptr = nullptr;
		try
		{
			ptr = arena->create<T>(std::forward<Args>(args)...);
		}
		catch (std::bad_alloc())
		{
			std::string errorMsg = std::format("Out of memory trying to allocate {}!", typeid(T).name());
			ServiceLocator::get<ILogger>()->error(CategoryLevel::Render, errorMsg);
		};

		return ptr;
	}
};