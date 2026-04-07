#include "Mesh.h"

OGL::Mesh::~Mesh() {
    this->Mesh::cleanup();
}

void OGL::Mesh::load(float* vertices, const uint32_t vertexCount, uint16_t* indices, const uint32_t indexCount, const uint8_t vertexSize, const std::bitset<VertexFormats::VertexAttribute::ATTR_COUNT>& enabledAttributes, void* additionalData) {
    this->vertexCount = indexCount;

    glGenVertexArrays(1, &this->vaoId);
    glBindVertexArray(this->vaoId);

    glGenBuffers(1, &this->vboId);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboId);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    int attributeIndex = 0;
    int offset = 0;

    if (enabledAttributes[VertexFormats::VertexAttribute::ATTR_POSITION]) {
        glVertexAttribPointer(attributeIndex, 3, GL_FLOAT, GL_FALSE, vertexSize * sizeof(GLfloat), reinterpret_cast<void*>(offset * sizeof(float)));
        glEnableVertexAttribArray(attributeIndex);

        offset += 3;
        attributeIndex++;
    }

    if (enabledAttributes[VertexFormats::VertexAttribute::ATTR_TEXCOORD]) {
        glVertexAttribPointer(attributeIndex, 2, GL_FLOAT, GL_FALSE, vertexSize * sizeof(GLfloat), reinterpret_cast<void*>(offset * sizeof(float)));
        glEnableVertexAttribArray(attributeIndex);

        offset += 2;
        attributeIndex++;
    }

    if (enabledAttributes[VertexFormats::VertexAttribute::ATTR_COLOR]) {
        glVertexAttribPointer(attributeIndex, 4, GL_FLOAT, GL_FALSE, vertexSize * sizeof(GLfloat), reinterpret_cast<void*>(offset * sizeof(float)));
        glEnableVertexAttribArray(attributeIndex);

        offset += 4;
        attributeIndex++;
    }

    glGenBuffers(1, &this->eboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->eboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(GLushort), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void OGL::Mesh::cleanup() {
    glDeleteBuffers(1, &this->vboId);
    glDeleteBuffers(1, &this->eboId);
    glDeleteVertexArrays(1, &this->vaoId);
}

void OGL::Mesh::draw(void* additionalData) const {
    glBindVertexArray(this->vaoId);
    glDrawElements(GL_TRIANGLES, this->vertexCount, GL_UNSIGNED_SHORT, nullptr);
    glBindVertexArray(0);
}
