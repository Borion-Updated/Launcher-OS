#include "OGLRenderer.h"

#include "../../../utils/Logger.h"
#include "tessellation/Mesh.h"
#include "texture/TextureUtil.h"

bool OGLRenderer::init() {
    if (gladLoadGLLoader(this->window->getGLADloadproc()) == 0) {
        logF("Failed to initialize GLAD");
        std::cerr << "Failed to initialize GLAD" << std::endl;
        std::abort();
    }

    this->tessellator = std::make_shared<Tessellator>(this);

    this->loadShaders();
    this->initMatrices();

    this->context->init();

    glClearColor(0.f, 0.f, 0.f, 0.f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

void OGLRenderer::exit() {
    this->context->exit();

    this->cleanupMeshes();
    this->cleanupShaders();
}

void OGLRenderer::render(const double deltaTime) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto& defMatrix = this->stack.push();

    defMatrix = translate(defMatrix, {0.f, 0.f, -250.f});

    this->context->render(deltaTime);

    this->stack.pop();

    this->stack.reset();
}

Renderer::Type OGLRenderer::getRendererType() {
    return Type::OpenGL;
}

uint32_t OGLRenderer::loadShader(const void* vertexShaderData, const size_t vertexShaderSize, const void* fragmentShaderData, const size_t fragmentShaderSize, const std::span<std::string> attributes) {
    OGL::Shader shader{};

    shader.load(vertexShaderData, vertexShaderSize, fragmentShaderData, fragmentShaderSize, attributes);

    const auto id = this->shaders.size();

    this->shaders.emplace_back(shader);

    return static_cast<uint32_t>(id);
}

std::shared_ptr<Mesh> OGLRenderer::upload(float* vertices, const uint32_t vertexCount, uint16_t* indices, const uint32_t indexCount, const uint8_t vertexSize, const std::bitset<VertexFormats::VertexAttribute::ATTR_COUNT>& enabledAttributes) {
    auto mesh = std::make_shared<OGL::Mesh>();

    mesh->load(vertices, vertexCount, indices, indexCount, vertexSize, enabledAttributes);

    uint8_t index = 0;

    if (enabledAttributes[VertexFormats::VertexAttribute::ATTR_COLOR])
        index |= 0b1;

    if (enabledAttributes[VertexFormats::VertexAttribute::ATTR_TEXCOORD])
        index |= 0b10;

    mesh->setShader(this->defaultShaders[index]);

    this->uploadedMeshes.emplace_back(mesh);

    return mesh;
}

uint32_t OGLRenderer::loadTexture(const void* textureData, const size_t textureSize, const uint32_t width, const uint32_t height) {
    const auto texId = OGL::TextureUtil::loadTexture(textureData, width, height);

    const auto id = this->textures.size();

    this->textures.emplace_back(texId);

    return static_cast<uint32_t>(id);
}

void OGLRenderer::draw(const std::shared_ptr<Mesh> mesh) {
    if (mesh->getShaderId() < this->shaders.size())
        this->shaders[mesh->getShaderId()].bind(this->projectionMatrix, this->viewMatrix, this->stack.top(), this->shaderColor);

    mesh->draw();
}

void OGLRenderer::draw(const std::shared_ptr<Mesh> mesh, const uint32_t texId) {
    const auto texture = texId < this->textures.size() ? this->textures[texId] : this->missingTexture.second;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    if (mesh->getShaderId() < this->shaders.size())
        this->shaders[mesh->getShaderId()].bind(this->projectionMatrix, this->viewMatrix, this->stack.top(), this->shaderColor, texture);

    mesh->draw();

    glBindTexture(GL_TEXTURE_2D, 0);
}

void OGLRenderer::loadShaders() {
    this->defaultShaders[0] = Renderer::loadShader(b::embed<"shaders/opengl/position.vsh">(), b::embed<"shaders/opengl/position.fsh">(), VertexFormats::POSITION);
    this->defaultShaders[1] = Renderer::loadShader(b::embed<"shaders/opengl/position_color.vsh">(), b::embed<"shaders/opengl/position_color.fsh">(), VertexFormats::POSITION_COLOR);
    this->defaultShaders[2] = Renderer::loadShader(b::embed<"shaders/opengl/position_tex.vsh">(), b::embed<"shaders/opengl/position_tex.fsh">(), VertexFormats::POSITION_TEX);
    this->defaultShaders[3] = Renderer::loadShader(b::embed<"shaders/opengl/position_tex_color.vsh">(), b::embed<"shaders/opengl/position_tex_color.fsh">(), VertexFormats::POSITION_TEX_COLOR);
}

void OGLRenderer::cleanupShaders() {
    for (const auto& shader : this->shaders) {
        shader.cleanup();
    }

    this->shaders.clear();
}

void OGLRenderer::cleanupMeshes() {
    for (const auto& mesh : this->uploadedMeshes) {
        mesh->cleanup();
    }

    this->uploadedMeshes.clear();
}

void OGLRenderer::loadMissingTexture() {
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

void OGLRenderer::cleanupTextures() {
    glDeleteTextures(static_cast<GLsizei>(this->textures.size()), this->textures.data());
    this->textures.clear();
}

void OGLRenderer::initMatrices() {
    const auto size = this->window->getWindowSize();

    this->projectionMatrix = glm::ortho(0.f, static_cast<float>(size.x), static_cast<float>(size.y), 0.f, 0.05f, 500.f);
    this->viewMatrix = glm::mat4{1.f};
}
