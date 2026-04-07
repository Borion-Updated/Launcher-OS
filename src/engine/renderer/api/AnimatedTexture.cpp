#include "AnimatedTexture.h"

void AnimatedTexture::generate(const std::shared_ptr<Renderer>& renderer) {
    this->renderer = renderer;

    if (!this->valid)
        return;

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

    this->textures.reserve(this->frames);

    for (const auto& blob : this->frameData | std::views::keys) {
        this->textures.emplace_back(this->renderer->loadTexture(blob.data.get(), blob.size, this->width, this->height));
    }
}

void AnimatedTexture::cleanup() {
    this->renderer = nullptr;
    this->mesh = nullptr;
    this->textures.clear();
}

void AnimatedTexture::draw() {
    if (!this->valid)
        return;

    this->renderer->draw(this->mesh, this->getCurrentTextureId());
}

GLuint AnimatedTexture::getCurrentTextureId() {
    return this->textures[this->getCurrentFrame()];
}
