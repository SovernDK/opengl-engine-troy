#pragma once
#include "mesh.h"
#include "graphics/graphics.h"

struct SDL_Color;

struct Primitive
{
    std::vector<gpu::Vertex> vertices;
    std::vector<GLuint> indices;
};

class PrimitiveFactory
{
public:
    static Primitive createTriangle();

    static Primitive createLine();
    static Primitive createLine(glm::vec2 start, glm::vec2 end);

    static Primitive createRect();
    static Primitive createRect(glm::vec2 origin, glm::vec2 size);

    static Primitive createQuad();
	static Primitive createQuad(glm::vec2 topLeft, glm::vec2 topRight, glm::vec2 botRight, glm::vec2 botLeft);
	static Primitive createQuad(glm::vec2 topLeft, glm::vec2 topRight, glm::vec2 botRight, glm::vec2 botLeft, gpu::UVRect uvs);

    static Primitive createPoly(std::vector<glm::vec2> points);
};