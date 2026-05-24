#pragma once
#include "utility/file_util.h"
#include "core/resources.h"
#include "core/config.h"

using namespace std;
using namespace core;

unordered_map<string, Font>                      Resources::fonts;
unordered_map<string, unique_ptr<ShaderProgram>> Resources::shaders;
unordered_map<string, unique_ptr<Texture2D>>     Resources::textures;
unordered_map<string, unique_ptr<Material>>      Resources::materials;

ShaderProgram* Resources::loadShader(const string& vertex, const string& fragment, string name)
{
    if (shaders.count(name))
    {
        return shaders[name].get();
    }

    unique_ptr<ShaderProgram> shaderProgram = make_unique<ShaderProgram>();
    Shader fShader, vShader;

    const std::string vertexPath = file_util::getPath(GConfig.getShadersPath(), vertex).string();
    const std::string fragmentPath = file_util::getPath(GConfig.getShadersPath(), fragment).string();

    fShader.FromFile(fragmentPath, GL_FRAGMENT_SHADER);
    vShader.FromFile(vertexPath, GL_VERTEX_SHADER);

    shaderProgram->init();
    shaderProgram->attach(fShader);
    shaderProgram->attach(vShader);
    shaderProgram->link();

    fShader.destroy();
    vShader.destroy();

    shaders[name] = move(shaderProgram);
    return shaders[name].get();
}

ShaderProgram* Resources::loadShader(const string& name)
{
    if (shaders.count(name))
    {
        return shaders[name].get();
    }

    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Couldn't find shader with handle %s", name.c_str());
    return nullptr;
}

Texture2D* Resources::saveTexture(const Texture2D& texture, std::string name)
{
    unique_ptr<Texture2D> tex = make_unique<Texture2D>(texture);
    textures[name] = move(tex);
    return textures[name].get();
}

Texture2D* Resources::loadTexture(const fs::path& path, string name)
{
    if (textures.count(name))
    {
        return textures[name].get();
    }

    unique_ptr<Texture2D> texture = make_unique<Texture2D>();

    texture->load(path.string());
    texture->setFiltering(GL_NEAREST);
    texture->setWrapping(GL_MIRRORED_REPEAT);
    texture->setWrapAxis(true, true);
    texture->setBorderColor(0, 0, 0);

    textures[name] = move(texture);
    return textures[name].get();
}

Texture2D* Resources::loadTexture(const string& name)
{
    if (textures.count(name))
    {
        return textures[name].get();
    }

    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Couldn't find texture with handle %s", name.c_str());
    return nullptr;
}

Material* Resources::createMaterial(ShaderProgram& shader, string handle)
{
    materials[handle] = make_unique<Material>(&shader);
    return materials[handle].get();
}

Material* Resources::getMaterial(const string& handle)
{
    if (materials.count(handle))
    {
        return materials[handle].get();
    }

    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Couldn't find material with handle %s", handle.c_str());
    return nullptr;
}

void Resources::clear()
{
    for (auto& [name, font] : fonts)
    {
        font.clear();
    }
}