#pragma once
#include "services/scene.h"

class ClansListScene : public IScene
{
public:
	ClansListScene(std::string id) : IScene(id) {}
	~ClansListScene() override;

	void start(core::IContext* ctx) override;
	void update(core::IContext* ctx, float dt) override;
	void draw(core::IContext* ctx) override;
	void quit(core::IContext* ctx) override;
};