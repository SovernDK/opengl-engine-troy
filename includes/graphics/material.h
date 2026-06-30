#pragma once
#include <memory>
#include <unordered_map>
#include <variant>
#include <vector>
#include <string>

#include "graphics/texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class ShaderProgram;
class Texture2D;

using Uniform = std::variant<bool, float, int, glm::vec2, glm::vec3, glm::vec4, glm::mat4>;

enum class BlendMode
{
	None,
	Alpha,
	Additive
};

class Material
{
public:
	std::shared_ptr<ShaderProgram> shader;
public:
	Material(std::shared_ptr<ShaderProgram> shader);
	~Material();
};

//Rewrite to interface and concrete class per material since there will be limited amount of properties to handle and 
//concrete classes will take less space and will be more performant then this
class MaterialInstance
{
public:
	std::shared_ptr<Material> base;
	BlendMode blendMode = BlendMode::None;
private:
	//Change to vectors (less memory, cache friendly)
	//Also remove shared ptr and use copies of Tex2D since its only 12 bytes and better performance then shared pointer
	struct TextureSlot { std::string name; GLuint texId; };
	std::vector<TextureSlot> textures;

	struct PropertySlot { std::string name; Uniform uniform; };
	std::vector<PropertySlot> properties;
public:
	MaterialInstance(std::shared_ptr<Material> shader) : base(shader) {};
	~MaterialInstance();

	//Change to texture objects
	void setTexture(const std::string& name, GLuint id);
	void setTexture(const std::string& name, TexID id);
	void addProperty(const std::string& name, Uniform value);
	void clearProperties();
	void setProperty(const std::string& name, Uniform value);

	void bind() const;
	void unbind() const;
};