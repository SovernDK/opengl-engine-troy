#pragma once
#include "core/fsm/finite_state_machine.h"
#include "services/service_locator.h"
#include "services/log_service.h"

#include <string>

enum class GameFlowId
{
	IDLE, START_GAME, START_TURN
};

class GameFlowFSM : public IFiniteStateMachine<GameFlowId>
{
public:
	GameFlowFSM(core::IContext& ctx) : IFiniteStateMachine(ctx) {};
	~GameFlowFSM() = default;

	void init() override;
	void requestTransition(GameFlowId to) override;
	void flush() override;
	void update(float dt) override;
	std::weak_ptr<IState> getCurrent() override;
};

struct IdleGameState : IState
{
	IdleGameState() = default;
	~IdleGameState() = default;

	void enter(core::IContext& ctx) override {};
	void update(core::IContext& ctx, float dt) override {};
	void exit(core::IContext& ctx) override {};

	std::string name() const override { return "IdleGameState"; };
};