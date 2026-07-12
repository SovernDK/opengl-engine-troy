#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "graphics/material.h"
#include "resources.h"

#include <string>

// Cannot use smart pointers in components because they are not trivially copyable and will break the ECS system
namespace ecs
{
    struct Layer
    {
        uint8_t value;
    };

    struct Transform
    {
        glm::vec3 position  = glm::vec3(0);
        glm::vec3 rotation  = glm::vec3(0);
        glm::vec3 scale     = glm::vec3(1);
    };

    struct Transform2D
    {
        glm::vec3 position  = glm::vec3(0);
        glm::vec2 scale     = glm::vec2(1);
        float rotation      = 0.0f;

        glm::mat4 model(glm::vec2 size) const
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
            model = glm::scale(model, glm::vec3(scaledSizeX, scaledSizeY, 1));

            return model;
        }

        glm::mat4 model(float w, float h) const
        {
            return model(glm::vec2(w, h));
        }

        Layer getLayer() const
        {
            return Layer{ static_cast<uint8_t>(glm::clamp(position.z, 0.0f, 255.0f)) };
        }
    };

    struct Sprite
    {
        TexID textureId{ 0 };

		gpu::UVRect uv;
		glm::vec2 size = glm::vec2(32);
        glm::vec4 color = glm::vec4(1.0f);
        //To int
        float depth = 1.0f;

		MaterialInstance material = MaterialInstance(Resources::sharedMat("def"));
	};

    struct MapGenSettings
    {
		float frequency = 0.001f;
		int octaves = 8;
		float gain = 0.5f;
		int seed = 1337;

        int width = 1920;
		int height = 1080;

		float sunX = 1.1f;
		float sunY = 1.1f;
		float sunZ = 2.0f;

		glm::vec3 ambientColor = glm::vec3(1.0f, 0.9f, 0.9f);
		float ambientStrength = 0.1f;
		float diffuseAmbient = 0.25f;
		float specularStrength = 0.3f;

		int steps = 64;
		float stepSize = 0.001f;
		float shadowLength = 0.015f;
		float shadowStr = 0.7f;
		float waterShadowStr = 0.2f;

		// Terrain
		float terrainSpecStr = 0.2f;
		float terrainSpecSpred = 32.0f;
		float terrainNormStr = 0.2f;

		// Forest
		float forestBumpStr = 0.5f;
		float forestNormStr = 0.3f;

		// Water
		glm::vec3 waterColor = glm::vec3(0.38f, 0.65f, 0.66f);
		glm::vec3 deepWaterColor = glm::vec3(0.05f, 0.18f, 0.40f);

		float waterSpecStr = 0.45f;
		float waterNormalStr = 0.5f;
		float waterLevel = 0.36f;
		float waterSpecSpread = 64.0f;
    };
}