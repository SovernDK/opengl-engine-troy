#pragma once
#include "graphics/camera/camera.h"
#include "graphics/rendering/render_queue.h"

class IRenderer
{
public:
    RenderQueue* commandBuffer = nullptr;
public:
    virtual ~IRenderer() = default;

    virtual void init() = 0;
    virtual void render(ICamera& cam) = 0;
    virtual void test() = 0;
};
