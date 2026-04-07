#include "RendererContext.h"

void RendererContext::exit() {}

bool RendererContext::onClick(const glm::ivec2& pos, int button) {
    return false;
}

bool RendererContext::onRelease(const glm::ivec2& pos, int button) {
    return false;
}

void RendererContext::onMove(const glm::ivec2& pos, int button) {}

void RendererContext::setRenderer(const std::shared_ptr<Renderer>& renderer) {
    this->renderer = renderer;
}
