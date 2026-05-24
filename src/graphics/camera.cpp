#pragma once
#include "graphics/camera.h"

using namespace glm;

Camera::Camera(float width, float height)
{
	this->width = width;
	this->height = height;

	projection = glm::ortho(0.0f, this->width, this->height, 0.0f, near, far);
}

void Camera::move(vec2 change)
{
	position += change;
}

void Camera::changeZoom(float change)
{
	zoom = glm::clamp(zoom + change, 0.2f, 5.0f);
}