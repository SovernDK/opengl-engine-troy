#pragma once
#include "scenes/world_map_scene.h"

#include "SDL3/SDL.h"
#include "core/game.h"

#include "utility/utils.h"
#include "core/resources.h"
#include "graphics/rendering/canvas_2d.h"
#include "graphics/graphics.h"

#include "graphics/material.h"
#include "graphics/texture.h"

#include <imgui.h>
#include <utility/file_util.h>
#include <services/ui_service.h>

#include "core/ecs/clan_components.h"
#include "scenes/clans_list_scene.h"
#include "scenes/main_menu_scene.h"

MapGeneration buildMapData(const std::shared_ptr<const ecs::MapGenSettings> settings);
void applyMapToWorld(core::Game& game, MapGeneration& mapGen, int w, int h);
void updateTerrain(core::Game& game);
void initializeWorld(core::Game& game);

using namespace ecs;

void WorldMapScene::start(core::IContext* ctx)
{
	core::Game& game = dynamic_cast<core::Game&>(*ctx);
	auto ui = ServiceLocator::get<IUIService>();

	/*for (int i = 0; i < 3; i++)
	{
		game.world->create()
			.addComponent<ecs::Clan>(ecs::Clan{
				.name = "Clan" + i,
				.coordinate = glm::vec2(i * 20, i * 30)
				});
	}

	game.world->system<ecs::Clan, ecs::Sprite>([](ecs::Entity& entity, ecs::Clan& t, ecs::Sprite& v)
	{
		SDL_Log("System run");
	});

	game.world->process();
	game.world->print();*/

	ecs::EntityId id = game.world->entity("Settings").id;
	assert(id != 0);
	auto& settings = game.world->get<ecs::MapGenSettings>(id);

	mapFuture = std::async(std::launch::async, buildMapData, settings);
}

void WorldMapScene::update(core::IContext* ctx, float dt)
{
	core::Game& game = dynamic_cast<core::Game&>(*ctx);
	auto ui = ServiceLocator::get<IUIService>();

	if (!mapReady && mapFuture.valid())
	{
		auto status = mapFuture.wait_for(std::chrono::milliseconds(0));
		if (status == std::future_status::ready)
		{
			initializeWorld(game);
			
			MapGeneration result = mapFuture.get();
			applyMapToWorld(game, result, 1920, 1080);
			ServiceLocator::get<ISceneService>()->requestRemoveLast();

			mapReady = true;
		}
	}

	updateTerrain(game);
}

void WorldMapScene::draw(core::IContext* ctx)
{
	
}

void WorldMapScene::quit(core::IContext* ctx)
{
	core::Game& game = dynamic_cast<core::Game&>(*ctx);
	auto ui = ServiceLocator::get<IUIService>();

	EntityId id = game.world->entity("WorldMap").id;

	if (id != 0)
	{
		game.world->destroy(id);
	}

	ui->destroy("topbar");
	mapReady = false;
}

void initializeWorld(core::Game& game)
{
	EntityId id = game.world->entity("Settings").id;
	auto settings = game.world->get<ecs::MapGenSettings>(id);
	auto ui = ServiceLocator::get<IUIService>();

	// =========== Entities =======================
	game.world->create("WorldMap")
		.addComponent<ecs::WorldMap>({})
		.addComponent<ecs::Sprite>({
			.size = glm::vec2(settings->width, settings->height),
			.depth = 1.0f,
			.material{ MaterialInstance(Resources::sharedMat("terrain")) },
			})
		.addComponent<ecs::Transform2D>({});

	MaterialInstance temp = MaterialInstance(Resources::sharedMat("def"));
	temp.blendMode = BlendMode::Alpha;
	temp.setProperty("mainColor", glm::vec4(1.0f));
	temp.setProperty("useTexture", true);

	Texture2D* tex = Resources::texture("triangle");
	temp.setTexture("image", tex->ID());

	game.world->create()
		.addComponent<ecs::Sprite>(ecs::Sprite{
				.size = glm::vec2(24),
				.material = temp
			})
		.addComponent<ecs::Transform2D>(ecs::Transform2D{
				.position = glm::vec3(850, 550, 1.0)
			})
		.addComponent<ecs::Settlement>({});

	game.world->create()
		.addComponent<ecs::Sprite>(ecs::Sprite{
				.size = glm::vec2(24),
				.material = temp
			})
		.addComponent<ecs::Transform2D>(ecs::Transform2D{
				.position = glm::vec3(450, 400, 1.0)
			})
		.addComponent<ecs::Settlement>({});

	// =========== UI =======================
	auto window = ui->createWindow()
		.setLocPos(UIAnchor::Top_Center)
		.setLocSize(0.3f, 0.03f)
		.setPivot(UIAnchor::Top_Center)
		.setStyle("topbar")
		.build("topbar");

	//Used copy since createButton will go out of scope -> auto& btnTemp becomes null
	auto btnTemp = ui->createButton()
		.setLocSize(0.3f, 1.0f)
		.setParent(window)
		.setOnClick([&](UIWidget* widget) {})
		.setKeepAspect(true);

	auto exitGame = [&](UIWidget* widget)
	{
		ServiceLocator::get<ISceneService>()->requestRemoveLast();
		ServiceLocator::get<ISceneService>()->requestTransition<MainMenuScene>(TransitionMode::Additive);
	};

	btnTemp.setOnClick(exitGame)
		.build("clans_btn");
	btnTemp.build("clans_btn2");
	btnTemp.build("clans_btn3");
}

// Async
MapGeneration buildMapData(const std::shared_ptr<const ecs::MapGenSettings> settings)
{
	MapGeneration mapGen{};

	mapGen.mountainsSetting.SetSeed(settings->seed);
	mapGen.mountainsSetting.SetFrequency(settings->frequency);
	mapGen.mountainsSetting.SetFractalOctaves(settings->octaves);
	mapGen.mountainsSetting.SetFractalGain(settings->gain);

	mapGen.moistureSetting.SetSeed(settings->seed);

	mapGen.generate(settings->width, settings->height, settings->seed);

	return mapGen;
}

void applyMapToWorld(core::Game& game, MapGeneration& mapGen, int w, int h)
{
	Texture2D mapTexture = TextureBuilder()
		.setInternalFormat(GL_RGBA8)
		.setFiltering(GL_NEAREST)
		.build(w, h, mapGen.heightPixels.data());

	Texture2D normalTexture = TextureBuilder()
		.setFormat(GL_RGBA)
		.build(w, h, mapGen.normalPixels.data());

	Texture2D moistureTexture = TextureBuilder()
		.setFormat(GL_RGBA)
		.build(w, h, mapGen.moisturePixels.data());

	Texture2D* newTex = Resources::saveTexture(std::move(mapTexture), "map");
	Texture2D* newNormal = Resources::saveTexture(std::move(normalTexture), "n_map");
	Texture2D* newMoisture = Resources::saveTexture(std::move(moistureTexture), "m_map");

	Texture2D* waterNormal = Resources::texture("water_normal3");
	Texture2D* forestNormal = Resources::texture("forest_normal");

	EntityId id = game.world->entity("WorldMap").id;
	assert(id != 0);

	auto& worldMaterial = game.world->getMut<ecs::Sprite>(id)->material;

	worldMaterial.blendMode = BlendMode::None;

	updateTerrain(game);

	worldMaterial.setTexture("image", newTex->ID());
	worldMaterial.setTexture("normal", newNormal->ID());
	worldMaterial.setTexture("waterNormal", waterNormal->ID());
	worldMaterial.setTexture("moistureMap", newMoisture->ID());
	worldMaterial.setTexture("forestNormal", forestNormal->ID());
}

void updateTerrain(core::Game& game)
{
	EntityId settingsId = game.world->entity("Settings").id;
	EntityId worldMapId = game.world->entity("WorldMap").id;

	if (settingsId == 0 || worldMapId == 0)
		return;

	auto settings = game.world->get<ecs::MapGenSettings>(settingsId);
	game.world->getMut<ecs::Sprite>(worldMapId)->size = glm::vec2(settings->width, settings->height);
	auto& worldMaterial = game.world->getMut<ecs::Sprite>(worldMapId)->material;

	float aspectRatio = settings->width / settings->height;
	worldMaterial.setProperty("aspectRatio", aspectRatio);
	worldMaterial.setProperty("sunPos", glm::vec3(settings->sunX, settings->sunY, settings->sunZ));
	worldMaterial.setProperty("ambientColor", settings->ambientColor);
	worldMaterial.setProperty("ambientStrength", settings->ambientStrength);
	worldMaterial.setProperty("specularStrength", settings->specularStrength);
	worldMaterial.setProperty("numSteps", settings->steps);
	worldMaterial.setProperty("stepSize", settings->stepSize);
	worldMaterial.setProperty("shadowLength", settings->shadowLength);
	worldMaterial.setProperty("terrainSpecStr", settings->terrainSpecStr);
	worldMaterial.setProperty("terrainSpecSpred", settings->terrainSpecSpred);
	worldMaterial.setProperty("terrainNormStr", settings->terrainNormStr);
	worldMaterial.setProperty("waterColor", settings->waterColor);
	worldMaterial.setProperty("diffuseAmbient", settings->diffuseAmbient);
	worldMaterial.setProperty("waterSpecStr", settings->waterSpecStr);
	worldMaterial.setProperty("waterNormalStr", settings->waterNormalStr);
	worldMaterial.setProperty("waterLevel", settings->waterLevel);
}