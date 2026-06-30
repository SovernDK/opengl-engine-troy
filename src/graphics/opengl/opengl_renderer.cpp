#pragma once
#include "graphics/opengl/opengl_renderer.h"
#include "graphics/opengl/gpu_buffer.h"
#include "graphics/mesh.h"
#include "graphics/material.h"
#include "core/profiler.h"

#include <variant>
#include <graphics/primitive_factory.h>
#include <resources.h>

void OpenGlRenderer::init()
{
	commandBuffer = new RenderQueue();
}

void OpenGlRenderer::render(ICamera& camera)
{
	setViewport(camera.viewport());
	clear(camera.bgColor());

	commandBuffer->sort();

	int drawCalls = 0;

	for (auto& cmd : commandBuffer->buffer)
	{
		switch (cmd.instance->blendMode)
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

		if (cmd.clip.isClipping)
		{
			glm::vec4 clip = cmd.clip.view;
			glEnable(GL_SCISSOR_TEST);
			glScissor(clip.x, clip.y, clip.z, clip.w);
		}
		else glDisable(GL_SCISSOR_TEST);

		if (cmd.type == CommandType::Sprite)
			renderImage(camera, cmd);
		else if (cmd.type == CommandType::Text)
			renderText(camera, cmd);
		else
			renderPrimitive(camera, cmd);

		drawCalls++;
	}

	core::Profiler::instance().setDrawCalls(drawCalls);
	commandBuffer->clear();
}

void OpenGlRenderer::test()
{

}

void OpenGlRenderer::clear(glm::vec4 color)
{
	clear(color.x, color.y, color.z, color.w);
}

void OpenGlRenderer::clear(float r, float g, float b, float a)
{
	glDisable(GL_SCISSOR_TEST);

	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGlRenderer::setViewport(glm::vec4 vp)
{
	setViewport(vp.x, vp.y, vp.z, vp.w);
}

void OpenGlRenderer::setViewport(float x, float y, float z, float w)
{
	glViewport(x, y, z, w);
}

void OpenGlRenderer::renderImage(ICamera& camera, RenderCommand cmd)
{
	GPUBuffer buffer;
	buffer.initBuffer(cmd.mesh->vertexCount(), cmd.mesh->indexCount());
	buffer.uploadStaticData(cmd.mesh->vertices, cmd.mesh->indices);

	buffer.startDraw();

	cmd.instance->setProperty("projection", camera.projection() * camera.view());
	cmd.instance->bind();

	buffer.draw(PrimitiveType::Triangle);

	cmd.instance->unbind();

	buffer.endDraw();
	buffer.destroy();
}

void OpenGlRenderer::renderPrimitive(ICamera& camera, RenderCommand cmd)
{
	GPUBuffer buffer;
	buffer.initBuffer(cmd.mesh->vertexCount(), cmd.mesh->indexCount());
	buffer.uploadStaticData(cmd.mesh->vertices, cmd.mesh->indices);

	buffer.startDraw();

	glLineWidth(cmd.primitive.lineWidth);

	cmd.instance->setProperty("projection", camera.projection() * camera.view());
	cmd.instance->bind();

	PrimitiveType current = PrimitiveType::Line;
	switch (cmd.type)
	{
		case CommandType::Line:
			current = PrimitiveType::Line; break;
		case CommandType::Rect:
			current = PrimitiveType::LineLoop; break;
		case CommandType::Triangle:
			current = PrimitiveType::LineLoop; break;
		case CommandType::Polygon:
			current = PrimitiveType::LineLoop; break;
		default:
			current = PrimitiveType::Line;
	}

	buffer.draw(current);

	cmd.instance->unbind();

	buffer.endDraw();
	buffer.destroy();
}

void OpenGlRenderer::renderText(ICamera& camera, RenderCommand cmd)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	auto& data = std::get<TextData>(cmd._data);
	auto mesh = PrimitiveFactory::createQuad();

	float x = data.origin.x;
	float y = data.origin.y;
	float scale = 1;

	GPUBuffer buffer;
	buffer.initBuffer(mesh.vertices.size(), mesh.indices.size());
	buffer.uploadDynamicData(mesh.vertices, mesh.indices);

	cmd.instance->setProperty("projection", camera.projection() * camera.view());
	cmd.instance->bind();

	buffer.startDraw();
	
	auto* font = Resources::font("Magda-Ld4");

	std::string::const_iterator c;
	for (c = data.text.begin(); c != data.text.end(); c++)
	{
		Glyph ch = font->_glyphs[*c];

		float xpos = x + ch.bearing.x;
		float ypos = y - ch.bearing.y;

		float w = ch.size.x;
		float h = ch.size.y;

		// update VBO for each character
		mesh = PrimitiveFactory::createQuad(
			glm::vec2(xpos, ypos + h),
			glm::vec2(xpos + w, ypos + h),
			glm::vec2(xpos + w, ypos),
			glm::vec2(xpos, ypos),
			ch.uvs
		);

		buffer.updateGeometryData(mesh.vertices, mesh.indices);
		buffer.draw(PrimitiveType::Triangle);

		x += (ch.advance >> 6);// +current.spacing * scale;
	}

	cmd.instance->unbind();

	buffer.endDraw();
	buffer.destroy();
}