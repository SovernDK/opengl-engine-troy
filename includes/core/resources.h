#pragma once

#include "graphics/shader_program.h"
#include "graphics/shader.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/graphics.h"

#include <unordered_map>
#include <memory>
#include <map>
#include <filesystem>
#include <array>
#include <graphics/material.h>

#include <ft2build.h>
#include FT_FREETYPE_H

struct Glyph
{
	gpu::UVRect uvs{ 0 };
	glm::vec2 size{ 0 };
	glm::vec2 bearing{ 0 };
	int advance = 0;

	Glyph() = default;
	Glyph(glm::vec2 size, glm::vec2 bearing, int advance)
	{
		this->size = size;
		this->bearing = bearing;
		this->advance = advance;
	}
	Glyph(Glyph&&) = default;
	Glyph& operator=(Glyph&&) = default;
	Glyph(const Glyph&) = default;
	Glyph& operator=(const Glyph&) = default;
};

struct Font
{
	TexID atlasId = 0;
	std::array<Glyph, 128> _glyphs;
};

class Resources
{
public:
	static std::unordered_map<std::string, Font> fonts;

	static std::weak_ptr<ShaderProgram> loadShader(const std::string& vertex, const std::string& fragment, std::string name);
	static std::weak_ptr<ShaderProgram> loadShader(const std::string& name);
	static std::shared_ptr<ShaderProgram> getStrPtrShader(const std::string& name);

	static Texture2D* saveTexture(Texture2D&& texture, std::string name);
	static Texture2D* loadTexture(const std::filesystem::path& path, std::string name);

	static Texture2D* texture(const std::string& name);
	static Texture2D* texture(TexID id);

	static bool loadTTFont(const std::filesystem::path path, float fontSize);
	//static std::vec2 textSize(const string text, string fontName, const int fontSize, const int spacing);
	static Font* font(const std::string& fontName);

	static void addSharedMat(const std::string& handle, std::shared_ptr<Material> material);
	static std::shared_ptr<Material> sharedMat(const std::string& handle);
	static Material copySharedMat(const std::string& handle);

	static void quit();
private:
	Resources() {}

	static std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> shaders;
	static std::unordered_map<std::string, std::unique_ptr<Texture2D>> textures;
	static std::unordered_map<std::string, std::shared_ptr<Material>> sharedMaterials;
};