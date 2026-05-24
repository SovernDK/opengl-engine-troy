#version 430 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D image;
uniform vec4 vertexColor;

void main()
{    
    //TTF texture data is stored only in red
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(image, TexCoords).r);
    FragColor = vec4(vertexColor.rgb, vertexColor.a) * sampled;
}  