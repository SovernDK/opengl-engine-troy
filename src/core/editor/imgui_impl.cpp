#pragma once
#include "editor/imgui_impl.h"

#include <SDL3/SDL.h>

#include <deque>
#include <memory>

#include "lib/imgui/imgui.h"
#include "lib/imgui/imgui_impl_sdl3.h"
#include "lib/imgui/imgui_impl_opengl3.h"

ImGuiImpl::ImGuiImpl(SDL_Window* window, void* sdl_gl_context, const char* glVersion)
{
	float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGuiContext* ctx = ImGui::CreateContext();

	contextInitialized = ctx != NULL;

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup scaling
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
	style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)

	// Setup Platform/Renderer backends
	platformInitialized = ImGui_ImplSDL3_InitForOpenGL(window, sdl_gl_context);
	rendererInitialized = ImGui_ImplOpenGL3_Init(glVersion);
}

bool ImGuiImpl::init()
{
	if (!contextInitialized || !platformInitialized || !rendererInitialized)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to initialize ImGui!");
		return false;
	}
	return true;
}

void ImGuiImpl::newFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
}

void ImGuiImpl::render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiImpl::processEvents(SDL_Event* event)
{
	ImGui_ImplSDL3_ProcessEvent(event);
}

void ImGuiImpl::quit()
{
	if (rendererInitialized) ImGui_ImplSDL3_Shutdown();
	if (platformInitialized) ImGui_ImplOpenGL3_Shutdown();
	if (contextInitialized)	 ImGui::DestroyContext();
}