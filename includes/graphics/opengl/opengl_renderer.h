#pragma once
#include "graphics/rendering/renderer.h"
#include "graphics/camera/camera.h"
#include "graphics/rendering/render_queue.h"

class GPUBuffer;

class OpenGlRenderer : public IRenderer
{
public:
    ~OpenGlRenderer() { delete commandBuffer; };

    void init() override;
    void render(ICamera& camera) override;
    void test() override;
private:
    void clear(glm::vec4 color);
    void clear(float r, float g, float b, float a);
    void setViewport(glm::vec4 vp);
    void setViewport(float x, float y, float z, float w);

    void renderPrimitive(ICamera& cam, RenderCommand cmd);
	void renderImage(ICamera& camera, RenderCommand cmd);
	void renderText(ICamera& camera, RenderCommand cmd);
};