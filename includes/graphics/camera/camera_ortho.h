#pragma once
#include "graphics/camera/camera.h"

class OrthoCamera : public ICamera
{
public:
	OrthoCamera() = default;
	~OrthoCamera() = default;

	void move(glm::vec3 change) override;
	void changeZoom(float change) override;
	void resize(int width, int height) override;

	void setViewport(glm::vec4 newVp) override;
	void setViewport(int x, int y, int w, int h) override;

	glm::mat4 projection() const override;
	glm::mat4 view() const override;
	glm::vec4 bounds() const override;
	glm::vec4 viewport() const override;
	glm::vec4 bgColor() const override;

	glm::vec2 pointToViewport(const glm::vec2& point) const override;
	glm::vec2 inverse(glm::vec2 coords) const override;

	bool visible(const glm::vec4 target) const override;
private:
	void recalculateViewport();
};