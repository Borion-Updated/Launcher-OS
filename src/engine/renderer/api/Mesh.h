#pragma once

#include "VertexFormats.h"

class Mesh {
public:
    virtual ~Mesh() = default;
    virtual void load(float* vertices, uint32_t vertexCount, uint16_t* indices, uint32_t indexCount, uint8_t vertexSize, const std::bitset<VertexFormats::VertexAttribute::ATTR_COUNT>& enabledAttributes, void* additionalData = nullptr) = 0;
    virtual void cleanup() = 0;
    virtual void draw(void* additionalData = nullptr) const = 0;

    void setShader(uint32_t shaderId);
    [[nodiscard]] uint32_t getShaderId() const;

private:
    uint32_t shaderId{};
};
