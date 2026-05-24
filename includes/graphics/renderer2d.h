#pragma once
#include "graphics/renderer.h"
#include "ecs/components.h"

#include <filesystem>
#include <string>
#include <map>
#include <memory>

#include "glm/glm.hpp"

struct Renderer2DContext
{
    std::unique_ptr<Renderer> renderer;

    Renderer2DContext()
        : renderer(std::make_unique<Renderer>())
    {
        renderer->init();
    }
};

class Renderer2D
{
public:
    static glm::vec4 currentScissor;
    static uint8_t currentZOrder;

    static Renderer* Init();
    static bool loadTTFont(const std::filesystem::path filePath, const float size = 24);
    static bool loadBitmapFont(const std::string& path);
    static glm::vec2 textSize(const std::string text, std::string fontName, const int fontSize, const int spacing);
    static void drawTriangle(const glm::vec3 triangle, const SDL_Color color);
    static void drawLine(const glm::vec2 start, const glm::vec2 end, float lineWidth, const SDL_Color color);
    static void drawPolygon(const std::vector<glm::vec2> points, float lineWidth, const SDL_Color color);
    static void drawRect(const glm::vec2 origin, const glm::vec2 size, float lineWidth, const SDL_Color color);

    static void drawQuad(const glm::vec4 rect, const SDL_Color color, uint8_t z = 0);
    static void drawSprite(ecs::Transform2D& transform, const ecs::Sprite& sprite);
    static void drawTexture(const std::string handle, const ecs::Transform2D& transform, glm::vec4 rect = glm::vec4(0), std::string material = "main");
    
    static void drawText(std::string text, const glm::vec2 position);
    static void drawText(std::string text, const glm::vec2 position, std::string fontName, const int size, int spacing = 0, SDL_Color c = SDL_Color{255,255,255,255});
    static void drawMultilineText(std::string text, const glm::vec2 position, std::string fontName, int fontSize, int spacing);

    static void render(Camera* cam);
    static void destroy();

    static void setScissor(const glm::vec4 clip)
    {
        currentScissor = clip;
    }

    static void setZOrder(const uint8_t z)
    {
        currentZOrder = z;
    }

    static Renderer2DContext* getContext()
    {
        return context.get();
    }

    static glm::vec4 colorToVec4(const SDL_Color& c);
    static uint8_t floatToUint8(float value);
private:
    static std::unique_ptr<Renderer2DContext> context;
};