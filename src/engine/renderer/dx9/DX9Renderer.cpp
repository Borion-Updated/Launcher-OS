#include "DX9Renderer.h"

#include "../../../utils/Utils.h"
#include "../../window/Win32Window.h"
#include "../api/Tessellator.h"
#include "DXCheck.h"
#include "utils/Utils.h"

bool DX9Renderer::init() {
    WF::DX9::init();

    this->tessellator = std::make_shared<Tessellator>(this);

    this->cachedWindowSize = this->window->getWindowSize();

    this->createContext();

    if (this->dxcontext == nullptr)
        return false;

    this->createDevice();

    if (this->device == nullptr)
        return false;

    this->createTextures();
    this->createShaders();
    this->updateViewport();
    this->updateMatrices();

    this->context->init();

    return true;
}

void DX9Renderer::exit() {
    if (this->context != nullptr) {
        if (this->device != nullptr) {
            this->context->exit();

            this->destroyShaders();
            this->destroyTextures();
            this->destroyMeshes();
            this->destroyDevice();
        }

        this->destroyContext();
    }

    WF::DX9::exit();
}

void DX9Renderer::render(const double deltaTime) {
    if (this->device == nullptr)
        return;

    if (const auto size = this->window->getWindowSize(); this->cachedWindowSize != size) {
        this->cachedWindowSize = size;

        this->updateViewport();
        this->updateMatrices();
    }

    this->shaderColor = glm::vec4{1.f};

    DX_CHECK(this->device->Clear, 0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_RGBA(0, 0, 0, 0), 1.f, 0);
    DX_CHECK(this->device->Clear, 0, nullptr, D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.f, 0);

    DX_CHECK(this->device->BeginScene);

    auto& defMat = this->stack.push();

    defMat = translate(defMat, {0.f, 0.f, -250.f});

    this->context->render(deltaTime);

    this->stack.pop();

    this->stack.reset();

    DX_CHECK(this->device->EndScene);

    DX_CHECK(this->device->Present, nullptr, nullptr, nullptr, nullptr);
}

RendererType DX9Renderer::getRendererType() {
    return RendererType::DirectX9;
}

void DX9Renderer::draw(std::shared_ptr<Mesh> mesh) {
    if (mesh->getShaderId() < this->shaders.size()) {
        ShaderBind bind{.device = this->device, .matrix = this->projectionMatrix * this->viewMatrix * this->stack.top(), .color = this->shaderColor};

        this->shaders[mesh->getShaderId()].bind(&bind);
    }

    mesh->draw(this->device);
}

void DX9Renderer::draw(std::shared_ptr<Mesh> mesh, const uint32_t texture) {
    const auto& tex = texture >= this->textures.size() ? this->missingTexture.second : this->textures[texture];

    DX_CHECK(this->device->SetTexture, 0, tex.texture);

    this->draw(mesh);

    DX_CHECK(this->device->SetTexture, 0, nullptr);
}

uint32_t DX9Renderer::loadShader(const void* vertexShaderData, size_t vertexShaderSize, const void* fragmentShaderData, size_t fragmentShaderSize, std::span<std::string> attributes) {
    DX9::Shader shader{};

    shader.load(vertexShaderData, fragmentShaderData, attributes, this->device);

    const auto id = this->shaders.size();

    this->shaders.emplace_back(shader);

    return id;
}

std::shared_ptr<Mesh> DX9Renderer::upload(float* vertices, const uint32_t vertexCount, uint16_t* indices, const uint32_t indexCount, const uint8_t vertexSize, const std::bitset<VertexFormats::VertexAttribute::ATTR_COUNT>& enabledAttributes) {
    auto mesh = std::make_shared<DX9::Mesh>();

    mesh->load(vertices, vertexCount, indices, indexCount, vertexSize, enabledAttributes, this);

    uint8_t defaultShaderIndex = 0;

    if (enabledAttributes[VertexFormats::VertexAttribute::ATTR_COLOR])
        defaultShaderIndex |= 0b1;

    if (enabledAttributes[VertexFormats::VertexAttribute::ATTR_TEXCOORD])
        defaultShaderIndex |= 0b10;

    mesh->setShader(this->defaultShaders[defaultShaderIndex]);

    this->meshes.emplace_back(mesh);

    return mesh;
}

uint32_t DX9Renderer::loadTexture(const void* textureData, size_t textureSize, uint32_t width, uint32_t height) {
    if (textureData == nullptr || textureSize == 0 || width <= 0 || height <= 0)
        return UINT16_MAX;

    constexpr auto format = D3DFMT_A8R8G8B8;
    const auto tex = this->allocateTexture(width, height, 1, format);

    {
        const auto rgba = static_cast<const uint8_t*>(textureData);

        const auto bgra = std::make_unique<uint8_t[]>(width * height * 4);

        for (int i = 0; i < width * height; i++) {
            for (int j = 0; j < 4; j++) {
                if (j == 3)
                    bgra[i * 4 + 3] = rgba[i * 4 + 3] & 0xFF;
                else
                    bgra[i * 4 + 2 - j] = rgba[i * 4 + j] & 0xFF;
            }
        }

        DX9::Utils::copyTexture(bgra.get(), width, height, tex);
    }

    const auto id = this->textures.size();

    this->textures.emplace_back(tex, width, height, format);

    return id;
}

LPDIRECT3DVERTEXBUFFER9 DX9Renderer::allocateVertexBuffer(const UINT length, const DWORD fvf) const {
    if (length == 0)
        return nullptr;

    LPDIRECT3DVERTEXBUFFER9 buf{};
    DX_CHECK(this->device->CreateVertexBuffer, length, 0, fvf, this->pool, &buf, nullptr);
    return buf;
}

LPDIRECT3DINDEXBUFFER9 DX9Renderer::allocateIndexBuffer(const UINT length) const {
    if (length == 0)
        return nullptr;

    LPDIRECT3DINDEXBUFFER9 buf{};
    DX_CHECK(this->device->CreateIndexBuffer, length, 0, D3DFMT_INDEX16, this->pool, &buf, nullptr);
    return buf;
}

LPDIRECT3DTEXTURE9 DX9Renderer::allocateTexture(const UINT width, const UINT height, const UINT levels, const D3DFORMAT format) const {
    if (width == 0 || height == 0 || levels == 0)
        return nullptr;

    LPDIRECT3DTEXTURE9 buf{};
    DX_CHECK(this->device->CreateTexture, width, height, levels, 0, format, this->pool, &buf, nullptr);
    return buf;
}

void DX9Renderer::createContext() {
    this->dxcontext = WF::DX9::Direct3DCreate9(D3D_SDK_VERSION);
}

void DX9Renderer::destroyContext() const {
    (void)this->dxcontext->Release();
}

void DX9Renderer::createDevice() {
    const auto size = this->window->getWindowSize();
    const auto hwnd = this->window->getHWND();

    D3DPRESENT_PARAMETERS params{};
    params.BackBufferWidth = size.x;
    params.BackBufferHeight = size.y;
    params.BackBufferFormat = D3DFMT_A8R8G8B8;
    params.BackBufferCount = 2;
    params.MultiSampleType = D3DMULTISAMPLE_NONE;
    params.MultiSampleQuality = 0;
    params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    params.hDeviceWindow = hwnd;
    params.Windowed = TRUE;
    params.EnableAutoDepthStencil = TRUE;
    params.AutoDepthStencilFormat = D3DFMT_D16;
    params.Flags = 0;
    params.FullScreen_RefreshRateInHz = 0;
    params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    auto res = this->dxcontext->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &params, &this->device);

    if (FAILED(res)) {
        res = this->dxcontext->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &params, &this->device);

        if (FAILED(res)) {
            res = this->dxcontext->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &params, &this->device);

            this->pool = D3DPOOL_SYSTEMMEM;
        }
    }

    if (this->device == nullptr) {
        std::cerr << "Failed to create Direct3D 9 device" << std::endl;
        return;
    }

    DX_CHECK(this->device->SetRenderState, D3DRS_FILLMODE, D3DFILL_SOLID);
    DX_CHECK(this->device->SetRenderState, D3DRS_ZWRITEENABLE, TRUE);
    DX_CHECK(this->device->SetRenderState, D3DRS_ALPHATESTENABLE, TRUE);
    DX_CHECK(this->device->SetRenderState, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    DX_CHECK(this->device->SetRenderState, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    DX_CHECK(this->device->SetRenderState, D3DRS_CULLMODE, D3DCULL_CW);
    DX_CHECK(this->device->SetRenderState, D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    DX_CHECK(this->device->SetRenderState, D3DRS_ALPHABLENDENABLE, TRUE);
    DX_CHECK(this->device->SetRenderState, D3DRS_LIGHTING, FALSE);
    DX_CHECK(this->device->SetRenderState, D3DRS_BLENDOP, D3DBLENDOP_ADD);
    DX_CHECK(this->device->SetRenderState, D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
    DX_CHECK(this->device->SetRenderState, D3DRS_SRCBLENDALPHA, D3DBLEND_SRCALPHA);
    DX_CHECK(this->device->SetRenderState, D3DRS_DESTBLENDALPHA, D3DBLEND_INVSRCALPHA);
    DX_CHECK(this->device->SetRenderState, D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);
}

void DX9Renderer::destroyDevice() const {
    (void)this->device->Release();
}

void DX9Renderer::destroyMeshes() {
    for (const auto& mesh : this->meshes) {
        mesh->cleanup();
    }

    this->meshes.clear();
}

void DX9Renderer::createTextures() {
    const auto black = glm::packUnorm4x8({0, 0, 0, 1});
    const auto magenta = glm::packUnorm4x8({1, 0, 1, 1});

    std::array<uint32_t, 16 * 16> errorPixels{};

    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 16; y++) {
            errorPixels[y * 16 + x] = (x >> 3) % 2 ^ (y >> 3) % 2 ? magenta : black;
        }
    }

    const auto id = this->loadTexture(errorPixels.data(), errorPixels.size(), 16, 16);

    this->missingTexture = {id, this->textures[id]};
}

void DX9Renderer::destroyTextures() {
    for (const auto image : this->textures) {
        if (image.texture != nullptr)
            (void)image.texture->Release();
    }

    this->textures.clear();
}

void DX9Renderer::createShaders() {
    this->defaultShaders[0] = Renderer::loadShader(Utils::loadBinary(this->hInstance, IDR_SHDR_DX9_POS_V), Utils::loadBinary(this->hInstance, IDR_SHDR_DX9_POS_F), VertexFormats::POSITION);
    this->defaultShaders[1] = Renderer::loadShader(Utils::loadBinary(this->hInstance, IDR_SHDR_DX9_POS_COL_V), Utils::loadBinary(this->hInstance, IDR_SHDR_DX9_POS_COL_F), VertexFormats::POSITION_COLOR);
    this->defaultShaders[2] = Renderer::loadShader(Utils::loadBinary(this->hInstance, IDR_SHDR_DX9_POS_TEX_V), Utils::loadBinary(this->hInstance, IDR_SHDR_DX9_POS_TEX_F), VertexFormats::POSITION_TEX);
    this->defaultShaders[3] = Renderer::loadShader(Utils::loadBinary(this->hInstance, IDR_SHDR_DX9_POS_TEX_COL_V), Utils::loadBinary(this->hInstance, IDR_SHDR_DX9_POS_TEX_COL_F), VertexFormats::POSITION_TEX_COLOR);
}

void DX9Renderer::destroyShaders() {
    for (auto& shader : this->shaders) {
        shader.cleanup();
    }

    this->shaders.clear();
}

void DX9Renderer::updateViewport() {
    const auto& size = this->cachedWindowSize;

    this->viewport.X = 0;
    this->viewport.Y = 0;
    this->viewport.Width = size.x;
    this->viewport.Height = size.y;
    this->viewport.MinZ = 0.f;
    this->viewport.MaxZ = 1.f;

    DX_CHECK(this->device->SetViewport, &this->viewport);
}

void DX9Renderer::updateMatrices() {
    this->projectionMatrix = glm::orthoRH_ZO(0.f, static_cast<float>(this->cachedWindowSize.x), static_cast<float>(this->cachedWindowSize.y), 0.f, 0.05f, 500.f);
    this->viewMatrix = {1.f};
}
