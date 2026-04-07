#pragma once

#include "Mesh.h"
#include "VertexFormats.h"

class Tessellator {
public:
    Tessellator(class Renderer* renderer);

    void begin();

    Tessellator& vertex(float x, float y, float z);
    Tessellator& color(float red, float green, float blue, float alpha = 1.f);
    Tessellator& texture(float u, float v);
    void endVertex();

    std::shared_ptr<Mesh> end();

    void clear();

private:
    Renderer* renderer{};
    std::vector<float> vertexBuffer{};
    std::vector<uint16_t> indexBuffer{};
    int vertexCount = 0;
    std::bitset<VertexFormats::VertexAttribute::ATTR_COUNT> enabledAttributes{};
    bool fixedFormat = false;
    glm::vec3 vert{};
    glm::vec4 col{};
    glm::vec2 tex{};
};
