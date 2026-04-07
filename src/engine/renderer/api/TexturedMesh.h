#pragma once

#include "Mesh.h"

class TexturedMesh {
public:
    void load(const b::EmbedInternal::EmbeddedFile& imageFile, const std::shared_ptr<class Renderer>& renderer);
    void load(const void* imageData, size_t imageSize, const std::shared_ptr<Renderer>& renderer);

    void cleanup();

    void draw() const;

    [[nodiscard]] uint32_t getWidth() const;
    [[nodiscard]] uint32_t getHeight() const;

private:
    std::shared_ptr<Renderer> renderer{};
    std::shared_ptr<Mesh> mesh{};
    uint32_t width{};
    uint32_t height{};
    uint32_t texId{};
};
