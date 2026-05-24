#pragma once
#include "glm/glm.hpp"

namespace gpu
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 color;
		glm::vec2 uv;
	};

	struct UVRect
	{
		float u0, v0, u1, v1;
	};

	inline UVRect GetUV(int x, int y, int w, int h, int texW, int texH)
	{
		float u0 = x / (float)texW;
		float v0 = y / (float)texH;
		float u1 = (x + w) / (float)texW;
		float v1 = (y + h) / (float)texH;

		return { u0, v0, u1, v1 };
	}

	inline bool AABB(const glm::vec4 a, const glm::vec4 b)
	{
		return !(a.x + a.z < b.x ||   // a is left of b
			a.x > b.x + b.z ||   // a is right of b
			a.y + a.w < b.y ||   // a is above b
			a.y > b.y + b.w);    // a is below b
	}
}