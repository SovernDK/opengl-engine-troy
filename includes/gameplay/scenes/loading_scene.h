#pragma once
#include "services/scene.h"

class LoadingScene : public IScene
{
public:
	LoadingScene(std::string id) : IScene(id) {}
	~LoadingScene() override;

	void start(core::IContext* ctx) override;
	void update(core::IContext* ctx, float dt) override;
	void draw(core::IContext* ctx) override;
	void quit(core::IContext* ctx) override;
};