#pragma once
#include <queue>
#include <vector>
#include <variant>

#include "graphics/camera.h"
#include "graphics/mesh.h"
#include "graphics/gpu_buffer.h"
#include "ecs/components.h"

class Material;
class Texture2D;

enum class RenderType
{
    Sprite, Text, Line, Rect
};

struct SpriteData
{
    Mesh mesh;
    Texture2D* texture;
    gpu::UVRect uvRect;
    glm::mat4 model;
};

struct TextData
{
    std::string text;
    std::string fontName;
    glm::vec2 origin;
    int size;
    int spacing;
};

struct LineData
{
    glm::vec2 start;
    glm::vec2 end;
    float lineWidth;
};

struct RectData
{
    glm::vec2 origin;
    glm::vec2 rect;
    float lineWidth;
};

struct RenderPayload
{
    std::variant<SpriteData, TextData, LineData, RectData> data;
};

struct RenderCommand
{
public:
    RenderType type             = RenderType::Sprite;
    Material* material          = nullptr;
    glm::vec4 color             = glm::vec4(1.0f);
    uint8_t z                   = 0;
    glm::vec4 scissorViewport   = glm::vec4(0);

    std::variant<SpriteData, TextData, LineData, RectData> data;
};

class Renderer
{
public:
	void init();
	void draw(Camera& camera);
	void submit(RenderCommand command);
	void destroy();
private:
    std::vector<RenderCommand> queue;

	GPUBuffer spriteBuffer;
    GPUBuffer textBuffer;
    GPUBuffer lineBuffer;

    void renderSprite(Camera& cam, const RenderCommand& current);
    void renderText(Camera& cam, const RenderCommand& current);
    void renderLine(Camera& cam, const RenderCommand& current);
    void renderWireframePolygon(Camera& cam, const RenderCommand& current);
    void renderWireframeRect(Camera& cam, const RenderCommand& current);
};