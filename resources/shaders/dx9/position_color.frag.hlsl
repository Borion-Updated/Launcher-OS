struct ShaderInput {
    float4 Color : COLOR0;
};

cbuffer ShaderColor : register(c0) {
    float4 shaderCol;
};

float4 main(ShaderInput IN) : SV_Target {
    return IN.Color * shaderCol;
}
