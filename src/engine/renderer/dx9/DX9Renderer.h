#pragma once

#include "../Renderer.h"
#include "shader/Shader.h"
#include "tessellation/Mesh.h"
#include "utils/Image.h"

class DX9Renderer : public Renderer {
public:
    struct ShaderBind {
        LPDIRECT3DDEVICE9 device{};
        glm::mat4 matrix{};
        glm::vec4 color{};
    };

    DX9Renderer(HINSTANCE hInstance, Win32Window* window, const std::shared_ptr<RendererContext>& context) : Renderer(hInstance, window, context) {}

    bool init() override;
    void exit() override;
    void render(double deltaTime) override;
    RendererType getRendererType() override;
    uint32_t loadShader(const void* vertexShaderData, size_t vertexShaderSize, const void* fragmentShaderData, size_t fragmentShaderSize, std::span<std::string> attributes) override;
    std::shared_ptr<Mesh> upload(float* vertices, uint32_t vertexCount, uint16_t* indices, uint32_t indexCount, uint8_t vertexSize, const std::bitset<VertexFormats::VertexAttribute::ATTR_COUNT>& enabledAttributes) override;
    uint32_t loadTexture(const void* textureData, size_t textureSize, uint32_t width, uint32_t height) override;
    void draw(std::shared_ptr<Mesh> mesh) override;
    void draw(std::shared_ptr<Mesh> mesh, uint32_t texId) override;

    [[nodiscard]] LPDIRECT3DVERTEXBUFFER9 allocateVertexBuffer(UINT length, DWORD fvf) const;
    [[nodiscard]] LPDIRECT3DINDEXBUFFER9 allocateIndexBuffer(UINT length) const;
    [[nodiscard]] LPDIRECT3DTEXTURE9 allocateTexture(UINT width, UINT height, UINT levels = 1, D3DFORMAT format = D3DFMT_A8R8G8B8) const;

private:
    LPDIRECT3D9 dxcontext{};
    LPDIRECT3DDEVICE9 device{};
    D3DVIEWPORT9 viewport{};
    glm::ivec2 cachedWindowSize{};

    std::vector<std::shared_ptr<DX9::Mesh>> meshes{};
    std::vector<DX9::Image> textures{};
    std::pair<uint16_t, DX9::Image> missingTexture{};
    std::array<uint16_t, 4> defaultShaders{};
    std::vector<DX9::Shader> shaders{};
    uint16_t defaultFontShader{};

    D3DPOOL pool = D3DPOOL_MANAGED;

    void createContext();
    void destroyContext() const;

    void createDevice();
    void destroyDevice() const;

    void destroyMeshes();

    void createTextures();
    void destroyTextures();

    void createShaders();
    void destroyShaders();

    void updateViewport();
    void updateMatrices();
};
