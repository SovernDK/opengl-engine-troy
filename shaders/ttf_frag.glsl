#version 430 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D image;
uniform vec4 mainColor;

void main()
{    
    //TTF texture data is stored only in red
    vec4 texColor = vec4(vec3(1.0), texture(image, TexCoords).r);
    FragColor = texColor * mainColor;
}