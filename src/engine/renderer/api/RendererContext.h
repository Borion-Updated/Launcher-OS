#pragma once

class RendererContext {
public:
    explicit RendererContext(const std::shared_ptr<class Win32Window>& window) : window(window) {}

    virtual ~RendererContext() = default;
    virtual void init() = 0;
    virtual void exit();
    virtual void render(double deltaTime) = 0;
    virtual bool onClick(const glm::ivec2& pos, int button);
    virtual bool onRelease(const glm::ivec2& pos, int button);
    virtual void onMove(const glm::ivec2& pos, int button);

    void setRenderer(const std::shared_ptr<class Renderer>& renderer);

protected:
    std::shared_ptr<Win32Window> window{};
    std::shared_ptr<Renderer> renderer{};
};
