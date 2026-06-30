#pragma once
#include "graphics/camera/camera.h"

class OrthoCamera : public ICamera
{
private:
	glm::mat4 m_projection = glm::mat4(1.0f);
	glm::vec3 m_position   = { 0.0f, 0.0f, 0.0f };

	float width	 = 0.0f;
	float height = 0.0f;
	float near	 = -1.0f;
	float far	 = 1.0f;
	float zoom	 = 1.0f;
public:
	OrthoCamera() = default;
	~OrthoCamera() = default;

	void move(glm::vec3 change) override;
	void changeZoom(float change) override;
	void resize(float width, float height) override;

	glm::mat4 projection() const override;
	glm::mat4 view() const override;
	glm::vec4 bounds() const override;
	glm::vec4 viewport() const override;
	glm::vec4 bgColor() const override;

	bool visible(const glm::vec4 target) const override;
};