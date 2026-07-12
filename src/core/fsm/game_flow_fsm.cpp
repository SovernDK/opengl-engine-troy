#pragma once
#include "core/fsm/game_flow_fsm.h"

#include "gameplay/states/start_game_state.h"

void GameFlowFSM::init()
{
	states[GameFlowId::IDLE] = std::make_shared<IdleGameState>();
	states[GameFlowId::START_GAME] = std::make_shared<StartGameState>();
	//states[GameFlowId::START_TURN] =
	
	currentState = states[GameFlowId::IDLE];
	currentState.lock()->enter(ctx);
	currentId = GameFlowId::IDLE;
	transitionId = GameFlowId::IDLE;
}

void GameFlowFSM::requestTransition(GameFlowId to) 
{
	transitionId = to;
}

void GameFlowFSM::flush()
{
	if (transitionId != currentId)
	{
		currentState.lock()->exit(ctx);
		currentState = states[transitionId];
		currentId = transitionId;
		currentState.lock()->enter(ctx);
	}
}

void GameFlowFSM::update(float dt) 
{
	if (!currentState.expired())
	{
		currentState.lock()->update(ctx, dt);
	}
}

std::weak_ptr<IState> GameFlowFSM::getCurrent()
{
	return currentState;
}