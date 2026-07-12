#pragma once
#include "scenes/main_menu_scene.h"

#include "SDL3/SDL.h"
#include "core/game.h"
#include "services/ui_service.h"

#include "imgui.h"
#include "scenes/loading_scene.h"
#include "graphics/rendering/canvas_2d.h"

LoadingScene::~LoadingScene() {}

float rot = 0;

void LoadingScene::start(core::IContext* ctx)
{
	core::Game& game = dynamic_cast<core::Game&>(*ctx);
	auto ui = ServiceLocator::get<IUIService>();
}

void LoadingScene::update(core::IContext* ctx, float dt)
{
	rot += 360.0f * dt;
	core::Game& game = dynamic_cast<core::Game&>(*ctx);
}

void LoadingScene::draw(core::IContext* ctx)
{
	//Change to sprite
	/*Canvas2D::drawImage("loading", ecs::Transform2D {
			.scale = glm::vec2(.1f),
			.rotation = rot,
		});*/
}

void LoadingScene::quit(core::IContext* ctx)
{
	core::Game& game = dynamic_cast<core::Game&>(*ctx);
	auto ui = ServiceLocator::get<IUIService>();
}