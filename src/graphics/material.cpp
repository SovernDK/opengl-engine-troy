#pragma once
#include "graphics/material.h"

#include "graphics/shader_program.h"

Material::Material(std::shared_ptr<ShaderProgram> shader)
{
    this->shader = shader;
}

Material::~Material()
{
    shader.reset();
}

// === Material Instance === //
MaterialInstance::~MaterialInstance()
{
    base.reset();
    textures.clear();
    properties.clear();
}

void MaterialInstance::setTexture(const std::string& name, GLuint id)
{
    for (auto& tex : textures)
    {
        if (tex.name == name)
            return;
    }

    textures.push_back({ name, id });
}

void MaterialInstance::setTexture(const std::string& name, TexID id)
{
	for (auto& tex : textures)
	{
		if (tex.name == name)
			return;
	}

	textures.push_back({ name, id.id });
}

void MaterialInstance::addProperty(const std::string& name, Uniform value)
{
	properties.push_back({ name, value });
}

void MaterialInstance::setProperty(const std::string& name, Uniform value)
{
    auto it = std::find_if(properties.begin(), properties.end(), [name](const auto& property)
    {
        return property.name == name;
    });

    if (it != properties.end())
    {
        it->uniform = value;
    }
    else addProperty(name, value);
}

void MaterialInstance::clearProperties()
{
    properties.clear();
}

void MaterialInstance::bind() const
{
    base->shader->use();

    for (auto& property : properties)
    {
		std::visit([&](auto&& value) { base->shader->setUniform(property.name, value); }, property.uniform);
    }

    int slot = 0;
    for (auto& texture : textures)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, texture.texId);

        base->shader->setUniform(texture.name, (int)slot);
        slot++;
    }
}

void MaterialInstance::unbind() const
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}