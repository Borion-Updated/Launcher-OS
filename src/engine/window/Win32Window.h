#pragma once

class Win32Window {
public:
    explicit Win32Window(const std::string& name);

    bool create(HINSTANCE hInstance);
    void destroy();

    void show() const;
    void hide() const;

    void run() const;

    void setRenderer(const std::shared_ptr<class Renderer>& renderer);

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
    HWND hwnd{};
    HINSTANCE hInstance{};
    HMODULE vulkanDLL{};
    HMODULE openGLDLL{};
    HDC hdc{};
    HGLRC hglrc{};
    WNDCLASSEXW windowClass{};

    void createFakeGLContext() const;

    static LRESULT windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
