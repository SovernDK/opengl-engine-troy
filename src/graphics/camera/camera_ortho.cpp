#pragma once
#include "graphics/camera/camera_ortho.h"
#include "graphics/graphics.h"
#include "utility/utils.h"

#pragma region Public functions
void OrthoCamera::move(glm::vec3 change)
{
	m_position += change;
}

void OrthoCamera::changeZoom(float change)
{
	zoom = glm::clamp(zoom + change, 0.2f, 5.0f);
}

void OrthoCamera::resize(int width, int height)
{
	m_viewport.w = width;
	m_viewport.h = height;

	m_projection = glm::ortho(0.0f, screenRes.x, screenRes.y, 0.0f, near, far);

	recalculateViewport();
}

void OrthoCamera::setViewport(glm::vec4 newVp)
{
	m_viewport.x = newVp.x;
	m_viewport.y = newVp.y;
	m_viewport.w = newVp.z;
	m_viewport.h = newVp.w;

	recalculateViewport();
}

void OrthoCamera::setViewport(int x, int y, int w, int h)
{
	m_viewport.x = x;
	m_viewport.y = y;
	m_viewport.w = w;
	m_viewport.h = h;

	recalculateViewport();
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
	return glm::vec4 
	{
		m_position.x,
		m_position.y,
		m_viewport.w / zoom,
		m_viewport.h / zoom
	};
}

glm::vec4 OrthoCamera::viewport() const
{
	return realViewport.vector();
}

glm::vec2 OrthoCamera::pointToViewport(const glm::vec2& point) const
{
	float relX = point.x - realViewport.x;
	float relY = point.y - (screenRes.y - realViewport.h - realViewport.y);

	float scaleX = screenRes.x / realViewport.w;
	float scaleY = screenRes.y / realViewport.h;

	relX *= scaleX;
	relY *= scaleY;

	return glm::vec2(relX, relY);
}

glm::vec2 OrthoCamera::inverse(glm::vec2 coords) const
{
	glm::vec4 c = glm::vec4(coords, 0.0f, 1.0f);
	c = glm::inverse(projection() * view()) * c; 
	return glm::vec2(c);
}

glm::vec4 OrthoCamera::bgColor() const
{
	return color::SDLColorToVec4(backgroundColor);
}

bool OrthoCamera::visible(const glm::vec4 target) const
{
	return AABB(bounds(), target);
}
#pragma endregion Public functions

#pragma region Private functions
/* Handles letterboxing to preserve screen aspect */
void OrthoCamera::recalculateViewport()
{
	realViewport = m_viewport;

	float aspect = (float)m_viewport.w / (float)m_viewport.h;
	float screenAspect = screenRes.x / screenRes.y;

	if (aspect > screenAspect)
	{
		realViewport.w *= m_viewport.h / screenRes.y;
	}
	else if (aspect < screenAspect)
	{
		realViewport.h *= m_viewport.w / screenRes.x;
	}

	realViewport.x += (screenRes.x - realViewport.w) / 2;
	realViewport.y += (screenRes.y - realViewport.h) / 2;
}
#pragma endregion Private functions