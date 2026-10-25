#include "Mesh.h"

#include "../DX9Renderer.h"
#include "../DXCheck.h"
#include "../utils/Utils.h"

void DX9::Mesh::load(float* vertices, uint32_t vertexCount, uint16_t* indices, uint32_t indexCount, uint8_t vertexSize, const std::bitset<VertexFormats::VertexAttribute::ATTR_COUNT>& enabledAttributes, void* additionalData) {
    if (vertexCount == 0 || indexCount == 0 || vertexSize == 0)
        return;

    const auto renderer = static_cast<DX9Renderer*>(additionalData);

    const auto vertexBufferSize = sizeof(float) * vertexCount;
    const auto indexBufferSize = sizeof(uint16_t) * indexCount;

    this->vertexBuffer = renderer->allocateVertexBuffer(vertexBufferSize, 0);
    this->indexBuffer = renderer->allocateIndexBuffer(indexBufferSize);
    this->vertexSize = vertexSize * sizeof(float);
    this->vertexCount = vertexCount;
    this->indexCount = indexCount;

    if (this->vertexBuffer != nullptr)
        Utils::copyVertices(vertices, vertexBufferSize, this->vertexBuffer);

    if (this->indexBuffer != nullptr)
        Utils::copyIndices(indices, indexBufferSize, this->indexBuffer);
}

void DX9::Mesh::cleanup() {
    if (this->vertexBuffer != nullptr) {
        (void)this->vertexBuffer->Release();
        this->vertexBuffer = nullptr;
    }

    if (this->indexBuffer != nullptr) {
        (void)this->indexBuffer->Release();
        this->indexBuffer = nullptr;
    }
}

void DX9::Mesh::draw(void* additionalData) const {
    if (this->vertexBuffer == nullptr || this->indexBuffer == nullptr)
        return;

    const auto device = static_cast<LPDIRECT3DDEVICE9>(additionalData);

    DX_CHECK(device->SetFVF, 0);

    DX_CHECK(device->SetStreamSource, 0, this->vertexBuffer, 0, this->vertexSize);
    DX_CHECK(device->SetIndices, this->indexBuffer);

    DX_CHECK(device->DrawIndexedPrimitive, D3DPT_TRIANGLELIST, 0, 0, this->indexCount, 0, this->indexCount / 3);
}
