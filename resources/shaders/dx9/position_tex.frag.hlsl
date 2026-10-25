struct ShaderInput {
    float2 UV : TEXCOORD0;
};

sampler2D Sampler : register(s0);

cbuffer ShaderColor : register(c0) {
    float4 shaderCol;
};

float4 main(ShaderInput IN) : SV_Target {
    float4 col = tex2D(Sampler, IN.UV) * shaderCol;

    if (col.w == 0.0f) {
        discard;
    }

    return col;
}
