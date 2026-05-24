#pragma once

#include "graphics/shader_program.h"
#include "graphics/shader.h"
#include "graphics/material.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/ttfonts.h"

#include <unordered_map>
#include <memory>
#include <map>
#include <filesystem>

struct Font
{
	std::map<int, std::map<char, TTFGlyph>> glyphs;

	void clear()
	{
		for (auto& [size, map] : glyphs)
		{
			for (auto [character, glyph] : map)
			{
				delete glyph.texture;
			}
		}
	}
};

class Resources
{
public:
	static std::unordered_map<std::string, Font> fonts;

	static ShaderProgram* loadShader(const std::string& vertex, const std::string& fragment, std::string name);
	static ShaderProgram* loadShader(const std::string& name);
	static Texture2D* saveTexture(const Texture2D& texture, std::string name);
	static Texture2D* loadTexture(const std::filesystem::path& path, std::string name);
	static Texture2D* loadTexture(const std::string& name);
	static Material* getMaterial(const std::string& handle);
	static Material* createMaterial(ShaderProgram& shader, std::string handle);
	static void clear();
private:
	Resources() {}

	static std::unordered_map<std::string, std::unique_ptr<ShaderProgram>> shaders;
	static std::unordered_map<std::string, std::unique_ptr<Texture2D>> textures;
	static std::unordered_map<std::string, std::unique_ptr<Material>> materials;
};