#pragma once
#include "core/context.h"
#include "glm/glm.hpp"

namespace editor
{
	class IWorldMapEditor
	{
	protected:
		core::IContext& ctx;
	public:
		IWorldMapEditor(core::IContext& ctx) : ctx(ctx) {};
		~IWorldMapEditor() = default;

		virtual void render() = 0;
	};

	class WorldMapEditor : public IWorldMapEditor
	{
	private:
		int screenWidth = 1920;
		int screenHeight = 1080;

		float frequency = 0.001f;
		int octaves = 8;
		float gain = 0.45f;
		int seed = 1337;

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

		float texW = 1920;
		float texH = 1080;
	public:
		WorldMapEditor(core::IContext& ctx) : IWorldMapEditor(ctx) {};

		void render() override;
	};
}