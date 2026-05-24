#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "graphics/graphics.h"

class Camera
{
public:
	Camera() = default;
	Camera(float width, float height);

	void move(glm::vec2 change);
	void changeZoom(float change);

	glm::mat4 getProjection() const
	{
		return projection;
	}

	glm::mat4 getView() const
	{
		glm::mat4 view = glm::mat4(1.0f);
		// Move world opposite to camera
		view = glm::translate(view, glm::vec3(-position, 0.0f));
		// Zoom (scale world)
		view = glm::scale(view, glm::vec3(zoom, zoom, 1.0f));

		return view;
	}

	glm::vec4 getBounds() const
	{
		return glm::vec4 {
			position.x,
			position.y,
			width / zoom,
			height / zoom
		};
	}

	glm::vec4 getViewport()
	{
		return glm::vec4{
			position.x,
			position.y,
			width,
			height
		};
	}
	// right now its only AABB handle rotating
	// maybe add bounding circle approximation
	bool isVisible(const glm::vec4 target)
	{
		return gpu::AABB(getBounds(), target);
	}

private:
	glm::mat4 projection = glm::mat4(1);
	float width = 0.0;
	float height = 0.0;
	float near = -1.0f;
	float far = 1.0f;

	float zoom = 1.0f;
	glm::vec2 position = { 0.0f, 0.0f };
};