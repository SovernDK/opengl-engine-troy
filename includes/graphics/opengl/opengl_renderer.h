#pragma once
#include "graphics/rendering/renderer.h"
#include "graphics/camera/camera.h"
#include "graphics/rendering/render_queue.h"
#include "graphics/opengl/opengl_framebuffer.h"
#include "graphics/opengl/gpu_buffer.h"

class GPUBuffer;
class MaterialInstance;

class OpenGlRenderer : public IRenderer
{
private:
	Framebuffer framebuffer;
	MaterialInstance* screenMaterial = nullptr;
	GPUBuffer screenBuffer;

	int drawCalls = 0;
public:
	OpenGlRenderer() = default;
	OpenGlRenderer(const OpenGlRenderer&) = delete;
	OpenGlRenderer& operator=(const OpenGlRenderer&) = delete;
	OpenGlRenderer(const OpenGlRenderer&&) = delete;
	OpenGlRenderer& operator=(const OpenGlRenderer&&) = delete;
    ~OpenGlRenderer();

    void init(int screenWidth, int screenHeight) override;
	void render(ICamera& camera) override;
	void resize(int width, int height) override;
private:
    void clear(glm::vec4 color);
    void clear(float r, float g, float b, float a);
    void setViewport(glm::vec4 vp);
    void setViewport(float x, float y, float z, float w);

	void blend(RenderCommand cmd);
    void clip(RenderCommand cmd);

	void renderToBuffer(ICamera& camera);
	void renderToTexture(MaterialInstance& mat);

    void renderPrimitive(ICamera& cam, RenderCommand cmd);
	void renderImage(ICamera& camera, RenderCommand cmd);
	void renderText(ICamera& camera, RenderCommand cmd);
};