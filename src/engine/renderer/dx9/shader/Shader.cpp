#include "Shader.h"

#include "../DX9Renderer.h"
#include "../DXCheck.h"

void DX9::Shader::load(const void* vertexData, const void* fragmentData, std::span<std::string> attributes, LPDIRECT3DDEVICE9 device) {
    if (vertexData == nullptr || fragmentData == nullptr)
        return;

    this->loadVertex(device, vertexData);
    this->loadVertexFormat(device, attributes);
    this->loadPixel(device, fragmentData);
}

void DX9::Shader::cleanup() {
    if (this->vertexDeclaration != nullptr) {
        (void)this->vertexDeclaration->Release();
        this->vertexDeclaration = nullptr;
    }

    if (this->vertexShader != nullptr) {
        (void)this->vertexShader->Release();
        this->vertexShader = nullptr;
    }

    if (this->pixelShader != nullptr) {
        (void)this->pixelShader->Release();
        this->pixelShader = nullptr;
    }
}

void DX9::Shader::bind(void* bindData) const {
    if (this->vertexDeclaration == nullptr || this->vertexShader == nullptr || this->pixelShader == nullptr)
        return;

    const auto bind = static_cast<DX9Renderer::ShaderBind*>(bindData);
    const auto device = bind->device;

    DX_CHECK(device->SetVertexDeclaration, this->vertexDeclaration);
    DX_CHECK(device->SetVertexShader, this->vertexShader);
    DX_CHECK(device->SetPixelShader, this->pixelShader);

    DX_CHECK(device->SetVertexShaderConstantF, 0, reinterpret_cast<const float*>(&bind->matrix), 4);
    DX_CHECK(device->SetPixelShaderConstantF, 0, reinterpret_cast<const float*>(&bind->color), 1);
}

void DX9::Shader::loadVertex(LPDIRECT3DDEVICE9 device, const void* data) {
    if (data == nullptr)
        return;

    DX_CHECK(device->CreateVertexShader, static_cast<const DWORD*>(data), &this->vertexShader);
}

void DX9::Shader::loadVertexFormat(LPDIRECT3DDEVICE9 device, std::span<std::string> attributes) {
    std::vector<D3DVERTEXELEMENT9> elements{};

    elements.reserve(attributes.size() + 1);

    WORD offset = 0;

    for (const auto& attr : attributes) {
        D3DVERTEXELEMENT9 element{};

        element.Stream = 0;
        element.Offset = offset;
        element.Method = D3DDECLMETHOD_DEFAULT;
        element.UsageIndex = 0;

        WORD formatSize = 0;

        if (attr == "Position") {
            element.Type = D3DDECLTYPE_FLOAT3;
            element.Usage = D3DDECLUSAGE_POSITION;
            formatSize = sizeof(float) * 3;
        }
        else if (attr == "Color") {
            element.Type = D3DDECLTYPE_FLOAT4;
            element.Usage = D3DDECLUSAGE_COLOR;
            formatSize = sizeof(float) * 4;
        }
        else if (attr == "UV") {
            element.Type = D3DDECLTYPE_FLOAT2;
            element.Usage = D3DDECLUSAGE_TEXCOORD;
            formatSize = sizeof(float) * 2;
        }
        else
            continue;

        elements.push_back(element);

        offset += formatSize;
    }

    elements.push_back(D3DDECL_END());

    DX_CHECK(device->CreateVertexDeclaration, elements.data(), &this->vertexDeclaration);
}

void DX9::Shader::loadPixel(LPDIRECT3DDEVICE9 device, const void* data) {
    if (data == nullptr)
        return;

    DX_CHECK(device->CreatePixelShader, static_cast<const DWORD*>(data), &this->pixelShader);
}
