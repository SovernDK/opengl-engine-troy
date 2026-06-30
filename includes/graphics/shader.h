#pragma once
#include <glad/glad.h>
#include <string>

class Shader
{
public:
    Shader() = default;
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    GLuint FromFile(const std::string& path, GLenum type);
    void destroy();

    GLuint ID() const;
private:
    GLuint id;
};