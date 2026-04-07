#include "Tessellator.h"

#include "VertexFormats.h"
#include "../Renderer.h"

Tessellator::Tessellator(Renderer* renderer) {
    this->renderer = renderer;

    this->vertexBuffer.reserve(0x5000);
    this->indexBuffer.reserve(0x1000);
}

void Tessellator::begin() {
    this->clear();
}

Tessellator& Tessellator::vertex(const float x, const float y, const float z) {
    if (!this->fixedFormat)
        this->enabledAttributes.set(VertexFormats::VertexAttribute::ATTR_POSITION);

    this->vert = {x, y, z};
    return *this;
}

Tessellator& Tessellator::color(const float red, const float green, const float blue, const float alpha) {
    if (!this->fixedFormat)
        this->enabledAttributes.set(VertexFormats::VertexAttribute::ATTR_COLOR);

    this->col = {red, green, blue, alpha};
    return *this;
}

Tessellator& Tessellator::texture(const float u, const float v) {
    if (!this->fixedFormat)
        this->enabledAttributes.set(VertexFormats::VertexAttribute::ATTR_TEXCOORD);

    this->tex = {u, v};
    return *this;
}

void Tessellator::endVertex() {
    if (this->enabledAttributes[VertexFormats::VertexAttribute::ATTR_POSITION]) {
        this->vertexBuffer.emplace_back(this->vert.x);
        this->vertexBuffer.emplace_back(this->vert.y);
        this->vertexBuffer.emplace_back(this->vert.z);
    }

    if (this->enabledAttributes[VertexFormats::VertexAttribute::ATTR_TEXCOORD]) {
        this->vertexBuffer.emplace_back(this->tex.x);
        this->vertexBuffer.emplace_back(this->tex.y);
    }

    if (this->enabledAttributes[VertexFormats::VertexAttribute::ATTR_COLOR]) {
        this->vertexBuffer.emplace_back(this->col.r);
        this->vertexBuffer.emplace_back(this->col.g);
        this->vertexBuffer.emplace_back(this->col.b);
        this->vertexBuffer.emplace_back(this->col.a);
    }

    this->indexBuffer.emplace_back(static_cast<uint16_t>(this->indexBuffer.size()));

    this->vertexCount++;

    this->fixedFormat = true;
}

std::shared_ptr<Mesh> Tessellator::end() {
    uint8_t vertexSize = 0;

    for (int i = 0; i < VertexFormats::VertexAttribute::ATTR_COUNT; i++) {
        if (this->enabledAttributes[i])
            vertexSize += VertexFormats::ATTRIBUTE_SIZES[i];
    }

    auto mesh = this->renderer->upload(this->vertexBuffer.data(), this->vertexBuffer.size(), this->indexBuffer.data(), this->indexBuffer.size(), vertexSize, this->enabledAttributes);

    this->clear();

    return mesh;
}

void Tessellator::clear() {
    this->enabledAttributes.reset();
    this->fixedFormat = false;
    this->vertexCount = 0;
    this->vertexBuffer.clear();
    this->indexBuffer.clear();

    this->vert = {};
    this->col = {};
    this->tex = {};
}
