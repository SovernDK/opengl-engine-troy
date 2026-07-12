#pragma once
#include <unordered_map>
#include <memory>
#include <string>

#include "core/context.h"

struct IState
{
	virtual ~IState() = default;

	virtual void enter(core::IContext& ctx) = 0;
	virtual void update(core::IContext& ctx, float dt) = 0;
	virtual void exit(core::IContext& ctx) = 0;

	virtual std::string name() const = 0;
};

template<typename StateId>
class IFiniteStateMachine
{
protected:
	std::unordered_map<StateId, std::shared_ptr<IState>> states;
	std::weak_ptr<IState> currentState;

	core::IContext& ctx;
	StateId currentId;
	StateId transitionId;
public:
	IFiniteStateMachine(core::IContext& ctx) : ctx(ctx) {};
	IFiniteStateMachine(IFiniteStateMachine&) = delete;
	IFiniteStateMachine& operator=(IFiniteStateMachine&) = delete;
	virtual ~IFiniteStateMachine() = default;

	virtual void init() = 0;
	virtual void requestTransition(StateId to) = 0;
	virtual void flush() = 0;
	virtual void update(float dt) = 0;
	virtual std::weak_ptr<IState> getCurrent() = 0;

	virtual void quit() 
	{ 
		currentState.reset();
		states.clear(); 
	}
};