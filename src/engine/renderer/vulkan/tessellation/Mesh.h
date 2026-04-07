#pragma once

#include "../../api/Mesh.h"
#include "../utils/Buffer.h"

namespace VK {
    class Mesh : public ::Mesh {
    public:
        ~Mesh() override = default;
        void load(float* vertices, uint32_t vertexCount, uint16_t* indices, uint32_t indexCount, uint8_t vertexSize, const std::bitset<VertexFormats::VertexAttribute::ATTR_COUNT>& enabledAttributes, void* additionalData) override;
        void cleanup() override;
        void draw(void* additionalData) const override;

        Buffer indexBuffer;
        Buffer vertexBuffer;
        VkDeviceAddress vertexBufferAddress;
    };
}
