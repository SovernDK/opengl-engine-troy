#pragma once
#include "core/game_flow_fsm.h"

struct StartGameState : IState
{
	StartGameState() = default;
	~StartGameState() = default;

	void enter(core::IContext& ctx) override;
	void update(core::IContext& ctx, float dt) override;
	void exit(core::IContext& ctx) override;

	std::string name() const override { return "StartGameState"; };
};