#pragma once

#include "../Renderer.h"
#include "shader/Shader.h"

class OGLRenderer : public Renderer {
public:
    OGLRenderer(const std::shared_ptr<Win32Window>& window, const std::shared_ptr<RendererContext>& context) : Renderer(window, context) {}

    bool init() override;
    void exit() override;
    void render(double deltaTime) override;
    Type getRendererType() override;
    uint32_t loadShader(const void* vertexShaderData, size_t vertexShaderSize, const void* fragmentShaderData, size_t fragmentShaderSize, std::span<std::string> attributes) override;
    std::shared_ptr<Mesh> upload(float* vertices, uint32_t vertexCount, uint16_t* indices, uint32_t indexCount, uint8_t vertexSize, const std::bitset<VertexFormats::VertexAttribute::ATTR_COUNT>& enabledAttributes) override;
    uint32_t loadTexture(const void* textureData, size_t textureSize, uint32_t width, uint32_t height) override;
    void draw(std::shared_ptr<Mesh> mesh) override;
    void draw(std::shared_ptr<Mesh> mesh, uint32_t texId) override;

private:
    std::array<uint32_t, 4> defaultShaders{};
    std::vector<std::shared_ptr<Mesh>> uploadedMeshes{};
    std::vector<OGL::Shader> shaders{};
    std::vector<GLuint> textures{};
    std::pair<uint32_t, GLuint> missingTexture{};

    void loadShaders();
    void cleanupShaders();

    void cleanupMeshes();

    void loadMissingTexture();
    void cleanupTextures();

    void initMatrices();
};
