#pragma once
#include <glad/glad.h>
#include "core/app_state.h"

class Arena;

class OpenGLImpl
{
public:
	static bool init(core::AppState& context, Arena* frameArena);
	static void quit(core::AppState& context);
private:
	static bool openGl(core::AppState& context);
	static bool initRenderer(core::AppState& context, Arena* frameArena);

	static void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam);
};
