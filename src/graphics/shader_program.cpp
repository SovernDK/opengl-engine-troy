#pragma once

#include "SDL3/SDL.h"
#include "graphics/shader_program.h"

#include <glm/glm.hpp>
#include <gtc/type_ptr.hpp>

using namespace glm;

void ShaderProgram::attach(const Shader& shader)
{
	glAttachShader(id, shader.ID());
}

void ShaderProgram::link()
{
	glLinkProgram(id);

	int success;
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
	    char log[512];
	    glGetProgramInfoLog(id, 512, nullptr, log);
	    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Program link error: %s", log);
	}
}

void ShaderProgram::use() const
{
	glUseProgram(id);
}

void ShaderProgram::setMat4(const std::string name, const mat4& mat)
{
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void ShaderProgram::setVec4(const std::string name, const vec4& value)
{
	glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void ShaderProgram::setVec3(const std::string name, const vec3& value)
{
	glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void ShaderProgram::setVec2(const std::string name, const vec2& value)
{
	glUniform2f(glGetUniformLocation(id, name.c_str()), value.x, value.y);
}

void ShaderProgram::setInt(const std::string& name, const int value)
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void ShaderProgram::setBool(const std::string& name, const bool value)
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void ShaderProgram::setFloat(const std::string& name, const float value)
{
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void ShaderProgram::destroy()
{
	if (id != 0)
		glDeleteProgram(id);
}

GLuint ShaderProgram::ID() const
{
	return id;
}

ShaderProgram::~ShaderProgram()
{
	if(id != 0)
		glDeleteProgram(id);
}