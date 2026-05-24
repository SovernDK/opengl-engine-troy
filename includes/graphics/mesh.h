#pragma once
#include <glad/glad.h>
#include "graphics/graphics.h"
#include <vector>

class Mesh
{
public:
	Mesh() = default;

	void setVerticies(const std::vector<gpu::Vertex>& vertices, const std::vector<GLuint>& indicies);

	std::vector<gpu::Vertex> getVertices() const
	{
		return vertices;
	}

	std::vector<GLuint> getIndices() const
	{
		return indicies;
	}
private:
	int vertexCount = 0;
	int indexCount = 0;

	std::vector<gpu::Vertex> vertices;
	std::vector<GLuint> indicies;
};