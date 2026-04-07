#pragma once

#include "Mesh.h"
#include "../Renderer.h"
#include "../utils/gif/GIF.h"

class AnimatedTexture : public GIF {
public:
    AnimatedTexture() : GIF() {}
    explicit AnimatedTexture(const std::string& file) : GIF(file) {}
    AnimatedTexture(const std::string& name, const b::EmbedInternal::EmbeddedFile& file) : GIF(name, file) {}

    void generate(const std::shared_ptr<Renderer>& renderer);
    void cleanup();

    void draw();

    [[nodiscard]] uint32_t getCurrentTextureId();

private:
    std::shared_ptr<Renderer> renderer{};
    std::shared_ptr<Mesh> mesh{};
    std::vector<uint32_t> textures{};
};
