#pragma once

#include "../engine/renderer/api/AnimatedTexture.h"
#include "../engine/renderer/api/RendererContext.h"
#include "../engine/renderer/api/TexturedMesh.h"

class BorionContext : public RendererContext {
public:
    explicit BorionContext(const std::shared_ptr<Win32Window>& window) : RendererContext(window) {}

    void init() override;
    void exit() override;

    void render(double deltaTime) override;

    bool onClick(const glm::ivec2& pos, int button) override;
    bool onRelease(const glm::ivec2& pos, int button) override;

private:
    TexturedMesh background{};
    TexturedMesh banner{};
    TexturedMesh download{};
    TexturedMesh close{};
    AnimatedTexture kirby{};

    float x = 0.f;
    float y = 0.f;
    bool horizontal = true;
    bool vertical = true;

    float rot = 0.f;
    float dlScale = 1.f;
    bool down = true;
    bool leftDown = false;
    bool rightDown = false;

    float closeCol = 1.f;

    bool injecting = false;
    std::wstring tmpPath{};

    [[nodiscard]] glm::vec4 getDownloadButtonDimensions() const;
    [[nodiscard]] bool isDownloadButtonHovered(const glm::ivec2& mousePos) const;

    [[nodiscard]] glm::vec4 getCloseButtonDimensions() const;
    [[nodiscard]] bool isCloseButtonHovered(const glm::ivec2& mousePos) const;

    static bool isHovered(const glm::vec4& dim, const glm::ivec2& mousePos);
};
