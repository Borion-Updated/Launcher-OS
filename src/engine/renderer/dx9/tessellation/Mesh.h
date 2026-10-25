#pragma once

#include "../../api/Mesh.h"

namespace DX9 {
    class Mesh : public ::Mesh {
    public:
        ~Mesh() override = default;
        void load(float* vertices, uint32_t vertexCount, uint16_t* indices, uint32_t indexCount, uint8_t vertexSize, const std::bitset<VertexFormats::VertexAttribute::ATTR_COUNT>& enabledAttributes, void* additionalData) override;
        void cleanup() override;
        void draw(void* additionalData) const override;

    private:
        LPDIRECT3DVERTEXBUFFER9 vertexBuffer{};
        LPDIRECT3DINDEXBUFFER9 indexBuffer{};
        size_t vertexCount{};
        size_t indexCount{};
        uint8_t vertexSize{};
    };
}
