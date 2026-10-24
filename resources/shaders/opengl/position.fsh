#version 330 core

uniform vec4 shaderColor;

out vec4 fragColor;

void main()
{
    fragColor = shaderColor;
}