#version 450

layout(location = 0) in vec4 inColor;

layout(location = 0) out vec4 outFragColor;

layout(push_constant) uniform constants
{
    layout(offset = 80) vec4 shaderColor;
} PushConstants;

void main()
{
    outFragColor = inColor * PushConstants.shaderColor;
}
