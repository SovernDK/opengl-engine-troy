#pragma once
#include "graphics/camera/camera_ortho.h"
#include "graphics/graphics.h"
#include "utility/utils.h"

void OrthoCamera::move(glm::vec3 change)
{
	m_position += change;
}

void OrthoCamera::changeZoom(float change)
{
	zoom = glm::clamp(zoom + change, 0.2f, 5.0f);
}

void OrthoCamera::resize(float width, float height)
{
	this->width = width;
	this->height = height;

	m_projection = glm::ortho(0.0f, this->width, this->height, 0.0f, near, far);
}

glm::mat4 OrthoCamera::projection() const
{
	return m_projection;
}

glm::mat4 OrthoCamera::view() const
{
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, -m_position);
	view = glm::scale(view, glm::vec3(zoom, zoom, 1.0f));

	return view;
}

glm::vec4 OrthoCamera::bounds() const
{
	return glm::vec4 {
			m_position.x,
			m_position.y,
			width / zoom,
			height / zoom
	};
}

glm::vec4 OrthoCamera::viewport() const
{
	return glm::vec4{
			m_position.x,
			m_position.y,
			width,
			height
	};
}

glm::vec4 OrthoCamera::bgColor() const
{
	return color::SDLColorToVec4(backgroundColor);
}

bool OrthoCamera::visible(const glm::vec4 target) const
{
	return AABB(bounds(), target);
}