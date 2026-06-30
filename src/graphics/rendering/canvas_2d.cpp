#pragma once
#include "graphics/rendering/canvas_2d.h"
#include "graphics/rendering/renderer.h"
#include "graphics/material.h"
#include "graphics/mesh.h"

#include "graphics/primitive_factory.h"
#include "core/ecs/base_components.h"
#include "core/resources.h"

#include "utility/utils.h"
#include "core/profiler.h"

std::shared_ptr<IRenderer> Canvas2D::renderer;
Arena* Canvas2D::arena;

glm::vec4 Canvas2D::clip = glm::vec4(0.0f);
bool Canvas2D::isClipping = false;
SDL_Color Canvas2D::color = { 255,255,255,255 };
float Canvas2D::depth = 1.0f;

void Canvas2D::init(std::shared_ptr<IRenderer> rendererPtr, Arena* frameArena)
{
	renderer = rendererPtr;
	arena = frameArena;
}

void Canvas2D::drawTest()
{

}

void Canvas2D::drawLine(glm::vec2 pointA, glm::vec2 pointB, SDL_Color color, float lineWidth)
{
	Primitive prim = PrimitiveFactory::createLine(pointA, pointB);
	Mesh* mesh = loadToBuffer<Mesh>();
	mesh->setVerticies(prim.vertices, prim.indices);

	MaterialInstance* mat = loadToBuffer<MaterialInstance>(Resources::sharedMat("primitive"));
	glm::vec4 mainColor = color::SDLColorToVec4(color);
	mat->setProperty("mainColor", mainColor);

	renderer->commandBuffer->submit(RenderCommand{
			.type = CommandType::Line,
			.depth = 1.0f,
			.mesh = mesh,
			.instance = mat,
			.primitive{
				.lineWidth = lineWidth
			}
		});
}

void Canvas2D::drawRect(glm::vec2 origin, glm::vec2 size, SDL_Color color, float lineWidth)
{
	Primitive prim = PrimitiveFactory::createRect(origin, size);
	Mesh* mesh = loadToBuffer<Mesh>();
	mesh->setVerticies(prim.vertices, prim.indices);

	MaterialInstance* mat = loadToBuffer<MaterialInstance>(Resources::sharedMat("primitive"));
	glm::vec4 mainColor = color::SDLColorToVec4(color);
	mat->setProperty("mainColor", mainColor);

	renderer->commandBuffer->submit(RenderCommand{
			.type = CommandType::Rect,
			.depth = 1.0f,
			.mesh = mesh,
			.instance = mat,
			.primitive{
				.lineWidth = lineWidth
			}
		});
}

void Canvas2D::drawWireRect(glm::vec2 origin, glm::vec2 size, SDL_Color color, float lineWidth)
{
	Primitive prim = PrimitiveFactory::createRect(origin, size);
	Mesh* mesh = loadToBuffer<Mesh>();
	mesh->setVerticies(prim.vertices, prim.indices);

	MaterialInstance* mat = loadToBuffer<MaterialInstance>(Resources::sharedMat("primitive"));
	glm::vec4 mainColor = color::SDLColorToVec4(color);
	mat->setProperty("mainColor", mainColor);

	renderer->commandBuffer->submit(RenderCommand{
			.type = CommandType::Rect,
			.depth = 1.0f,
			.mesh = mesh,
			.instance = mat,
			.primitive{
				.lineWidth = lineWidth
			}
		});
}

void Canvas2D::drawWirePoly(std::vector<glm::vec2> points, SDL_Color color, float lineWidth)
{
	Primitive prim = PrimitiveFactory::createPoly(points);
	Mesh* mesh = loadToBuffer<Mesh>();
	mesh->setVerticies(prim.vertices, prim.indices);

	MaterialInstance* mat = loadToBuffer<MaterialInstance>(Resources::sharedMat("primitive"));

	glm::vec4 mainColor = color::SDLColorToVec4(color);
	mat->setProperty("mainColor", mainColor);

	renderer->commandBuffer->submit(RenderCommand{
			.type = CommandType::Polygon,
			.depth = 1.0f,
			.mesh = mesh,
			.instance = mat,
			.primitive{
				.lineWidth = lineWidth
			}
		});
}

void Canvas2D::drawSprite(ecs::Sprite& sprite, ecs::Transform2D& transform)
{
	/*auto texPtr = Resources::texture(sprite.texture);
	if (!texPtr)
	{
		ServiceLocator::get<ILogger>()->error(std::format("Passed expired Texture2D pointer with handle [{}]!", sprite.texture));
		return;
	}*/

	Primitive prim = PrimitiveFactory::createQuad();
	Mesh* mesh = loadToBuffer<Mesh>();
	mesh->setVerticies(prim.vertices, prim.indices);

	glm::mat4 model = transform.model(glm::vec2(sprite.size));
	sprite.material.setProperty("model", model);
	sprite.material.setProperty("time", core::Profiler::instance().getElapsedTime());

	drawImage(mesh, &sprite.material);
}

void Canvas2D::drawImage(Mesh* mesh, MaterialInstance* instance)
{
	renderer->commandBuffer->submit(RenderCommand{
		.type = CommandType::Sprite,
		.depth = depth,
		.mesh = mesh,
		.instance = instance,
		.clip = { 
			.isClipping = isClipping,
			.view = clip
		}
	});
}

void Canvas2D::drawImage(const std::string& handle, ecs::Transform2D transform)
{
	auto texPtr = Resources::texture(handle);
	if (!texPtr)
	{
		ServiceLocator::get<ILogger>()->error(std::format("Passed expired Texture2D pointer with handle [{}]!", handle));
		return;
	}

	float texWidth = texPtr->width;
	float texHeight = texPtr->height;

	Primitive prim = PrimitiveFactory::createQuad();
	Mesh* mesh = loadToBuffer<Mesh>();
	mesh->setVerticies(prim.vertices, prim.indices);

	MaterialInstance* material = loadToBuffer<MaterialInstance>(Resources::sharedMat("def"));

	material->blendMode = BlendMode::Alpha;

	material->setTexture("image", texPtr->ID());
	material->setProperty("useTexture", true);
	material->setProperty("mainColor", glm::vec4(1.0f));

	glm::mat4 model = transform.model(texWidth, texHeight);
	material->setProperty("model", model);

	drawImage(mesh, material);
}

void Canvas2D::drawQuad(const ecs::Transform2D& transform, glm::vec2 size, MaterialInstance* const mat)
{
	Primitive prim = PrimitiveFactory::createQuad();
	Mesh* mesh = loadToBuffer<Mesh>();
	mesh->setVerticies(prim.vertices, prim.indices);

	glm::mat4 model = transform.model(glm::vec2(size));
	mat->setProperty("model", model);

	mat->setProperty("time", core::Profiler::instance().getElapsedTime());

	drawImage(mesh, mat);
}

void Canvas2D::drawQuad(const glm::vec4& rect, MaterialInstance* const mat)
{
	Primitive prim = PrimitiveFactory::createQuad();
	Mesh* mesh = loadToBuffer<Mesh>();
	mesh->setVerticies(prim.vertices, prim.indices);

	ecs::Transform2D transform{ .position = glm::vec3(rect.x, rect.y, 0.0f) };
	glm::mat4 model = transform.model(glm::vec2(rect.z, rect.w));
	mat->setProperty("model", model);

	drawImage(mesh, mat);
}

void Canvas2D::drawText(std::string& text, ecs::Transform2D& transform, int fontSize, SDL_Color color)
{
	MaterialInstance* material = loadToBuffer<MaterialInstance>(Resources::sharedMat("ttf"));
	material->blendMode = BlendMode::Alpha;
	auto* font = Resources::font("Magda-Ld4");
	//material->setTexture("image", font->atlas->id);
	material->setTexture("image", font->atlasId);
	material->setProperty("mainColor", color::SDLColorToVec4(color));

	renderer->commandBuffer->submit(RenderCommand{
		.type = CommandType::Text,
		.depth = depth,
		.instance = material,
		._data = TextData {
			.text = text,
			.fontSize = 14,
			.origin = transform.position
		},
		/*.clip = {
			.isClipping = isClipping,
			.view = clip
		}*/
	});
}