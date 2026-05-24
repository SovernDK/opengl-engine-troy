#pragma once
#include <SDL3/SDL.h>
#include <glad/glad.h>

#include "core/game.h"
#include "core/app_state.h"
#include "editor/imgui_impl.h"

class Game;

namespace core
{
	class Application
	{
	private:
		Game game;
		ImGuiImpl imgui;
	public:
		SDL_AppResult Init(void** appState, int argc, char** argv);
		SDL_AppResult Iterate(void* appState);
		SDL_AppResult Event(void* appState, SDL_Event* event);
		void Quit(void* appState, SDL_AppResult result);

		SDL_AppResult initSDL(AppState& context);
		SDL_AppResult initOpenGl(AppState& context);
		SDL_AppResult startGame(AppState& context);

		void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
			GLsizei length, const char* message, const void* userParam);
	};
}