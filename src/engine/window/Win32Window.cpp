#include "Win32Window.h"

#include "../../utils/Logger.h"
#include "../../utils/TimeUtils.h"
#include "../../utils/Utils.h"
#include "../renderer/Renderer.h"
#include "../renderer/vulkan/Vulkan.h"

Win32Window::Win32Window(const std::wstring& name) {
    this->wname = name;
}

bool Win32Window::execute(HINSTANCE hInstance, const std::shared_ptr<RendererContext>& rendererContext) {
    const auto lastRenderer = Utils::readBin(Utils::getLauncherFolder() + L"renderer.bin");

    const auto maxRendererEx = lastRenderer.empty() ? magic_enum::enum_integer(RendererType::Count) : std::clamp(static_cast<uint8_t>(lastRenderer[0] + 1), uint8_t{1}, magic_enum::enum_integer(RendererType::Count));

    for (uint8_t i = 0; i < maxRendererEx; i++) {
        if (this->create(hInstance, magic_enum::enum_cast<RendererType>(maxRendererEx - 1 - i).value(), rendererContext))
            break;

        this->destroy();
    }

    if (this->hwnd == nullptr)
        return false;

    const auto type = this->renderer->getRendererType();

    Utils::writeBin(Utils::getLauncherFolder() + L"renderer.bin", &type, sizeof(RendererType));

    this->show();

    this->run();

    this->destroy();

    return true;
}

bool Win32Window::create(HINSTANCE hInstance, RendererType rendererType, const std::shared_ptr<RendererContext>& rendererContext) {
    return this->create(hInstance, Renderer::create(hInstance, rendererType, this, rendererContext), rendererContext);
}

bool Win32Window::create(HINSTANCE hInstance, const std::shared_ptr<Renderer>& renderer, const std::shared_ptr<RendererContext>& rendererContext) {
    if (this->hwnd != nullptr)
        return false;

    WF::User32::init();
    WF::DWM::init();

    this->iconified = false;
    this->requestedQuit = false;

    const auto icon = Utils::loadImage(hInstance, IDR_IMG_ICON);
    const auto hIcon = Utils::convertToHICON(icon.data(), icon.size());

    this->hInstance = hInstance;
    this->renderer = renderer;
    this->rendererContext = rendererContext;

    rendererContext->setWindow(this);
    rendererContext->setRenderer(renderer);

    this->windowClass.cbSize = sizeof(WNDCLASSEX);
    this->windowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    this->windowClass.lpfnWndProc = windowProc;
    this->windowClass.cbClsExtra = 0;
    this->windowClass.cbWndExtra = 0;
    this->windowClass.hInstance = hInstance;
    this->windowClass.hIcon = hIcon;
    this->windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    this->windowClass.hbrBackground = nullptr;
    this->windowClass.lpszMenuName = nullptr;
    this->windowClass.lpszClassName = this->wname.c_str();
    this->windowClass.hIconSm = hIcon;

    if (RegisterClassExW(&this->windowClass) == 0) {
        const auto lastError = GetLastError();

        logF("Failed to register class: {}", lastError);
        return false;
    }

    const int screenWidth  = GetSystemMetrics(SM_CXSCREEN);
    const int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    constexpr int width = 854;
    constexpr int height = 480;

    const auto fSize = this->getAdjustedWindowSize(width, height);

    this->hwnd = CreateWindowExW(WS_EX_LAYERED, this->windowClass.lpszClassName, this->wname.c_str(), WS_POPUP,
        (screenWidth - fSize.x) / 2, (screenHeight - fSize.y) / 2, fSize.x, fSize.y, nullptr, nullptr, this->hInstance, nullptr);

    if (this->hwnd == nullptr) {
        const auto lastError = GetLastError();

        logF("Failed to create window: {}", lastError);
        return false;
    }

    (void)SetWindowLongPtrW(this->hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    (void)WF::User32::SetLayeredWindowAttributes(this->hwnd, 0, 255, LWA_ALPHA);

    constexpr MARGINS margins = {.cxLeftWidth = -1};
    (void)WF::DWM::DwmExtendFrameIntoClientArea(this->hwnd, &margins);

    const auto rendererType = this->renderer->getRendererType();

    if (rendererType == RendererType::OpenGL) {
        WF::OGL::init();

        this->hdc = GetDC(this->hwnd);

        const int pixelAttribs[] = {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
            WGL_SWAP_METHOD_ARB, WGL_SWAP_COPY_ARB,
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
            WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
            WGL_COLOR_BITS_ARB, 32,
            WGL_ALPHA_BITS_ARB, 8,
            WGL_DEPTH_BITS_ARB, 24,
            0
        };

        int pixelFormat;
        UINT numFormats;
        if (WF::OGL::wglChoosePixelFormatARB(this->hdc, pixelAttribs, nullptr, 1, &pixelFormat, &numFormats) != TRUE) {
            const auto lastError = GetLastError();

            logF("Failed to choose pixel format: {}", lastError);
            return false;
        }

        PIXELFORMATDESCRIPTOR pfd{};
        if (WF::GDI::DescribePixelFormat(this->hdc, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd) == 0) {
            const auto lastError = GetLastError();

            logF("Failed to describe pixel format: {}", lastError);
            return false;
        }

        if (WF::GDI::SetPixelFormat(this->hdc, pixelFormat, &pfd) != TRUE) {
            const auto lastError = GetLastError();

            logF("Failed to set pixel format: {}", lastError);
            return false;
        }

        const int contextAttributes[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
            WGL_CONTEXT_MINOR_VERSION_ARB, 3,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0
        };

        this->hglrc = WF::OGL::wglCreateContextAttribsARB(this->hdc, nullptr, contextAttributes);

        (void)WF::OGL::wglMakeCurrent(this->hdc, this->hglrc);
        (void)WF::OGL::wglSwapIntervalEXT(1);
    }
    else if (rendererType == RendererType::Vulkan) {
        this->vulkanDLL = LoadLibraryW(L"vulkan-1.dll");

        if (this->vulkanDLL == nullptr)
            return false;
    }

    return this->renderer->init();
}

void Win32Window::destroy() {
    if (this->renderer == nullptr || this->hwnd == nullptr)
        return;

    const auto rendererType = this->renderer->getRendererType();

    if (rendererType == RendererType::OpenGL) {
        (void)WF::OGL::wglMakeCurrent(nullptr, nullptr);
        (void)ReleaseDC(this->hwnd, this->hdc);
        (void)WF::OGL::wglDeleteContext(this->hglrc);
    }

    this->renderer->exit();

    (void)DestroyWindow(this->hwnd);
    (void)UnregisterClassW(this->windowClass.lpszClassName, this->hInstance);

    this->hwnd = nullptr;
    this->hInstance = nullptr;

    if (rendererType == RendererType::OpenGL)
        WF::OGL::exit();
    else if (rendererType == RendererType::Vulkan && this->vulkanDLL != nullptr) {
        (void)FreeLibrary(this->vulkanDLL);
        this->vulkanDLL = nullptr;
    }

    WF::DWM::exit();
    WF::User32::exit();
}

void Win32Window::show() const {
    if (this->hwnd == nullptr)
        return;

    (void)ShowWindow(this->hwnd, SW_SHOW);
}

void Win32Window::hide() const {
    if (this->hwnd == nullptr)
        return;

    (void)ShowWindow(this->hwnd, SW_HIDE);
}

void Win32Window::run() const {
    if (this->renderer == nullptr || this->hwnd == nullptr)
        return;

    const auto rendererType = this->renderer->getRendererType();
    const bool swapBuffers = rendererType == RendererType::OpenGL;

    double deltaTime = 0.0;
    auto start = TimeUtils::currentTimeMillis();

    MSG msg{};
    while (msg.message != WM_QUIT && !this->requestedQuit) {
        if (PeekMessageW(&msg, this->hwnd, 0, 0, PM_REMOVE)) {
            (void)TranslateMessage(&msg);
            (void)DispatchMessageW(&msg);
        }

        if (this->iconified)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        else {
            this->renderer->render(deltaTime);

            if (swapBuffers)
                (void)WF::GDI::SwapBuffers(this->hdc);
        }

        const auto end = TimeUtils::currentTimeMillis();
        const auto diff = end - start;
        deltaTime = static_cast<double>(diff) / 1000.0;
        start = end;
    }
}

PFN_vkVoidFunction Win32Window::getVulkanInstanceFunction(VkInstance instance, const char* pName) const {
    static auto func = reinterpret_cast<PFN_vkGetInstanceProcAddr>(GetProcAddress(this->vulkanDLL, "vkGetInstanceProcAddr"));
    return func(instance, pName);
}

PFN_vkVoidFunction Win32Window::getVulkanInstanceFunctionKHR(const char* pName) const {
    return reinterpret_cast<PFN_vkVoidFunction>(GetProcAddress(this->vulkanDLL, pName));
}

std::vector<const char*> Win32Window::getRequiredExtensions() const {
    return {VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME};
}

VkSurfaceKHR Win32Window::createVulkanSurface(VkInstance instance, VkAllocationCallbacks* cb) const {
    VkSurfaceKHR surface{};

    VkWin32SurfaceCreateInfoKHR info{};
    info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    info.pNext = nullptr;

    info.flags = 0;
    info.hinstance = this->windowClass.hInstance;
    info.hwnd = this->hwnd;

    VK_CHECK(vkCreateWin32SurfaceKHR, instance, &info, cb, &surface);

    return surface;
}

static void* getGLFuncAddr(const char* name) {
    void* res = nullptr;

    if (WF::OGL::hasGetProcAddress())
        res = WF::OGL::wglGetProcAddress(name);

    if (WF::OGL::getModule() != nullptr && res == nullptr)
        res = GetProcAddress(WF::OGL::getModule(), name);

    return res;
}

GLADloadproc Win32Window::getGLADloadproc() const {
    return getGLFuncAddr;
}

HWND Win32Window::getHWND() const {
    return this->hwnd;
}

static int mouseX = 0, mouseY = 0;
glm::ivec2 Win32Window::getMousePos() const {
    return {mouseX, mouseY};
}

glm::ivec2 Win32Window::getWindowSize() const {
    RECT rect{};
    (void)GetClientRect(this->hwnd, &rect);

    const auto w = rect.right - rect.left;
    const auto h = rect.bottom - rect.top;

    return {w, h};
}

glm::ivec2 Win32Window::getAdjustedWindowSize(const int targetWidth, const int targetHeight) const {
    const auto dpiAware = WF::User32::SetProcessDPIAware();

    constexpr auto exStyle = WS_EX_TRANSPARENT;
    constexpr auto style = WS_POPUP;

    RECT rect{.left = 0, .top = 0, .right = targetWidth, .bottom = targetHeight};

    if (dpiAware)
        (void)WF::User32::AdjustWindowRectExForDpi(&rect, style, FALSE, exStyle, WF::User32::GetDpiForSystem());
    else
        (void)WF::User32::AdjustWindowRectEx(&rect, style, FALSE, exStyle);

    const int fWidth = rect.right - rect.left;
    const int fHeight = rect.bottom - rect.top;

    return {fWidth, fHeight};
}

static bool clicked = false;
static POINT posDiff{};

LRESULT WINAPI Win32Window::windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    const auto window = reinterpret_cast<Win32Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    if (window == nullptr)
        return DefWindowProcW(hwnd, msg, wParam, lParam);

    switch (msg) {
        case WM_SIZE: {
            if (wParam == SIZE_MINIMIZED)
                window->iconified = true;
            else
                window->iconified = false;
            return 0;
        }
        case WM_LBUTTONDOWN: {
            if (window->rendererContext != nullptr) {
                if (window->rendererContext->onClick({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}, 0))
                    return 0;
            }

            clicked = true;

            RECT rect{};
            (void)GetWindowRect(hwnd, &rect);

            POINT pos{};
            (void)GetCursorPos(&pos);

            posDiff = {pos.x - rect.left, pos.y - rect.top};
            return 0;
        }
        case WM_LBUTTONUP: {
            clicked = false;

            if (window->rendererContext != nullptr) {
                if (window->rendererContext->onRelease({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}, 0))
                    return 0;
            }

            return 0;
        }
        case WM_RBUTTONDOWN: {
            if (window->rendererContext != nullptr) {
                if (window->rendererContext->onClick({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}, 1))
                    return 0;
            }

            break;
        }
        case WM_RBUTTONUP: {
            if (window->rendererContext != nullptr) {
                if (window->rendererContext->onRelease({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}, 1))
                    return 0;
            }

            break;
        }
        case WM_MBUTTONDOWN: {
            if (window->rendererContext != nullptr) {
                if (window->rendererContext->onClick({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}, 2))
                    return 0;
            }

            break;
        }
        case WM_MBUTTONUP: {
            if (window->rendererContext != nullptr) {
                if (window->rendererContext->onRelease({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}, 2))
                    return 0;
            }

            break;
        }
        case WM_MOUSEMOVE: {
            if (window->rendererContext != nullptr) {
                const auto context = window->rendererContext;

                const auto pos = glm::ivec2{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

                if ((wParam & MK_LBUTTON) != 0)
                    context->onMove(pos, 0);

                if ((wParam & MK_RBUTTON) != 0)
                    context->onMove(pos, 1);

                if ((wParam & MK_MBUTTON) != 0)
                    context->onMove(pos, 2);
            }

            if (clicked) {
                POINT pos{};
                (void)GetCursorPos(&pos);

                RECT rect{};
                (void)GetWindowRect(hwnd, &rect);

                /*const auto w = rect.right - rect.left;
                const auto h = rect.bottom - rect.top;

                const auto screenWidth  = GetSystemMetrics(SM_CXSCREEN);
                const auto screenHeight = GetSystemMetrics(SM_CYSCREEN);

                const auto newX = std::clamp(pos.x - posDiff.x, static_cast<LONG>(0), screenWidth - w);
                const auto newY = std::clamp(pos.y - posDiff.y, static_cast<LONG>(0), screenHeight - h);*/

                const auto newX = pos.x - posDiff.x;
                const auto newY = pos.y - posDiff.y;

                (void)SetWindowPos(hwnd, HWND_TOP, newX, newY, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW);
            }

            mouseX = GET_X_LPARAM(lParam);
            mouseY = GET_Y_LPARAM(lParam);
            return 0;
        }
        case WM_CLOSE:
        case WM_DESTROY: {
            window->requestedQuit = true;
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}
