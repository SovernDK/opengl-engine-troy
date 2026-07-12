#pragma once
#include "scenes/clans_list_scene.h"

#include "SDL3/SDL.h"
#include "core/game.h"
#include "services/ui_service.h"
#include "ecs/clan_components.h"

ClansListScene::~ClansListScene() {}

void ClansListScene::start(core::IContext* ctx)
{
	core::Game& game = dynamic_cast<core::Game&>(*ctx);
	auto ui = ServiceLocator::get<IUIService>();

	/*auto window = ui->createWindow()
		.setLocPos(UIAnchor::Top_Left)
		.setLocSize(0.2f, 1.0f)
		.setPivot(UIAnchor::Top_Left)
		.setStyle("defWindow")
		.build("clans_list");

	int index = 0;
	game.world->view<ecs::Clan>([&](ecs::Entity& entity, ecs::Clan& clan)
	{
		index++;
		ui->createButton()
			.setLocSize(0.1f, 0.3f)
			.setParent(window)
			.addOnClick([&](UIWidget* widget) {})
			.build("clan"+ index);
	});*/
}

void ClansListScene::update(core::IContext* ctx, float dt)
{
	core::Game& game = dynamic_cast<core::Game&>(*ctx);
}

void ClansListScene::draw(core::IContext* ctx)
{

}

void ClansListScene::quit(core::IContext* ctx)
{
	core::Game& game = dynamic_cast<core::Game&>(*ctx);
	auto ui = ServiceLocator::get<IUIService>();

}