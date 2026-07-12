#pragma once
#include "scenes/main_menu_scene.h"

#include "SDL3/SDL.h"
#include "core/game.h"
#include "services/ui_service.h"

#include <imgui.h>
#include <scenes/world_map_scene.h>
#include <scenes/loading_scene.h>
#include <services/audio_service.h>
#include <utility/file_util.h>
#include <utility/utils.h>
#include <profiler.h>

MainMenuScene::~MainMenuScene() {}

void MainMenuScene::start(core::IContext* ctx)
{
	core::Game& game = dynamic_cast<core::Game&>(*ctx);
	auto ui = ServiceLocator::get<IUIService>();

	auto& builder = TextureBuilder()
		.setFiltering(GL_NEAREST)
		.setFormat(GL_RGBA)
		.setInternalFormat(GL_RGBA)
		.setWrapping(GL_REPEAT)
		.setWrapAxis(true, true)
		.setBorderColor(0, 0, 0);

	Resources::loadTexture(file_util::createPath("assets", "gradient.png").string(), "gradient", builder);

	ServiceLocator::get<IAudioService>()->playMusic("music");

	auto window = ui->createWindow()
		.setLocPos(UIAnchor::Center)
		.setLocSize(0.15f, 0.3f)
		.setPivot(UIAnchor::Center)
		.setStyle("mm_window")
		.build("main_menu_window");

	ui->createButton()
		.setLocSize(0.1f, 0.3f)
		.setParent(window)
		.setStyle("mm_button")
		.addOnClick([&](UIWidget* widget) {
			ServiceLocator::get<ISceneService>()->requestRemoveLast();
			ServiceLocator::get<ISceneService>()->requestTransition<WorldMapScene>(TransitionMode::Additive);
			ServiceLocator::get<ISceneService>()->requestTransition<LoadingScene>(TransitionMode::Additive);
		})
		.setText("New game")
		.build("new_game_btn");

	ui->createButton()
		.setLocSize(0.1f, 0.3f)
		.setParent(window)
		.setStyle("mm_button")
		.setText("Settings")
		.build("load_game_btn");

	ui->createButton()
		.setLocSize(0.1f, 0.3f)
		.setParent(window)
		.setStyle("mm_button")
		.setText("Exit")
		.build("exit_game_btn");
}

void MainMenuScene::update(core::IContext* ctx, float dt)
{
	core::Game& game = dynamic_cast<core::Game&>(*ctx);
}

void MainMenuScene::draw(core::IContext* ctx)
{
	core::Game& game = dynamic_cast<core::Game&>(*ctx);
	
	glm::vec4 bg = glm::vec4(0, 0, game.screenWidth, game.screenHeight);
	auto* mat = Canvas2D::loadToArena<MaterialInstance>(Resources::sharedMat("grad"));
	
	SDL_Color upperColor1{ 188, 52,  11,  255 };
	SDL_Color upperColor2{ 244, 148, 118, 255 };

	mat->setProperty("horizontal", false);
	mat->setProperty("upper_color1", color::SDLColorToVec3(upperColor1));
	mat->setProperty("upper_color2", color::SDLColorToVec3(upperColor2));

	SDL_Color lowerColor1{ 237, 175, 18,  255 };
	SDL_Color lowerColor2{ 249, 218, 149, 255 };

	mat->setProperty("lower_color1", color::SDLColorToVec3(lowerColor1));
	mat->setProperty("lower_color2", color::SDLColorToVec3(lowerColor2));
	mat->setProperty("time", core::Profiler::instance().getElapsedTime());

	Canvas2D::drawQuad(bg, mat);
}

void MainMenuScene::quit(core::IContext* ctx)
{
	core::Game& game = dynamic_cast<core::Game&>(*ctx);
	auto ui = ServiceLocator::get<IUIService>();

	ui->destroy("main_menu_window");
}