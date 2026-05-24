#pragma once
#include "core/resources.h"
#include "ecs/components.h"

#include "graphics/renderer2d.h"
#include "graphics/primitive_factory.h"
#include "graphics/graphics.h"

#include "utility/utils.h"
#include "utility/file_util.h"

#include "core/config.h"

#include <gtc/type_ptr.hpp>

using namespace ecs;
using namespace glm;
using namespace std;
using namespace gpu;
using namespace core;

std::unique_ptr<Renderer2DContext> Renderer2D::context;

glm::vec4 Renderer2D::currentScissor = glm::vec4(0);
uint8_t Renderer2D::currentZOrder = 0;

bool rendererInitialized = false;

Renderer* Renderer2D::Init()
{
	context = make_unique<Renderer2DContext>();
	ShaderProgram* shader			= Resources::loadShader("vertex.glsl", "fragment.glsl", "main");
	ShaderProgram* fontShader		= Resources::loadShader("ttf_vert.glsl", "ttf_frag.glsl", "ttf");
	ShaderProgram* primitiveShader  = Resources::loadShader("primitive_vert.glsl", "primitive_frag.glsl", "primitive");

	Material* material = Resources::createMaterial(*shader, "main");
	material->setBlendMode(BlendMode::Alpha);

	Material* fontMaterial = Resources::createMaterial(*fontShader, "ttf");
	fontMaterial->setBlendMode(BlendMode::Alpha);

	Material* primitiveMaterial = Resources::createMaterial(*primitiveShader, "primitive");

	fs::path pathToFont = GConfig.getDefaultFontPath();
	loadTTFont(pathToFont);

	rendererInitialized = true;
	return context.get()->renderer.get();
}

// Remember if you start using scaling (there is a scale in renderer) you need to use it here also to get
// proper measurments
vec2 Renderer2D::textSize(const string text, string fontName, const int fontSize, const int spacing)
{
	if (fontName.empty())
		fontName = GConfig.defaultFontName;
	std::string path = file_util::getPath(GConfig.getFontsPath(), fontName + ".ttf").string();
	loadTTFont(path, fontSize);

	vec2 textSize = vec2(0);

	for (string::const_iterator c = text.begin(); c != text.end(); c++)
	{
		TTFGlyph glyph = Resources::fonts[fontName].glyphs[fontSize][*c];
		int advance = glyph.advance;
		textSize.x += (advance >> 6) + spacing;
		textSize.y = std::max((float)glyph.size.y, textSize.y);
	}

	return textSize;
}

bool Renderer2D::loadTTFont(const fs::path path, float fontSize)
{
	string fontName = file_util::getNameFromPath(path);

	if (Resources::fonts.count(fontName) && Resources::fonts[fontName].glyphs.count(fontSize))
	{
		return true;
	}

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return false;
	}

	FT_Face face;
	if (FT_New_Face(ft, path.string().c_str(), 0, &face))
	{
		SDL_Log("ERROR::FREETYPE: Failed to load font (%s - size: %i) from: %s", fontName.c_str(), fontSize, path.c_str());
		return false;
	}
	FT_Set_Pixel_Sizes(face, 0, fontSize);

	if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
	{
		SDL_Log("ERROR::FREETYPE: Failed to load Glyph (%s - size: %i) from: %s", fontName.c_str(), fontSize, path.c_str());
		return false;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

	for (unsigned char c = 0; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}

		Texture2D* glyphTexture = new Texture2D();
		glyphTexture->setChannels(GL_RED);
		glyphTexture->setFiltering(GL_LINEAR);
		glyphTexture->setMipMap(GL_LINEAR);
		glyphTexture->setWrapping(GL_CLAMP_TO_EDGE);
		glyphTexture->create(face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap.buffer);

		TTFGlyph glyph = {
			.texture = glyphTexture,
			.size = ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			.bearing = ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			.advance = face->glyph->advance.x
		};

		Resources::fonts[fontName].glyphs[fontSize][c] = glyph;
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	return true;
}

bool Renderer2D::loadBitmapFont(const string& name)
{
	return true;
}

void Renderer2D::drawLine(const vec2 start, const vec2 end, float lineWidth, const SDL_Color color)
{
	Material* material = Resources::getMaterial("primitive");
	glm::vec4 vColor = colorToVec4(color);

	RenderPayload payload;

	getContext()->renderer->submit(RenderCommand
		{
			.type = RenderType::Line,
			.material = material,
			.color = vColor,
			.z = 200,
			.data = LineData {
					.start = start,
					.end = end,
					.lineWidth = lineWidth
				}
		});
}

void Renderer2D::drawPolygon(const std::vector<glm::vec2> points, float lineWidth, const SDL_Color color)
{
	
}

void Renderer2D::drawRect(const glm::vec2 origin, const glm::vec2 size, float lineWidth, const SDL_Color color)
{
	Material* material = Resources::getMaterial("primitive");
	glm::vec4 vColor = colorToVec4(color);

	getContext()->renderer->submit(RenderCommand
		{
			.type = RenderType::Rect,
			.material = material,
			.color = vColor,
			.z = 200,
			.data = RectData
				{
					.origin = origin,
					.rect = size,
					.lineWidth = lineWidth
				}
		});
}

void Renderer2D::drawTriangle(const vec3 triangle, const SDL_Color color)
{

}

// Function which submits rectangle to the renderer
// vec4 rect - x and y are origin point wz are sizes
void Renderer2D::drawQuad(const vec4 rect, const SDL_Color color, uint8_t z)
{
	Material* material = Resources::getMaterial("main");
	mat4 model = mat4(1.0f);

	// translate
	model = translate(model, vec3(rect.x, rect.y, 0.0f));

	//rotate around center
	model = translate(model, vec3(0.5f * rect.z, 0.5f * rect.w, 0.0f));
	model = rotate(model, radians(0.0f), vec3(0.0f, 0.0f, 1.0f));
	model = translate(model, vec3(-0.5f * rect.z, -0.5f * rect.w, 0.0f));

	// scale
	model = glm::scale(model, vec3(rect.z, rect.w, 1.0f));

	glm::vec4 vColor = colorToVec4(color);

	getContext()->renderer->submit(RenderCommand
	{
		.type = RenderType::Sprite,
		.material = material,
		.color = vColor,
		.z = currentZOrder,
		.scissorViewport = currentScissor,
		.data = SpriteData{
					.model = model
				}
	});
}

void Renderer2D::drawSprite(Transform2D& transform, const Sprite& sprite)
{
	Material* material = Resources::getMaterial("main");

	ecs::Transform2D tempTransform = transform;
	tempTransform.scale.x *= sprite.size.x;
	tempTransform.scale.y *= sprite.size.y;

	mat4 model = mat4(1.0f);

	// translate
	model = translate(model, tempTransform.position);

	//rotate around center
	model = translate(model, vec3(0.5f * tempTransform.scale.x, 0.5f * tempTransform.scale.y, 0.0f));
	model = rotate(model, radians(transform.rotation), vec3(0.0f, 0.0f, 1.0f));
	model = translate(model, vec3(-0.5f * tempTransform.scale.x, -0.5f * tempTransform.scale.y, 0.0f));

	// scale
	model = glm::scale(model, vec3(tempTransform.scale, 1));

	vec4 color = sprite.color;

	Texture2D* texture = Resources::loadTexture(sprite.texture);
	UVRect uvRect = GetUV(sprite.offset.x, sprite.offset.y, sprite.size.x, sprite.size.y, texture->getWidth(), texture->getHeight());

	getContext()->renderer->submit(RenderCommand
		{
			.type = RenderType::Sprite,
			.material = material,
			.color = color,
			.z = static_cast<uint8_t>(glm::clamp(transform.position.z, 0.0f, 255.0f)),
			.data = SpriteData{
					.texture = texture,
					.uvRect = uvRect,
					.model = transform.getMatrix(sprite.size)
				}
		});
}

void Renderer2D::drawTexture(const string handle, const ecs::Transform2D& transform, vec4 uv, string mat)
{
	Material* material = Resources::getMaterial(mat);
	Texture2D* texture = Resources::loadTexture(handle);

	vec2 size = { texture->getWidth(), texture->getHeight() };

	ecs::Transform2D tempTransform = transform;
	tempTransform.scale.x *= size.x;
	tempTransform.scale.y *= size.y;

	mat4 model = mat4(1.0f);

	// translate
	model = translate(model, tempTransform.position);

	//rotate around center
	model = translate(model, vec3(0.5f * tempTransform.scale.x, 0.5f * tempTransform.scale.y, 0.0f));
	model = rotate(model, radians(transform.rotation), vec3(0.0f, 0.0f, 1.0f));
	model = translate(model, vec3(-0.5f * tempTransform.scale.x, -0.5f * tempTransform.scale.y, 0.0f));

	// scale
	model = glm::scale(model, vec3(tempTransform.scale, 1));

	vec4 color = vec4(1.0f);

	UVRect uvRect;
	if (uv != vec4(0.0f))
		uvRect = GetUV(uv.x, uv.y, uv.z, uv.w, size.x, size.y);
	else
		uvRect = GetUV(0, 0, size.x, size.y, size.x, size.y);

	getContext()->renderer->submit(RenderCommand
	{
		.type = RenderType::Sprite,
		.material = material,
		.color = color,
		.z = transform.getLayer().value,
		.data = SpriteData{
				.texture = texture,
				.uvRect = uvRect,
				.model = model
			}
	});
}

void Renderer2D::drawText(string text, const vec2 position)
{
	string fontName = GConfig.defaultFontName;
	fs::path path = file_util::getPath(GConfig.getFontsPath(), fontName + ".ttf");
	loadTTFont(path, 18);

	Material* material = Resources::getMaterial("ttf");

	getContext()->renderer->submit(RenderCommand
	{
		.type = RenderType::Text,
		.material = material,
		.color = vec4(1.0f),
		.z = currentZOrder,
		.scissorViewport = currentScissor,
		.data = TextData {
				.text = text,
				.fontName = fontName,
				.origin = position,
				.size = 18,
				.spacing = 0
			}
	});
}

void Renderer2D::drawText(string text, const vec2 position, string fontName, int fontSize, int spacing, SDL_Color color)
{
	if (fontName.empty())
		fontName = GConfig.defaultFontName;

	fs::path path = file_util::getPath(GConfig.getFontsPath(), fontName + ".ttf");
	loadTTFont(path, fontSize);

	Material* material = Resources::getMaterial("ttf");
	vec4 vColor = colorToVec4(color);

	getContext()->renderer->submit(RenderCommand
	{
		.type = RenderType::Text,
		.material = material,
		.color = vColor,
		.z = currentZOrder,
		.scissorViewport = currentScissor,
		.data = TextData {
				.text = text,
				.fontName = fontName,
				.origin = position,
				.size = fontSize,
				.spacing = spacing
			}
	});
}

void Renderer2D::drawMultilineText(string text, const vec2 position, string fontName, int fontSize, int spacing)
{
	if(fontName.empty())
		fontName = GConfig.defaultFontName;

	fs::path path = file_util::getPath(GConfig.getFontsPath(), fontName + ".ttf");
	loadTTFont(path, fontSize);

	std::vector<string> multilineText = text::split(text, "\n");

	Material* material = Resources::getMaterial("ttf");

	glm::vec2 linePosition = position;
	for (std::string line : multilineText)
	{
		getContext()->renderer->submit(RenderCommand
			{
				.type = RenderType::Text,
				.material = material,
				.color = vec4(1.0f),
				.z = currentZOrder,
				.scissorViewport = currentScissor,
				.data = TextData {
					.text = text,
					.fontName = fontName,
					.origin = position,
					.size = fontSize,
					.spacing = spacing
				}
			});

		float textHeight = textSize(line, fontName, fontSize, spacing).y;
		linePosition.y += textHeight;
	}
}

void Renderer2D::render(Camera* cam)
{
	getContext()->renderer->draw(*cam);
}

void Renderer2D::destroy()
{
	if(rendererInitialized)
		getContext()->renderer->destroy();
}

glm::vec4 Renderer2D::colorToVec4(const SDL_Color& c)
{
	return glm::vec4(
		c.r / 255.0f,
		c.g / 255.0f,
		c.b / 255.0f,
		c.a / 255.0f
	);
}

uint8_t Renderer2D::floatToUint8(float value)
{
	return static_cast<uint8_t>(glm::clamp(value, 0.0f, 255.0f));
}