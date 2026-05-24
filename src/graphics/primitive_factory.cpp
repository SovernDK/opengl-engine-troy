#pragma once
#include "graphics/primitive_factory.h"

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL.h>

using namespace std;
using namespace gpu;
using namespace glm;

Primitive PrimitiveFactory::createTriangle()
{
    vector<Vertex> verts = {
        { { 0.0f,  0.5f, 0.0f }, {1.0, 1.0, 1.0}, { 0.5f, 1.0f } },
        { {-0.5f, -0.5f, 0.0f }, {1.0, 1.0, 1.0}, { 0.5f, 1.0f } },
        { { 0.5f, -0.5f, 0.0f }, {1.0, 1.0, 1.0}, { 0.5f, 1.0f } },
    };

    std::vector<GLuint> indices = {
      0, 1, 2
    };

    return { verts, indices };
}

Primitive PrimitiveFactory::createLine()
{
    vector<Vertex> verts = {
        { { 0.0f,  0.0f, 0.0f }, {1,1,1}, {0.0f, 0.0f} },
        { { 1.0f,  1.0f, 0.0f }, {1,1,1}, {0.0f, 0.0f} }, 
    };

    vector<GLuint> indices = {
        0, 1
    };

    return { verts, indices };
}

Primitive PrimitiveFactory::createLine(vec2 start, vec2 end, SDL_Color c)
{
    glm::vec3 color = {
        (float)c.r,
        (float)c.g,
        (float)c.b
    };

    vector<Vertex> verts = {
        { { start.x, start.y, 0.0f }, color, {0.0f, 0.0f} },
        { { end.x,   end.y,   0.0f }, color, {0.0f, 0.0f} },
    };

    vector<GLuint> indices = {
        0, 1
    };

    return { verts, indices };
}

Primitive PrimitiveFactory::createRect()
{
    vector<Vertex> verts = {
        { { 0.0f, 1.0f, 0.0f }, {1,1,1}, {0.0f, 1.0f} }, // top-left
        { { 1.0f, 1.0f, 0.0f }, {1,1,1}, {1.0f, 1.0f} }, // top-right
        { { 1.0f, 0.0f, 0.0f }, {1,1,1}, {1.0f, 0.0f} }, // bottom-right
        { { 0.0f, 0.0f, 0.0f }, {1,1,1}, {0.0f, 0.0f} }, // bottom-left
    };

    vector<GLuint> indices = {
        0, 1, 2, 3
    };

    return { verts, indices };
}

Primitive PrimitiveFactory::createRect(glm::vec2 origin, glm::vec2 size, SDL_Color c)
{
    glm::vec3 color = {
        (float)c.r,
        (float)c.g,
        (float)c.b
    };

    float x = origin.x;
    float y = origin.y;
    float w = size.x;
    float h = size.y;

    vector<Vertex> verts = {
        { { x,     y,       0.0f }, color, {0.0f, 1.0f} }, // top-left
        { { x + w, y,       0.0f }, color, {1.0f, 1.0f} }, // top-right
        { { x + w, y + h,   0.0f }, color, {1.0f, 0.0f} }, // bottom-right
        { { x,     y + h,   0.0f }, color, {0.0f, 0.0f} }, // bottom-left
    };

    vector<GLuint> indices = {
        0, 1, 2, 3
    };

    return { verts, indices };
}

Primitive PrimitiveFactory::createQuad()
{
    vector<Vertex> verts = {
        { { 0.0f,  1.0f, 0.0f }, {1,1,1}, {0.0f, 1.0f} }, // 0 - top-left
        { { 1.0f,  1.0f, 0.0f }, {1,1,1}, {1.0f, 1.0f} }, // 1 - top-right
        { { 1.0f,  0.0f, 0.0f }, {1,1,1}, {1.0f, 0.0f} }, // 2 - bottom-right
        { { 0.0f,  0.0f, 0.0f }, {1,1,1}, {0.0f, 0.0f} }, // 3 - bottom-left
    };

    vector<GLuint> indices = {
        0, 1, 3,   // first triangle
        1, 2, 3    //second triangle
    };

    return { verts, indices };
}

Primitive PrimitiveFactory::createQuad(vec2 topLeft, vec2 topRight, vec2 botRight, vec2 botLeft)
{
    vector<Vertex> verts = {
        { { topLeft.x,  topLeft.y,  1.0f }, {1,1,1}, {0.0f, 1.0f} }, // 0 - top-left
        { { topRight.x, topRight.y, 1.0f }, {1,1,1}, {1.0f, 1.0f} }, // 1 - top-right
        { { botRight.x, botRight.y, 1.0f }, {1,1,1}, {1.0f, 0.0f} }, // 2 - bottom-right
        { { botLeft.x,  botLeft.y,  1.0f }, {1,1,1}, {0.0f, 0.0f} }, // 3 - bottom-left
    };

    vector<GLuint> indices = {
        0, 1, 3,   // first triangle
        1, 2, 3    //second triangle
    };

    return { verts, indices };
}