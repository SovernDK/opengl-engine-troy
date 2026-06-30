#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <SDL3/SDL_pixels.h>

class ICamera
{
public:
	SDL_Color backgroundColor = { 0, 0, 0, 255 };
public:
	virtual ~ICamera() = default;

	virtual void move(glm::vec3) = 0;
	virtual void changeZoom(float change) = 0;
	virtual void resize(float width, float height) = 0;

	virtual glm::mat4 projection() const = 0;
	virtual glm::mat4 view() const = 0;
	virtual glm::vec4 bounds() const = 0;
	virtual glm::vec4 viewport() const = 0;
	virtual glm::vec4 bgColor() const = 0;
	virtual bool visible(const glm::vec4 target) const = 0;
};