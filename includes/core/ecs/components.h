#pragma once
#include <flecs.h>

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include <string>

namespace ecs
{

    struct Layer
    {
        uint8_t value;
    };

    struct Transform
    {
        glm::vec3 position = glm::vec3(0);
        glm::vec3 rotation = glm::vec3(0);
        glm::vec3 scale = glm::vec3(1);
    };

    struct Transform2D
    {
        glm::vec3 position = glm::vec3(0);
        glm::vec2 scale = glm::vec2(1);
        float rotation = 0.0f;
        uint8_t z = 0;
        uint8_t layer = 0;

        glm::mat4 getMatrix(glm::vec2 size)
        {
            float scaledSizeX = scale.x * size.x;
            float scaledSizeY = scale.y * size.y;

            glm::mat4 model = glm::mat4(1.0f);
            // translate
            model = glm::translate(model, position);

            //rotate around center
            model = glm::translate(model, glm::vec3(0.5f * scaledSizeX, 0.5f * scaledSizeY, 0.0f));
            model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::translate(model, glm::vec3(-0.5f * scaledSizeX, -0.5f * scaledSizeY, 0.0f));

            // scale
            model = glm::scale(model, glm::vec3(scale, 1));

            return model;
        }

        Layer getLayer() const
        {
            return Layer{ static_cast<uint8_t>(glm::clamp(position.z, 0.0f, 255.0f)) };
        }
    };

    struct Sprite
    {
        std::string texture = "main";
        //replace offset with index (if spritesheet else index is ignored and entire texture is used)
        glm::vec2 offset = glm::vec2(0);
        glm::vec2 size = glm::vec2(10);
        glm::vec4 color = glm::vec4(1.0f);
    };

    struct Velocity
    {
        glm::vec2 value;
    };
}