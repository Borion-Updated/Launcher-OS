#pragma once

#include "../renderer/RendererType.h"
#include "../renderer/api/RendererContext.h"

class Win32Window {
public:
    explicit Win32Window(const std::wstring& name);

    bool execute(HINSTANCE hInstance, const std::shared_ptr<RendererContext>& rendererContext);

    bool create(HINSTANCE hInstance, RendererType rendererType, const std::shared_ptr<RendererContext>& rendererContext);
    bool create(HINSTANCE hInstance, const std::shared_ptr<Renderer>& renderer, const std::shared_ptr<RendererContext>& rendererContext);
    void destroy();

    void show() const;
    void hide() const;

    void run() const;

    PFN_vkVoidFunction getVulkanInstanceFunction(VkInstance instance, const char* pName) const;
    PFN_vkVoidFunction getVulkanInstanceFunctionKHR(const char* pName) const;

    [[nodiscard]] std::vector<const char*> getRequiredExtensions() const;

    VkSurfaceKHR createVulkanSurface(VkInstance instance, VkAllocationCallbacks* cb) const;

    [[nodiscard]] GLADloadproc getGLADloadproc() const;

    [[nodiscard]] HWND getHWND() const;
    [[nodiscard]] glm::ivec2 getMousePos() const;
    [[nodiscard]] glm::ivec2 getWindowSize() const;

private:
    std::wstring wname{};
    std::shared_ptr<Renderer> renderer{};
    std::shared_ptr<RendererContext> rendererContext{};
    HWND hwnd{};
    HINSTANCE hInstance{};
    HMODULE vulkanDLL{};
    HMODULE openGLDLL{};
    HDC hdc{};
    HGLRC hglrc{};
    WNDCLASSEXW windowClass{};
    bool iconified{};
    bool requestedQuit{};

    [[nodiscard]] glm::ivec2 getAdjustedWindowSize(int targetWidth, int targetHeight) const;

    static LRESULT WINAPI windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
