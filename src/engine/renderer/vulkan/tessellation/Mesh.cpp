#include "Mesh.h"

#include "../VKRenderer.h"
#include "../Vulkan.h"
#include "../utils/FragPushConstants.h"
#include "../utils/PushConstants.h"
#include "../utils/Vertex.h"

void VK::Mesh::load(float* vertices, const uint32_t vertexCount, uint16_t* indices, const uint32_t indexCount, const uint8_t vertexSize, const std::bitset<VertexFormats::VertexAttribute::ATTR_COUNT>& enabledAttributes, void* additionalData) {
    std::vector<Vertex> vkVertices{};
    vkVertices.resize(vertexCount);

    for (int i = 0; i < indexCount; i++) {
        Vertex vertex{};

        const int vertexIndex = i * vertexSize;

        int off = 0;

        if (enabledAttributes[VertexFormats::VertexAttribute::ATTR_POSITION]) {
            vertex.position.x = vertices[vertexIndex + off];
            vertex.position.y = vertices[vertexIndex + off + 1];
            vertex.position.z = vertices[vertexIndex + off + 2];

            off += 3;
        }

        if (enabledAttributes[VertexFormats::VertexAttribute::ATTR_TEXCOORD]) {
            vertex.uvX = vertices[vertexIndex + off];
            vertex.uvY = vertices[vertexIndex + off + 1];

            off += 2;
        }

        if (enabledAttributes[VertexFormats::VertexAttribute::ATTR_COLOR]) {
            vertex.color.r = vertices[vertexIndex + off];
            vertex.color.g = vertices[vertexIndex + off + 1];
            vertex.color.b = vertices[vertexIndex + off + 2];
            vertex.color.a = vertices[vertexIndex + off + 3];

            off += 4;
        }

        vkVertices[i] = vertex;
    }

    const auto renderer = static_cast<VKRenderer*>(additionalData);

    renderer->uploadMeshRaw(vkVertices, indices, indexCount, *this);
}

void VK::Mesh::cleanup() {}

void VK::Mesh::draw(void* additionalData) const {
    const auto data = static_cast<VKRenderer::DrawData*>(additionalData);

    const auto cmd = data->cmd;

    {
        PushConstants constants{};

        constants.worldMatrix = data->matrix;
        constants.vertexBuffer = this->vertexBufferAddress;

        Vulkan::vkCmdPushConstants(cmd, data->pipeline->layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), &constants);
    }

    {
        FragPushConstants constants{};

        constants.shaderColor = data->shaderColor;

        Vulkan::vkCmdPushConstants(cmd, data->pipeline->layout, VK_SHADER_STAGE_FRAGMENT_BIT, 80, sizeof(FragPushConstants), &constants);
    }

    Vulkan::vkCmdBindIndexBuffer(cmd, this->indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);

    Vulkan::vkCmdDrawIndexed(cmd, this->indexBuffer.size / sizeof(uint16_t), 1, 0, 0, 0);
}
