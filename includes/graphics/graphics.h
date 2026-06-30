#pragma once
#include "glm/glm.hpp"
#define TO_GL_COORDS(rect, viewportHeight) glm::vec4(rect.x, viewportHeight - rect.y - rect.w, rect.z, rect.w)
#define AABB(a, b) gpu::checkAABB(a, b)

#define TRANSPARENT SDL_Color{ 255, 255, 255, 0 }
#define WHITE		SDL_Color{ 255, 255, 255, 255 }
#define BLACK		SDL_Color{ 0, 0, 0, 255 }
#define RED			SDL_Color{ 255, 0, 0, 255 }
#define GREEN		SDL_Color{ 0, 255, 0, 255 }
#define BLUE		SDL_Color{ 0, 0, 255, 255 }

namespace gpu
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec2 uv;
	};

	struct UVRect
	{
		float u0 = 0.0f,
			  v0 = 0.0f, 
			  u1 = 1.0f, 
			  v1 = 1.0f;

		UVRect() { u0 = v0 = u1 = v1 = 0.0f; }
		UVRect(float v) { u0 = v0 = u1 = v1 = v; }
		UVRect(float u0, float v0, float u1, float v1) 
		{ 
			this->u0 = u0;
			this->v0 = v0;
			this->u1 = u1;
			this->v1 = v1;
		}
	};

	inline UVRect GetUV(int x, int y, int w, int h, int texW, int texH)
	{
		float u0 = x / (float)texW;
		float v0 = y / (float)texH;
		float u1 = (x + w) / (float)texW;
		float v1 = (y + h) / (float)texH;

		return { u0, v0, u1, v1 };
	}

	inline bool checkAABB(const glm::vec4 a, const glm::vec4 b)
	{
		return !(a.x + a.z < b.x ||   // a is left of b
			a.x > b.x + b.z ||   // a is right of b
			a.y + a.w < b.y ||   // a is above b
			a.y > b.y + b.w);    // a is below b
	}
}