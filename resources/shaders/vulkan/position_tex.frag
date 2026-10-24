#version 450

layout(location = 0) in vec2 inUV;

layout(location = 0) out vec4 outFragColor;

layout(set = 0, binding = 0) uniform sampler2D displayTexture;

layout(push_constant) uniform constants
{
    layout(offset = 80) vec4 shaderColor;
} PushConstants;

void main()
{
    vec4 texColor = texture(displayTexture, inUV) * PushConstants.shaderColor;

    if (texColor.a == 0.0) {
        discard;
    }

    outFragColor = texColor;
}
