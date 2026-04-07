#include "Renderer.h"

#include "../../utils/Logger.h"
#include "opengl/OGLRenderer.h"
#include "utils/stb_image.h"
#include "vulkan/VKRenderer.h"

std::shared_ptr<Renderer> Renderer::create(const Type type, const std::shared_ptr<Win32Window>& window, const std::shared_ptr<RendererContext>& context) {
    switch (type) {
    case Type::OpenGL:
        return std::make_shared<OGLRenderer>(window, context);
    case Type::Vulkan:
        return std::make_shared<VKRenderer>(window, context);
    default:
        std::cerr << "Reached impossible case" << std::endl;
        logF("No renderer");
        std::abort();
    }
}

void Renderer::setShaderColor(const float r, const float g, const float b, const float a) {
    this->setShaderColor(glm::vec4{r, g, b, a});
}

void Renderer::setShaderColor(const glm::vec4& color) {
    this->shaderColor = color;
}

uint32_t Renderer::loadShader(const b::EmbedInternal::EmbeddedFile& vertexShaderFile, const b::EmbedInternal::EmbeddedFile& fragmentShaderFile, const std::span<std::string> attributes) {
    return this->loadShader(vertexShaderFile.data(), vertexShaderFile.size(), fragmentShaderFile.data(), fragmentShaderFile.size(), attributes);
}

uint32_t Renderer::loadTexture(const b::EmbedInternal::EmbeddedFile& imageFile, uint32_t* width, uint32_t* height) {
    return this->loadTexture(imageFile.data(), imageFile.size(), width, height);
}

uint32_t Renderer::loadTexture(const void* imageData, const size_t imageSize, uint32_t* width, uint32_t* height) {
    int w, h, c;
    const auto data = stbi_load_from_memory(static_cast<stbi_uc const*>(imageData), imageSize, &w, &h, &c, 4);

    const auto id = this->loadTexture(data, w * h * c, w, h);

    if (width != nullptr)
        *width = w;

    if (height != nullptr)
        *height = h;

    stbi_image_free(data);

    return id;
}

std::shared_ptr<RendererContext> Renderer::getContext() {
    return this->context;
}

std::shared_ptr<Tessellator> Renderer::getTessellator() {
    return this->tessellator;
}

MatrixStack& Renderer::getModelMatrix() {
    return this->stack;
}
