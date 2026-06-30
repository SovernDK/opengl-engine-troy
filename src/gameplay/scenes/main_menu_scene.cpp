#pragma once
#include "scenes/main_menu_scene.h"

#include "SDL3/SDL.h"
#include "core/game.h"
#include "services/ui_service.h"

#include <imgui.h>
#include <scenes/world_map_scene.h>
#include <scenes/loading_scene.h>

MainMenuScene::~MainMenuScene() {}

void MainMenuScene::start(core::IContext* ctx)
{
	core::Game& game = dynamic_cast<core::Game&>(*ctx);
	auto ui = ServiceLocator::get<IUIService>();

	auto window = ui->createWindow()
		.setLocPos(UIAnchor::Center)
		.setLocSize(0.15f, 0.3f)
		.setPivot(UIAnchor::Center)
		.setStyle("defWindow")
		.build("main_menu_window");

	ui->createButton()
		.setLocSize(0.1f, 0.3f)
		.setParent(window)
		.setOnClick([&](UIWidget* widget) { 
			ServiceLocator::get<ISceneService>()->requestRemoveLast();
			ServiceLocator::get<ISceneService>()->requestTransition<WorldMapScene>(TransitionMode::Additive);
			ServiceLocator::get<ISceneService>()->requestTransition<LoadingScene>(TransitionMode::Additive);
		})
		.setText("New game")
		.build("new_game_btn");

	ui->createButton()
		.setLocSize(0.1f, 0.3f)
		.setParent(window)
		.build("load_game_btn");

	ui->createButton()
		.setLocSize(0.1f, 0.3f)
		.setParent(window)
		.build("exit_game_btn");
}

void MainMenuScene::update(core::IContext* ctx, float dt)
{
	core::Game& game = dynamic_cast<core::Game&>(*ctx);
}

void MainMenuScene::draw(core::IContext* ctx)
{

}

void MainMenuScene::quit(core::IContext* ctx)
{
	core::Game& game = dynamic_cast<core::Game&>(*ctx);
	auto ui = ServiceLocator::get<IUIService>();

	ui->destroy("main_menu_window");
}