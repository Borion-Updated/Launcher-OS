#include "Mesh.h"

void Mesh::setShader(const uint32_t shaderId) {
    this->shaderId = shaderId;
}

uint32_t Mesh::getShaderId() const {
    return this->shaderId;
}
