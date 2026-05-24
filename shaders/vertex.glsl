#version 430 core
layout(location = 0)   in vec3 aPos;
layout(location = 1)   in vec3 aColor;
layout(location = 2)   in vec2 aTexCoords;

//out vec4 vertexColor;
out vec2 TexCoords;

uniform vec2 uvOffset;
uniform vec2 uvScale;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main()
{
    gl_Position = projection * model * vec4(aPos, 1.0);
    TexCoords = aTexCoords * uvScale + uvOffset;
    //vertexColor = vec4(aColor, 1.0);
}