cbuffer TransformationMatrix : register(c0) {
    matrix transMat;
};

struct Vertex {
    float3 Position : POSITION0;
    float2 UV : TEXCOORD0;
    float4 Color : COLOR0;
};

struct VertexShaderOutput {
    float2 UV : TEXCOORD0;
    float4 Color : COLOR0;
    float4 Position : SV_Position;
};

VertexShaderOutput main(Vertex IN) {
    VertexShaderOutput OUT;

    OUT.Position = mul(transMat, float4(IN.Position, 1.0f));
    OUT.UV = IN.UV;
    OUT.Color = IN.Color;

    return OUT;
}
