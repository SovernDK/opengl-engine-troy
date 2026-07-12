#pragma once
#include "core/game.h"
#include "core/memory/arena.h"

#include "ecs/base_components.h"
#include "utility/file_util.h"
#include "core/resources.h"
#include "graphics/camera/camera_ortho.h"

#include "graphics/rendering/canvas_2d.h"

#include "services/service_locator.h"
#include "services/ui_service.h"
#include "services/file_service.h"
#include "services/input_service.h"
#include "services/log_service.h"
#include "services/audio_service.h"

#include <glad/glad.h>
#include "glm/glm.hpp"

#include <nlohmann/json.hpp>

#include "scenes/world_map_scene.h"
#include "scenes/main_menu_scene.h"
#include "scenes/clans_list_scene.h"
#include "scenes/loading_scene.h"
#include <config.h>

using namespace glm;
using namespace core;

using json = nlohmann::json;
using ECSWorld = ecs::ECSWorld;

bool m_editor = false;

/*
add static and dynamic flags for sprites (change where memory goes in gpu)
*/
void Game::init(AppState& state)
{
#pragma region register services
	ServiceLocator::registerService<ILogger, LoggerService>();
	ServiceLocator::registerService<IInputService, InputService>();
	ServiceLocator::registerService<IFileService, FileService>();
	ServiceLocator::registerService<ISceneService, SceneService>();
	ServiceLocator::registerService<IUIService, UIService>();
	ServiceLocator::registerService<IAudioService, AudioService>();
#pragma endregion

#pragma region load assets
	Resources::loadShader("def_vert.glsl", "def_frag.glsl", "def");
	Resources::loadShader("ttf_vert.glsl", "ttf_frag.glsl", "ttf");
	Resources::loadShader("primitive_vert.glsl", "primitive_frag.glsl", "primitive");
	Resources::loadShader("terrain_vert.glsl", "terrain_frag.glsl", "terrain");
	Resources::loadShader("ui_vert.glsl", "ui_frag.glsl", "ui");
	Resources::loadShader("screen_vert.glsl", "screen_frag.glsl", "screen");
	Resources::loadShader("grad_vert.glsl", "grad_frag.glsl", "grad");

	Material def(Resources::getStrPtrShader("def"));
	Material ttf(Resources::getStrPtrShader("ttf"));
	Material ui(Resources::getStrPtrShader("ui"));
	Material terrain(Resources::getStrPtrShader("terrain"));
	Material primitive(Resources::getStrPtrShader("primitive"));
	Material screen(Resources::getStrPtrShader("screen"));
	Material grad(Resources::getStrPtrShader("grad"));

	Resources::addSharedMat("default", std::make_shared<Material>(def));
	Resources::addSharedMat("ttf", std::make_shared<Material>(ttf));
	Resources::addSharedMat("ui", std::make_shared<Material>(ui));
	Resources::addSharedMat("terrain", std::make_shared<Material>(terrain));
	Resources::addSharedMat("primitive", std::make_shared<Material>(primitive));
	Resources::addSharedMat("screen", std::make_shared<Material>(screen));
	Resources::addSharedMat("grad", std::make_shared<Material>(grad));
#pragma endregion
	world = std::make_unique<ECSWorld>();
	world->create("Settings").add<ecs::MapGenSettings>({});

	mainCam = std::make_unique<OrthoCamera>();
	mainCam->screenRes.x = screenWidth;
	mainCam->screenRes.y = screenHeight;

	mainCam->resize(screenWidth, screenHeight);

	editor = std::make_unique<editor::Editor>(*this);

	Resources::loadTexture(file_util::createPath("assets", "default.png").string(),	 "default");
	Resources::loadTexture(file_util::createPath("assets", "heightmap.png").string(),"heightmap");
	Resources::loadTexture(file_util::createPath("assets", "voronoi.png").string(),	 "voronoi");
	Resources::loadTexture(file_util::createPath("assets", "loading.png").string(),	 "loading");

	Resources::loadTexture(file_util::createPath("assets", "triangle.png").string(), "triangle");

	Resources::loadTexture(file_util::createPath("assets", "water_normal.png").string(),  "water_normal");
	Resources::loadTexture(file_util::createPath("assets", "water_normal2.png").string(), "water_normal2");
	Resources::loadTexture(file_util::createPath("assets", "water_normal3.png").string(), "water_normal3");
	Resources::loadTexture(file_util::createPath("assets", "forestNormal.png").string(),  "forest_normal");

	ServiceLocator::get<IAudioService>()->createAudioDevice();
	Resources::loadClip(file_util::createPath("assets", "sfx", "ui_move.wav").string(), "clip");
	Resources::loadMusic(file_util::createPath("assets", "music", "music.wav").string(), "music");

#pragma region Initialize data
	json settingsData = ServiceLocator::get<IFileService>()->loadJsonFile(file_util::createPath("settings.json").string());
	json uiData		  = ServiceLocator::get<IFileService>()->loadJsonFile(file_util::createPath("style.json").string());

	ServiceLocator::get<IInputService>()->load(settingsData);

	ServiceLocator::get<IUIService>()->loadStyles(uiData);
	ServiceLocator::get<IUIService>()->init(screenWidth, screenHeight);

	ServiceLocator::get<ISceneService>()->registerContext(this);
	ServiceLocator::get<ISceneService>()->registerScene<WorldMapScene>(std::make_shared<WorldMapScene>("WorldMapScene"));
	ServiceLocator::get<ISceneService>()->registerScene<MainMenuScene>(std::make_shared<MainMenuScene>("MainMenuScene"));
	ServiceLocator::get<ISceneService>()->registerScene<ClansListScene>(std::make_shared<ClansListScene>("ClansListScene"));
	ServiceLocator::get<ISceneService>()->registerScene<LoadingScene>(std::make_shared<LoadingScene>("LoadingScene"));

	ServiceLocator::get<ISceneService>()->requestTransition<MainMenuScene>(TransitionMode::Additive);
#pragma region

	//Canvas2D::setInternalResolution(1280, 720);
}

void Game::update(float dt) const
{
	if(m_editor)
		editor->update(dt, *mainCam);

	glm::vec2 mousePos = ServiceLocator::get<IInputService>()->getMousePos();
	glm::vec2 convertedMouse = mainCam->pointToViewport(mousePos);

	ServiceLocator::get<IUIService>()->handleMouse(convertedMouse);
	ServiceLocator::get<IUIService>()->update();

	ServiceLocator::get<ISceneService>()->update(dt);

	ServiceLocator::get<IInputService>()->reset();
}

void Game::input(SDL_Event& e, float dt)
{
	if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_C)
	{
		ServiceLocator::get<IAudioService>()->playOnce("clip");
		ServiceLocator::get<IAudioService>()->stopMusic(5000);
	}

	if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_F1)
	{
		if (m_editor) 
		{
			resize(screenWidth, screenHeight);
			m_editor = false;
		}
		else 
		{
			resize(screenWidth * 0.7f, screenHeight * 0.7f);
			m_editor = true;
		}
	}

	ServiceLocator::get<IInputService>()->processEvents(e);
}

void Game::draw() const
{
	if(m_editor)
		editor->draw();

	glm::vec2 mousePos = ServiceLocator::get<IInputService>()->getMousePos();
	glm::vec2 vpCoords = mainCam->inverse(mousePos);
	glm::vec2 convertedMouse = mainCam->pointToViewport(mousePos);
	
	Canvas2D::drawRect(convertedMouse, glm::vec2(2.0f), GREEN);

	world->view<ecs::Transform2D, ecs::Sprite>([&](ecs::Entity& entity, ecs::Transform2D& t, ecs::Sprite& s)
	{
		Canvas2D::setDepth(1.0f);
		Canvas2D::drawSprite(s, t);
		Canvas2D::clear();
	});

	ServiceLocator::get<IUIService>()->draw();
	ServiceLocator::get<ISceneService>()->draw();
}

void Game::quit() const
{
	// Logger saves session to log, add periodic saves not only save on ending
	ServiceLocator::get<ILogger>()->quit();
	// Destroy remaining scenes and their dependencies (access to service locator here is still necessary so we clear them)
	ServiceLocator::get<ISceneService>()->quit();

	ServiceLocator::quit();

	world->quit();
	Resources::quit();
}

void Game::resize(int viewportWidth, int viewportHeight)
{
	mainCam->resize(viewportWidth, viewportHeight);
}