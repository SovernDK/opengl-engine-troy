#pragma once

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#define SDL_MAIN_USE_CALLBACKS
#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"

#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>

#include <core/application.h>

using namespace core;

Application application;

SDL_AppResult SDL_AppInit(void** appState, int argc, char** argv)
{
    return application.Init(appState, argc, argv);
}

SDL_AppResult SDL_AppIterate(void* appState)
{
    return application.Iterate(appState);
}

SDL_AppResult SDL_AppEvent(void* appState, SDL_Event* event)
{
    return application.Event(appState, event);
}

void SDL_AppQuit(void* appState, SDL_AppResult result)
{
    application.Quit(appState, result);
}