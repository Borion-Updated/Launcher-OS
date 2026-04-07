#pragma once

#include "../window/Win32Window.h"
#include "api/MatrixStack.h"
#include "api/RendererContext.h"
#include "api/Tessellator.h"

class Renderer {
public:
    enum class Type : uint8_t {
        OpenGL,
        Vulkan
    };

    static std::shared_ptr<Renderer> create(Type type, const std::shared_ptr<Win32Window>& window, const std::shared_ptr<RendererContext>& context);

    Renderer(const std::shared_ptr<Win32Window>& window, const std::shared_ptr<RendererContext>& context) : window(window), context(context) {}

    virtual ~Renderer() = default;
    virtual bool init() = 0;
    virtual void exit() = 0;
    virtual void render(double deltaTime) = 0;
    virtual Type getRendererType() = 0;
    virtual uint32_t loadShader(const void* vertexShaderData, size_t vertexShaderSize, const void* fragmentShaderData, size_t fragmentShaderSize, std::span<std::string> attributes) = 0;
    virtual std::shared_ptr<Mesh> upload(float* vertices, uint32_t vertexCount, uint16_t* indices, uint32_t indexCount, uint8_t vertexSize, const std::bitset<VertexFormats::VertexAttribute::ATTR_COUNT>& enabledAttributes) = 0;
    virtual uint32_t loadTexture(const void* textureData, size_t textureSize, uint32_t width, uint32_t height) = 0;
    virtual void draw(std::shared_ptr<Mesh> mesh) = 0;
    virtual void draw(std::shared_ptr<Mesh> mesh, uint32_t texId) = 0;

    void setShaderColor(float r, float g, float b, float a = 1.f);
    void setShaderColor(const glm::vec4& color = {1.f, 1.f, 1.f, 1.f});
    uint32_t loadShader(const b::EmbedInternal::EmbeddedFile& vertexShaderFile, const b::EmbedInternal::EmbeddedFile& fragmentShaderFile, std::span<std::string> attributes);
    uint32_t loadTexture(const b::EmbedInternal::EmbeddedFile& imageFile, uint32_t* width, uint32_t* height);
    uint32_t loadTexture(const void* imageData, size_t imageSize, uint32_t* width, uint32_t* height);
    std::shared_ptr<RendererContext> getContext();
    std::shared_ptr<Tessellator> getTessellator();
    MatrixStack& getModelMatrix();

protected:
    std::shared_ptr<Win32Window> window{};
    std::shared_ptr<RendererContext> context{};
    std::shared_ptr<Tessellator> tessellator{};
    glm::mat4 projectionMatrix{};
    glm::mat4 viewMatrix{};
    MatrixStack stack{};
    glm::vec4 shaderColor{};
};
