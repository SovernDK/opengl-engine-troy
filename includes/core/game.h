#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_keyboard.h>

#include <flecs.h>
#include <memory>

#include "core/app_state.h"
#include "graphics/camera.h"

namespace core
{
	class Game
	{
	private:
		int screenWidth		= 0,
			screenHeight	= 0;

		int viewportWidth	= 0,
			viewportHeight	= 0;

		flecs::world world;

		std::unique_ptr<Camera>	mainCam;

		const bool* keys = SDL_GetKeyboardState(nullptr);
	public:
		Game() = default;
		Game(int width, int height)
		{
			this->viewportWidth	 = width;
			this->viewportHeight = height;

			this->screenWidth	= width;
			this->screenHeight	= height;
		}

		void init(AppState& state);
		void update(float dt);
		void input(SDL_Event& e, float dt);
		void draw();
		void quit();

		const flecs::world getWorld() { return world; }

		Camera& getMainCam()	{ return *mainCam; }
	};
}