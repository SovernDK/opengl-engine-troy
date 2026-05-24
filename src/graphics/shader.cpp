#pragma once
#include "SDL3/SDL.h"
#include <glad/glad.h>

#include "graphics/shader.h"
#include "utility/file_util.h"

using namespace std;

GLuint Shader::FromFile(const string& path, GLenum type)
{
	string source = file_util::load(path);
	const char* src = source.c_str(); //glShaderSource expects pointer of a pointer thats way this is here

	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		char log[512];
		glGetShaderInfoLog(shader, 512, nullptr, log);
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Shader compile error: %s", log);
		glDeleteShader(shader);
	}

	id = shader;
	return shader;
}

void Shader::destroy()
{
	if (id != 0)
		glDeleteShader(id);
}

GLuint Shader::ID() const
{
	return id;
}

Shader::~Shader()
{
	if(id != 0 ) 
		glDeleteShader(id);
}