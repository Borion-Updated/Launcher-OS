#pragma once

#include "../../api/Mesh.h"

namespace OGL {
    class Mesh : public ::Mesh {
    public:
        ~Mesh() override;
        void load(float* vertices, uint32_t vertexCount, uint16_t* indices, uint32_t indexCount, uint8_t vertexSize, const std::bitset<VertexFormats::VertexAttribute::ATTR_COUNT>& enabledAttributes, void* additionalData = nullptr) override;
        void cleanup() override;
        void draw(void* additionalData = nullptr) const override;

        GLuint vaoId{};
        GLuint vboId{};
        GLuint eboId{};
        int vertexCount{};
    };
}
