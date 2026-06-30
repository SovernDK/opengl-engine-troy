#pragma once
#include <glad/glad.h>
#include "graphics/graphics.h"
#include <vector>

class Mesh
{
public:
	std::vector<gpu::Vertex> vertices{};
	std::vector<GLuint> indices{};
public:
	Mesh() = default;

	void setVerticies(const std::vector<gpu::Vertex>& vertices, const std::vector<GLuint>& indices)
	{
		this->vertices = vertices;
		this->indices = indices;
	}

	int vertexCount() const { return vertices.size(); }
	int indexCount() const { return indices.size(); }
};