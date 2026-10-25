cbuffer TransformationMatrix : register(c0) {
    matrix transMat;
};

struct Vertex {
    float3 Position : POSITION0;
};

struct VertexShaderOutput {
    float4 Position : SV_Position;
};

VertexShaderOutput main(Vertex IN) {
    VertexShaderOutput OUT;

    OUT.Position = mul(transMat, float4(IN.Position, 1.0f));

    return OUT;
}
