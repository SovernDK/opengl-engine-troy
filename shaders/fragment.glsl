#version 430 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D image;
uniform vec4 vertexColor;
uniform bool useTexture;

void main()
{
    vec4 texColor = vec4(1.0);

    if (useTexture)
        texColor = texture(image, TexCoords);

    FragColor = vec4(vertexColor.rgb, vertexColor.a) * texColor;
}