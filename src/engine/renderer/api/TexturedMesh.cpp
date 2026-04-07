#include "TexturedMesh.h"

#include "../Renderer.h"

void TexturedMesh::load(const b::EmbedInternal::EmbeddedFile& imageFile, const std::shared_ptr<Renderer>& renderer) {
    this->load(imageFile.data(), imageFile.size(), renderer);
}

void TexturedMesh::load(const void* imageData, const size_t imageSize, const std::shared_ptr<Renderer>& renderer) {
    this->renderer = renderer;
    this->texId = this->renderer->loadTexture(imageData, imageSize, &this->width, &this->height);

    const auto w = static_cast<float>(this->width);
    const auto h = static_cast<float>(this->height);

    const auto tess = renderer->getTessellator();

    tess->begin();

    tess->vertex(0.f, 0.f, 0.f).texture(0.f, 0.f).endVertex();
    tess->vertex(0.f, h, 0.f).texture(0.f, 1.f).endVertex();
    tess->vertex(w, 0.f, 0.f).texture(1.f, 0.f).endVertex();

    tess->vertex(0.f, h, 0.f).texture(0.f, 1.f).endVertex();
    tess->vertex(w, h, 0.f).texture(1.f, 1.f).endVertex();
    tess->vertex(w, 0.f, 0.f).texture(1.f, 0.f).endVertex();

    this->mesh = tess->end();
}

void TexturedMesh::cleanup() {
    this->renderer = nullptr;
    this->mesh = nullptr;

    this->width = 0;
    this->height = 0;
    this->texId = 0;
}

void TexturedMesh::draw() const {
    this->renderer->draw(this->mesh, this->texId);
}

uint32_t TexturedMesh::getWidth() const {
    return this->width;
}

uint32_t TexturedMesh::getHeight() const {
    return this->height;
}
