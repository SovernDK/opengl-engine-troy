#pragma once
#include <SDL3/SDL.h>

class SDL_Window;

class ImGuiImpl
{
private:
	bool contextInitialized = false;
	bool platformInitialized = false;
	bool rendererInitialized = false;
public:
	ImGuiImpl() = default;
	ImGuiImpl(SDL_Window* window, void* sdl_gl_context, const char* glVersion);
	~ImGuiImpl() = default;

	bool init();
	void newFrame();
	void render();
	void processEvents(SDL_Event* event);
	void quit();
};