#pragma once
#include "graphics/renderer.h"
#include "graphics/mesh.h"
#include "graphics/material.h"
#include "graphics/primitive_factory.h"
#include "graphics/ttfonts.h"
#include "graphics/camera.h"

#include "core/resources.h"
#include "core/profiler.h"

#include <algorithm>
#include <gtc/type_ptr.hpp>

using namespace glm;
using namespace gpu;

void Renderer::init()
{
	Primitive quad = PrimitiveFactory::createQuad();
	Primitive line = PrimitiveFactory::createLine();

	spriteBuffer.initBuffer(quad.vertices.size(), quad.indices.size());
	spriteBuffer.uploadStaticData(quad.vertices, quad.indices);

	textBuffer.initBuffer(quad.vertices.size(), quad.indices.size());
	textBuffer.uploadDynamicData(quad.vertices, quad.indices);

	lineBuffer.initBuffer(line.vertices.size(), line.indices.size());
	lineBuffer.uploadDynamicData(line.vertices, line.indices);

	SDL_Log("Renderer initialized");
}

void Renderer::draw(Camera& camera)
{
	int drawCalls = 0;

	std::sort(queue.begin(), queue.end(),
		[](const RenderCommand& a, const RenderCommand& b)
		{
			return a.z < b.z;
		});

	glm::vec4 vp = camera.getViewport();
	glViewport(vp.x, vp.y, vp.z, vp.w);

	Camera* prevCamera;
	for (const auto& current : queue)
	{
		switch (current.material->getBlend())
		{
		case BlendMode::None:
			glDisable(GL_BLEND);
			break;

		case BlendMode::Alpha:
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;

		case BlendMode::Additive:
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			break;
		}

		if (current.scissorViewport != vec4(0))
		{
			vec4 v = current.scissorViewport;
			glEnable(GL_SCISSOR_TEST);
			glScissor(v.x, v.y, v.z, v.w);
		}

		switch (current.type)
		{
		case RenderType::Sprite:
			renderSprite(camera, current);
			break;
		case RenderType::Text:
			renderText(camera, current);
			break;
		case RenderType::Line:
			renderLine(camera, current);
			break;
		case RenderType::Rect:
			renderWireframeRect(camera, current);
			break;
		}

		if (current.scissorViewport != vec4(0)) 
			glDisable(GL_SCISSOR_TEST);

		drawCalls++;
	}
	queue.clear();

	core::Profiler::Instance().setDrawCalls(drawCalls);
}

void Renderer::submit(RenderCommand command)
{
	assert(command.material != NULL);

	queue.push_back(command);
}

void Renderer::destroy()
{
	queue.clear();
	spriteBuffer.destroy();
	textBuffer.destroy();
}

void Renderer::renderSprite(Camera& cam, const RenderCommand& current)
{
	SpriteData data = std::get<SpriteData>(current.data);

	spriteBuffer.startDraw();

	vec2 offset = { data.uvRect.u0, data.uvRect.v0 };
	vec2 scale = { data.uvRect.u1 - data.uvRect.u0, data.uvRect.v1 - data.uvRect.v0 };

	current.material->activate();

	current.material->setUVOffset(offset);
	current.material->setUvScale(scale);

	current.material->setColor(current.color);
	if (data.texture != nullptr)
		current.material->setTexture(data.texture);
	else
		current.material->setTexture(nullptr);

	current.material->setModel(data.model);
	current.material->setProjection(cam.getProjection() * cam.getView());
	current.material->bind();

	spriteBuffer.draw(PrimitiveType::Triangle);
	spriteBuffer.endDraw();
}

void Renderer::renderText(Camera& cam, const RenderCommand& current)
{
	TextData data = std::get<TextData>(current.data);
	// Enable alpha
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float x = data.origin.x;
	float y = data.origin.y;
	float scale = 1;

	current.material->activate();
	current.material->setProjection(cam.getProjection() * cam.getView());
	current.material->setColor(current.color);

	textBuffer.startDraw();

	std::vector<Vertex> glyphVertices(4);
	Primitive quad = PrimitiveFactory::createQuad(vec2(0), vec2(0), vec2(0), vec2(0));
	std::vector<GLuint> glyphIndices = quad.indices;

	// iterate through all characters
	std::string::const_iterator c;
	for (c = data.text.begin(); c != data.text.end(); c++)
	{
		TTFGlyph ch = Resources::fonts[data.fontName].glyphs[data.size][*c];

		float xpos = x + ch.bearing.x * scale;
		float ypos = y - ch.bearing.y * scale;

		float w = ch.size.x * scale;
		float h = ch.size.y * scale;

		// update VBO for each character
		glyphVertices = PrimitiveFactory::createQuad(vec2(xpos, ypos + h),
			vec2(xpos + w, ypos + h),
			vec2(xpos + w, ypos),
			vec2(xpos, ypos)).vertices;
		// render glyph texture over quad
		current.material->setTexture(ch.texture);
		current.material->bind();

		textBuffer.updateGeometryData(glyphVertices, glyphIndices);
		textBuffer.draw(PrimitiveType::Triangle);

		x += (ch.advance >> 6) + data.spacing * scale;
	}

	textBuffer.endDraw();
	//Check if this method make sense
	textBuffer.clear();
}

void Renderer::renderLine(Camera& cam, const RenderCommand& current)
{
	LineData data = std::get<LineData>(current.data);

	glLineWidth(data.lineWidth);

	current.material->activate();
	current.material->setProjection(cam.getProjection() * cam.getView());

	lineBuffer.startDraw();

	glm::vec4 vc = current.color;
	SDL_Color color = SDL_Color{ (uint8_t)vc.x, (uint8_t)vc.y, (uint8_t)vc.z, (uint8_t)vc.w };
	Primitive line = PrimitiveFactory::createLine(data.start, data.end, color);

	current.material->bind();

	lineBuffer.updateGeometryData(line.vertices, line.indices);
	lineBuffer.draw(PrimitiveType::Line);

	lineBuffer.endDraw();
	lineBuffer.clear();
}

void Renderer::renderWireframePolygon(Camera& cam, const RenderCommand& current)
{
	//Take points and make from them verticies and indicies
}

void Renderer::renderWireframeRect(Camera& cam, const RenderCommand& current)
{
	RectData data = std::get<RectData>(current.data);

	glLineWidth(data.lineWidth);

	GPUBuffer buffer{};

	glm::vec4 vc = current.color;
	SDL_Color color = SDL_Color{ (uint8_t)vc.x, (uint8_t)vc.y, (uint8_t)vc.z, (uint8_t)vc.w };
	Primitive rect = PrimitiveFactory::createRect(data.origin, data.rect, color);

	buffer.initBuffer(rect.vertices.size(), rect.indices.size());
	buffer.uploadStaticData(rect.vertices, rect.indices);

	current.material->activate();
	current.material->setProjection(cam.getProjection() * cam.getView());

	buffer.startDraw();

	current.material->bind();

	buffer.draw(PrimitiveType::LineLoop);

	buffer.endDraw();
	buffer.clear();
}