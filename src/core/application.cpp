#pragma once
#include "core/application.h"

#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_opengl3.h>

#include <SDL3_image/SDL_image.h>

#include <iostream>
#include <glad/glad.h>

#include "core/app_state.h"

#include "utility/file_util.h"
#include "core/config.h"

#include "core/profiler.h"

using namespace core;

SDL_AppResult Application::Init(void** appState, int argc, char** argv)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    AppState* context = new AppState();

    bool configLoaded = GConfig.load(file_util::getPath("config.json"));
    if (!configLoaded) {
        SDL_Log("Unable to load config! Closing the app.");
        return SDL_APP_FAILURE;
    }

    initSDL(*context);
    initOpenGl(*context);

	imgui = ImGuiImpl(context->window, context->glContext, "#version 430");
    if(!imgui.init()) {
        SDL_Log("Failed to initialize ImGui! Closing the app.");
        return SDL_APP_FAILURE;
    }

    startGame(*context);
    *appState = context;

    return SDL_APP_CONTINUE;
}

SDL_AppResult Application::Iterate(void* appState)
{
    AppState* context = (AppState*)appState;

    imgui.newFrame();

    Profiler::Instance().update();

    game.update(Profiler::Instance().getDeltaTime());
    game.draw();

	imgui.render();

    SDL_GL_SwapWindow(context->window);

    return SDL_APP_CONTINUE;
}

SDL_AppResult Application::Event(void* appState, SDL_Event* event)
{
    AppState* context = (AppState*)appState;

	imgui.processEvents(event);

    if (event->type == SDL_EVENT_QUIT)                                      { return SDL_APP_SUCCESS; }
    if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_ESCAPE) { return SDL_APP_SUCCESS; }

    game.input(*event, Profiler::Instance().getDeltaTime());

    return SDL_APP_CONTINUE;
}

void Application::Quit(void* appState, SDL_AppResult result)
{
    AppState* context = (AppState*)appState;

	imgui.quit();
    game.quit();

    if (context)
    {
        if (context->glContext)
        {
            SDL_GL_DestroyContext(context->glContext);
            context->glContext = nullptr;
            SDL_Log("OpenGL context destroyed");
        }
        if (context->window)
        {
            SDL_DestroyWindow(context->window);
            context->window = nullptr;
            SDL_Log("Window destroyed");
        }

        delete context;
        SDL_Log("App context freed");
    }

    SDL_Quit();
}

SDL_AppResult Application::initSDL(AppState& context)
{
    SDL_SetHint(SDL_HINT_IME_IMPLEMENTED_UI, "composition");
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Submit click events when focusing the window.
    SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
    // Touch events are handled natively, no need to generate synthetic mouse events for touch devices.
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");
    //openGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

    const float window_size_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, context.name.c_str());
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_CENTERED);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_CENTERED);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, int(context.width * window_size_scale));
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, int(context.height * window_size_scale));
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN, true);
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, false);
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN, true);
    context.window = SDL_CreateWindowWithProperties(props);
    SDL_DestroyProperties(props);

    if (!context.window) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Couldn't create window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult Application::initOpenGl(AppState& context)
{
    //openGL context
    context.glContext = SDL_GL_CreateContext(context.window);
    if (!context.glContext)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "GL context creation failed");
        return SDL_APP_FAILURE;
    }

    SDL_GL_MakeCurrent(context.window, context.glContext);
    SDL_GL_SetSwapInterval(1);

    //glad initialization
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to init GLAD");
        return SDL_APP_FAILURE;
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    //openGL Debug Output
    int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        //glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        SDL_Log("OpenGL Debug output initialized!");
    }
    else
    {
        SDL_Log("Error trying to initialize Debug Output");
        return SDL_APP_FAILURE;
    }

    SDL_Log("OpenGL version: %s", glGetString(GL_VERSION));

    return SDL_APP_CONTINUE;
}

SDL_AppResult Application::startGame(AppState& context)
{
    game = Game(context.viewportWidth, context.viewportHeight);
    game.init(context);

    return SDL_APP_CONTINUE;
}

// OpenGL Debug Output
void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam)
{
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
    case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
    case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
    case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}