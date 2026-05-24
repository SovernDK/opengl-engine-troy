#pragma once
#include <glad/glad.h>
#include <vector>

#include <graphics/graphics.h>

enum DrawType
{
	Dynamic, Static
};

enum PrimitiveType
{
	Triangle, Line, LineStrip, LineLoop
};

class GPUBuffer
{
public:
	GLuint VAO, VBO, EBO;
	int indexCount;

	GPUBuffer() = default;
	~GPUBuffer();

	void initBuffer(size_t maxVertices, size_t maxIndices);
	void uploadDynamicData(const std::vector<gpu::Vertex>& vertices, const std::vector<GLuint>& indices);
	void uploadStaticData(const std::vector<gpu::Vertex>& vertices, const std::vector<GLuint>& indices);
	void updateGeometryData(const std::vector<gpu::Vertex>& vertices, const std::vector<GLuint>& indices);
	void startDraw();
	void endDraw();
	void clear();
	void draw(PrimitiveType type);
	void destroy();
};
