#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "core/rect.h"
#include <SDL3/SDL_pixels.h>

class ICamera
{
public:
	SDL_Color backgroundColor = { 0, 0, 0, 255 };

	float near = -1.0f;
	float far = 1.0f;
	float zoom = 1.0f;

	glm::vec2 screenRes{ 0.0f };
	Recti m_viewport{ 0 };
protected:
	Recti realViewport{ 0 };
	glm::mat4 m_projection = glm::mat4(1.0f);
	glm::vec3 m_position{ 0.0f };
public:
	virtual ~ICamera() = default;

	virtual void move(glm::vec3) = 0;
	virtual void changeZoom(float change) = 0;
	virtual void resize(int width, int height) = 0;

	virtual void setViewport(glm::vec4 newVp) = 0;
	virtual void setViewport(int x, int y, int w, int h) = 0;

	virtual glm::mat4 projection() const = 0;
	virtual glm::mat4 view() const = 0;
	virtual glm::vec4 bounds() const = 0;
	virtual glm::vec4 viewport() const = 0;
	virtual glm::vec4 bgColor() const = 0;
	virtual bool visible(const glm::vec4 target) const = 0;

	virtual glm::vec2 pointToViewport(const glm::vec2& point) const = 0;
	virtual glm::vec2 inverse(glm::vec2 coords) const = 0;
};