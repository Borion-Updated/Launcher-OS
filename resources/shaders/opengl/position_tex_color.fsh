#version 330 core

uniform sampler2D texture_0;
uniform vec4 shaderColor;

in vec2 texCoord;
in vec4 vertexColor;

out vec4 fragColor;

void main()
{
    vec4 color = texture(texture_0, texCoord) * vertexColor * shaderColor;

    if (color.a == 0.0) {
        discard;
    }

    fragColor = color;
}