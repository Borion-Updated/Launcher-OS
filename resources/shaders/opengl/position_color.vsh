#version 330 core

in vec3 Position;
in vec4 Color;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

out vec4 vertexColor;

void main()
{
    gl_Position = projectionMatrix * modelMatrix * vec4(Position, 1.0);
    vertexColor = Color;
}