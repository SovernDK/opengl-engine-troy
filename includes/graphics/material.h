#pragma once
#include "graphics/shader_program.h"
#include "graphics/texture.h"
#include "ecs/components.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class BlendMode
{
    None,
    Alpha,
    Additive
};

class Material
{
public:
    Material(ShaderProgram* shader)
    {
        this->shader = shader;
    }
    
    void setModel(const glm::mat4& model)
    {
        this->model = model;
    }

    void setProjection(const glm::mat4& projection)
    {
        this->projection = projection;
    }

    void setTexture(Texture2D* texture)
    {
        this->texture = texture;
    }

    void setColor(const glm::vec4& color)
    {
        this->color = color;
    }

    void setUvScale(glm::vec2 scale)
    {
        this->uvScale = scale;
    }

    void setUVOffset(glm::vec2 offset)
    {
        this->uvOffset = offset;
    }

    void setBlendMode(BlendMode blend)
    {
        this->blendMode = blend;
    }

    void activate() const
    {
        shader->use();
    }

    void bind() const
    {
        shader->setVec4("vertexColor", color);
        shader->setMat4("projection", projection);
        shader->setMat4("model", model);
        
        if (texture)
        {
            shader->setBool("useTexture", true);
            texture->bind(0);
            shader->setInt("image", 0);

            shader->setVec2("uvOffset", uvOffset);
            shader->setVec2("uvScale", uvScale);
        }
        else
        {
            shader->setBool("useTexture", false);
        }
    }

    BlendMode getBlend() const { return blendMode; }
    ShaderProgram* getShader() const { return shader; }

private:
    ShaderProgram* shader   = nullptr;
    glm::mat4 model         = glm::mat4(1);
    glm::mat4 projection    = glm::mat4(1);
    glm::vec2 uvOffset      = glm::vec2(0);
    glm::vec2 uvScale       = glm::vec2(0);
    glm::vec4 color         = glm::vec4(1.0f);
    Texture2D* texture      = nullptr;
    BlendMode blendMode     = BlendMode::None;
};