#pragma once
#include <glad/glad.h>
#include "shader.h"
#include <string>
#include <glm/gtc/matrix_transform.hpp>

class ShaderProgram
{
public:
    ShaderProgram() = default;
    ~ShaderProgram();
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    void init()
    {
        id = glCreateProgram();
    }

    void attach(const Shader& shader);
    void link();
    void use() const;

    void setMat4(const std::string name, const glm::mat4& mat);
    void setVec4(const std::string name, const glm::vec4& value);
    void setVec3(const std::string name, const glm::vec3& value);
    void setVec2(const std::string name, const glm::vec2& value);
    void setInt(const std::string& name, const int value);
    void setBool(const std::string& name, const bool value);
    void setFloat(const std::string& name, const float value);

    void destroy();
    
    GLuint ID() const;
private:
    GLuint id;
};