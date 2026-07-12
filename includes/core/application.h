#pragma once
#include <SDL3/SDL.h>
#include <glad/glad.h>

#include "core/game.h"
#include "core/app_state.h"
#include "core/memory/arena.h"

#include "editor/imgui_impl.h"

class Game;

namespace core
{
	class Application
	{
	private:
		Game game;
		ImGuiImpl imgui;
		mem::Arena frameArena;
	public:
		Application() : frameArena(1024 * 1024) {};

		SDL_AppResult Init(void** appState, int argc, char** argv);
		SDL_AppResult Iterate(void* appState);
		SDL_AppResult Event(void* appState, SDL_Event* event);
		void Quit(void* appState, SDL_AppResult result);

		SDL_AppResult initSDL(AppState& context);

		void printCompilerInfo();
	};
}