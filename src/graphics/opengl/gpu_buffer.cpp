#pragma once
#include "graphics/opengl/gpu_buffer.h"
#include "core/profiler.h"

using namespace gpu;

void GPUBuffer::initBuffer(size_t maxVertices, size_t maxIndices)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        maxVertices * sizeof(Vertex),
        nullptr,
        GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        maxIndices * sizeof(GLuint),
        nullptr,
        GL_DYNAMIC_DRAW);

    // --- Vertex layout---
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, //position
        sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, //uv
        sizeof(Vertex),
        (void*)offsetof(Vertex, uv));
    glEnableVertexAttribArray(1);

    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, //uv
    //    sizeof(Vertex),
    //    (void*)offsetof(Vertex, uv));
    //glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void GPUBuffer::uploadDynamicData(const std::vector<gpu::Vertex>& vertices, const std::vector<GLuint>& indices)
{
    indexCount = indices.size();

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex),
        vertices.data(),
        GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(GLuint),
        indices.data(),
        GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
}

void GPUBuffer::uploadStaticData(const std::vector<gpu::Vertex>& vertices, const std::vector<GLuint>& indices)
{
    indexCount = indices.size();

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex),
        vertices.data(),
        GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(GLuint),
        indices.data(),
        GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void GPUBuffer::updateGeometryData(const std::vector<gpu::Vertex>& vertices, const std::vector<GLuint>& indices)
{
    indexCount = indices.size();

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER,
        0,
        vertices.size() * sizeof(Vertex),
        vertices.data());

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
        0,
        indices.size() * sizeof(GLuint),
        indices.data());
}

void GPUBuffer::startDraw() const
{
    glBindVertexArray(VAO);
}

void GPUBuffer::endDraw()
{
    glBindVertexArray(0);
}

void GPUBuffer::clear()
{
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GPUBuffer::draw(const PrimitiveType type) const
{
    switch (type)
    {
        case PrimitiveType::Triangle:
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
            break;
        case PrimitiveType::Line:
            glDrawElements(GL_LINES, indexCount, GL_UNSIGNED_INT, 0);
            break;
        case PrimitiveType::LineStrip:
            glDrawElements(GL_LINE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
            break;
        case PrimitiveType::LineLoop:
            glDrawElements(GL_LINE_LOOP, indexCount, GL_UNSIGNED_INT, 0);
            break;
    }
}

void GPUBuffer::destroy()
{
    if (EBO) glDeleteBuffers(1, &EBO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (VAO) glDeleteVertexArrays(1, &VAO);
}

GPUBuffer::~GPUBuffer()
{
    //Fallback just in case
    if (EBO) glDeleteBuffers(1, &EBO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (VAO) glDeleteVertexArrays(1, &VAO);
}