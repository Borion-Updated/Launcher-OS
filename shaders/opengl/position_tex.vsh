#version 330 core

in vec3 Position;
in vec2 UV;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

out vec2 texCoord;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(Position, 1.0);
    texCoord = UV;
}