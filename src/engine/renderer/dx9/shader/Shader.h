#pragma once

namespace DX9 {
    class Shader {
    public:
        void load(const void* vertexData, const void* fragmentData, std::span<std::string> attributes, LPDIRECT3DDEVICE9 device);
        void cleanup();

        void bind(void* bindData) const;

        void loadVertex(LPDIRECT3DDEVICE9 device, const void* data);
        void loadVertexFormat(LPDIRECT3DDEVICE9 device, std::span<std::string> attributes);
        void loadPixel(LPDIRECT3DDEVICE9 device, const void* data);

    private:
        LPDIRECT3DVERTEXSHADER9 vertexShader{};
        LPDIRECT3DVERTEXDECLARATION9 vertexDeclaration{};
        LPDIRECT3DPIXELSHADER9 pixelShader{};
    };
}
