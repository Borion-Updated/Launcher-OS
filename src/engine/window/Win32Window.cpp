#include "Win32Window.h"

#include "../../utils/Logger.h"
#include "../../utils/TimeUtils.h"
#include "../../utils/Utils.h"
#include "../renderer/Renderer.h"
#include "../renderer/vulkan/Vulkan.h"

HMODULE glMod = nullptr;

using wglGetProcAddress_t = void*(*)(const char*);
static wglGetProcAddress_t FwglGetProcAddress = nullptr;

using wglCreateContext_t = HGLRC(*)(HDC);
static wglCreateContext_t FwglCreateContext = nullptr;

using wglDeleteContext_t = BOOL(*)(HGLRC);
static wglDeleteContext_t FwglDeleteContext = nullptr;

using wglMakeCurrent_t = BOOL(*)(HDC, HGLRC);
static wglMakeCurrent_t FwglMakeCurrent = nullptr;

using wglChoosePixelFormatARB_t = BOOL(*)(HDC, const int*, const FLOAT*, UINT, int*, UINT*);
static wglChoosePixelFormatARB_t FwglChoosePixelFormatARB = nullptr;

using wglCreateContextAttribsARB_t = HGLRC(*)(HDC, HGLRC, const int*);
static wglCreateContextAttribsARB_t FwglCreateContextAttribsARB = nullptr;

using wglSwapIntervalEXT_t = BOOL(*)(int);
static wglSwapIntervalEXT_t FwglSwapIntervalEXT = nullptr;

#define GET_WGL_FUN(dl, name) F##name = reinterpret_cast<name##_t>(GetProcAddress(dl, #name))
#define GET_WGL_PROC(name) F##name = reinterpret_cast<name##_t>(FwglGetProcAddress(#name))

#define WGL_DRAW_TO_WINDOW_ARB            0x2001
#define WGL_SUPPORT_OPENGL_ARB            0x2010
#define WGL_DOUBLE_BUFFER_ARB             0x2011
#define WGL_PIXEL_TYPE_ARB                0x2013
#define WGL_TYPE_RGBA_ARB                 0x202B
#define WGL_ACCELERATION_ARB              0x2003
#define WGL_FULL_ACCELERATION_ARB         0x2027
#define WGL_COLOR_BITS_ARB                0x2014
#define WGL_ALPHA_BITS_ARB                0x201B
#define WGL_DEPTH_BITS_ARB                0x2022
#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x0001
#define WGL_SWAP_METHOD_ARB               0x2007
#define WGL_SWAP_COPY_ARB                 0x2029

static bool iconified = false;
static bool requestedQuit = false;
static Win32Window* instance = nullptr;

Win32Window::Win32Window(const std::string& name) {
    this->wname = Utils::convertToWString(name);
}

bool Win32Window::create(HINSTANCE hInstance) {
    if (this->renderer == nullptr) {
        logF("Renderer not set! Aborting");
        std::abort();
    }

    if (this->hwnd != nullptr)
        return false;

    iconified = false;
    requestedQuit = false;
    instance = this;

    const auto icon = b::embed<"assets/icon.png">();
    const auto hIcon = Utils::convertToHICON(icon.data(), icon.size());

    this->hInstance = hInstance;

    this->windowClass.cbSize = sizeof(WNDCLASSEX);
    this->windowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
#ifdef _M_IX86
    this->windowClass.lpfnWndProc = reinterpret_cast<WNDPROC>(windowProc);
#else
    this->windowClass.lpfnWndProc = windowProc;
#endif
    this->windowClass.cbClsExtra = 0;
    this->windowClass.cbWndExtra = 0;
    this->windowClass.hInstance = hInstance;
    this->windowClass.hIcon = hIcon;
    this->windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    this->windowClass.hbrBackground = nullptr;
    this->windowClass.lpszMenuName = nullptr;
    this->windowClass.lpszClassName = this->wname.c_str();
    this->windowClass.hIconSm = hIcon;

    if (RegisterClassExW(&this->windowClass) == 0) {
        const auto lastError = GetLastError();

        logF("Failed to register class: {}", lastError);
        std::abort();
    }

    const int screenWidth  = GetSystemMetrics(SM_CXSCREEN);
    const int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    constexpr int width = 854;
    constexpr int height = 480;

    this->hwnd = CreateWindowExW(WS_EX_LAYERED, this->windowClass.lpszClassName, this->wname.c_str(), WS_POPUP,
        (screenWidth - width) / 2, (screenHeight - height) / 2, width, height, nullptr, nullptr, this->hInstance, nullptr);

    if (this->hwnd == nullptr) {
        const auto lastError = GetLastError();

        logF("Failed to create window: {}", lastError);
        std::abort();
    }

    (void)SetLayeredWindowAttributes(this->hwnd, 0, 255, LWA_ALPHA);

    constexpr MARGINS margins = { -1 };
    (void)DwmExtendFrameIntoClientArea(this->hwnd, &margins);

    const auto rendererType = this->renderer->getRendererType();

    if (rendererType == Renderer::Type::OpenGL) {
        this->openGLDLL = LoadLibraryW(L"opengl32.dll");

        if (this->openGLDLL == nullptr) {
            const auto lastError = GetLastError();

            logF("Failed to load OpenGL DLL: {}", lastError);
            std::abort();
        }

        glMod = this->openGLDLL;
        GET_WGL_FUN(this->openGLDLL, wglGetProcAddress);
        GET_WGL_FUN(this->openGLDLL, wglCreateContext);
        GET_WGL_FUN(this->openGLDLL, wglDeleteContext);
        GET_WGL_FUN(this->openGLDLL, wglMakeCurrent);

        if (FwglGetProcAddress == nullptr || FwglCreateContext == nullptr || FwglDeleteContext == nullptr || FwglMakeCurrent == nullptr) {
            logF("Failed to load Windows OpenGL functions");
            std::abort();
        }

        this->createFakeGLContext();

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
        if (FwglChoosePixelFormatARB(this->hdc, pixelAttribs, nullptr, 1, &pixelFormat, &numFormats) != TRUE) {
            const auto lastError = GetLastError();

            logF("Failed to choose pixel format: {}", lastError);
            std::abort();
        }

        PIXELFORMATDESCRIPTOR pfd{};
        (void)DescribePixelFormat(this->hdc, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

        if (SetPixelFormat(this->hdc, pixelFormat, &pfd) != TRUE) {
            const auto lastError = GetLastError();

            logF("Failed to set pixel format: {}", lastError);
            std::abort();
        }

        const int contextAttributes[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
            WGL_CONTEXT_MINOR_VERSION_ARB, 3,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0
        };

        this->hglrc = FwglCreateContextAttribsARB(this->hdc, nullptr, contextAttributes);

        (void)FwglMakeCurrent(this->hdc, this->hglrc);

        (void)FwglSwapIntervalEXT(1);
    }
    else if (rendererType == Renderer::Type::Vulkan) {
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

    if (rendererType == Renderer::Type::OpenGL) {
        (void)FwglMakeCurrent(nullptr, nullptr);
        (void)ReleaseDC(this->hwnd, this->hdc);
        (void)FwglDeleteContext(this->hglrc);
    }

    this->renderer->exit();

    (void)DestroyWindow(this->hwnd);
    (void)UnregisterClassW(this->windowClass.lpszClassName, this->hInstance);

    this->hwnd = nullptr;
    this->hInstance = nullptr;

    instance = nullptr;

    if (rendererType == Renderer::Type::OpenGL) {
        (void)FreeLibrary(this->openGLDLL);
        this->openGLDLL = nullptr;

        glMod = nullptr;

        FwglGetProcAddress = nullptr;
        FwglCreateContext = nullptr;
        FwglDeleteContext = nullptr;
        FwglMakeCurrent = nullptr;
        FwglChoosePixelFormatARB = nullptr;
        FwglCreateContextAttribsARB = nullptr;
    }
    else if (rendererType == Renderer::Type::Vulkan && this->vulkanDLL != nullptr) {
        (void)FreeLibrary(this->vulkanDLL);
        this->vulkanDLL = nullptr;
    }
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
    const bool swapBuffers = rendererType == Renderer::Type::OpenGL;

    double deltaTime = 0.0;
    auto start = TimeUtils::currentTimeMillis();

    MSG msg{};
    while (msg.message != WM_QUIT && !requestedQuit) {
        if (PeekMessageW(&msg, this->hwnd, 0, 0, PM_REMOVE)) {
            (void)TranslateMessage(&msg);
            (void)DispatchMessageW(&msg);
        }

        if (iconified)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        else {
            this->renderer->render(deltaTime);

            if (swapBuffers)
                (void)SwapBuffers(this->hdc);
        }

        const auto end = TimeUtils::currentTimeMillis();
        const auto diff = end - start;
        deltaTime = static_cast<double>(diff) / 1000.0;
        start = end;
    }
}

void Win32Window::setRenderer(const std::shared_ptr<Renderer>& renderer) {
    this->renderer = renderer;
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

    if (FwglGetProcAddress != nullptr)
        res = FwglGetProcAddress(name);

    if (glMod != nullptr && res == nullptr)
        res = GetProcAddress(glMod, name);

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

LRESULT WINAPI emptyCallback(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    return DefWindowProcW(window, msg, wParam, lParam);
}

void Win32Window::createFakeGLContext() const {
    WNDCLASSEX wndClass{};
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wndClass.lpfnWndProc = emptyCallback;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = this->hInstance;
    wndClass.hIcon = LoadIconW(nullptr, IDI_WINLOGO);
    wndClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wndClass.hbrBackground = nullptr;
    wndClass.lpszMenuName = nullptr;
    wndClass.lpszClassName = L"OGLFakeWindow";
    wndClass.hIconSm = nullptr;

    if (RegisterClassExW(&wndClass) == 0) {
        const auto lastError = GetLastError();

        logF("Failed to register class: {}", lastError);
        std::cerr << "Failed to register class: " << fmt::to_string(lastError) << std::endl;
        std::abort();
    }

    const auto fakeWindow = CreateWindowExW(WS_EX_LAYERED, wndClass.lpszClassName, L"Fake Window", WS_POPUP,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, this->hInstance, nullptr);

    const auto fakeHDC = GetDC(fakeWindow);

    PIXELFORMATDESCRIPTOR pfd{};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cAlphaBits = 8;
    pfd.cDepthBits = 24;

    const auto pf = ChoosePixelFormat(fakeHDC, &pfd);

    if (pf == 0) {
        const auto lastError = GetLastError();

        logF("Failed to choose pixel format: {}", lastError);
        std::cerr << "Failed to choose pixel format: " << fmt::to_string(lastError) << std::endl;

        (void)ReleaseDC(fakeWindow, fakeHDC);
        (void)DestroyWindow(fakeWindow);
        (void)UnregisterClassW(wndClass.lpszClassName, wndClass.hInstance);

        std::abort();
    }

    if (SetPixelFormat(fakeHDC, pf, &pfd) == FALSE) {
        const auto lastError = GetLastError();

        logF("Failed to set pixel format: {}", lastError);
        std::cerr << "Failed to set pixel format: " << fmt::to_string(lastError) << std::endl;

        (void)ReleaseDC(fakeWindow, fakeHDC);
        (void)DestroyWindow(fakeWindow);
        (void)UnregisterClassW(wndClass.lpszClassName, wndClass.hInstance);

        std::abort();
    }

    const auto fakeHGLRC = FwglCreateContext(fakeHDC);
    (void)FwglMakeCurrent(fakeHDC, fakeHGLRC);

    GET_WGL_PROC(wglChoosePixelFormatARB);
    GET_WGL_PROC(wglCreateContextAttribsARB);
    GET_WGL_PROC(wglSwapIntervalEXT);

    (void)FwglMakeCurrent(fakeHDC, nullptr);
    (void)FwglDeleteContext(fakeHGLRC);

    (void)ReleaseDC(fakeWindow, fakeHDC);
    (void)DestroyWindow(fakeWindow);
    (void)UnregisterClassW(wndClass.lpszClassName, wndClass.hInstance);

    if (FwglChoosePixelFormatARB == nullptr || FwglCreateContextAttribsARB == nullptr || FwglSwapIntervalEXT == nullptr) {
        logF("Failed to load Windows OpenGL ARB functions");
        std::cerr << "Failed to load Windows OpenGL ARB functions" << std::endl;
        std::abort();
    }
}

static bool clicked = false;
static POINT posDiff{};

LRESULT Win32Window::windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_SIZE: {
            if (wParam == SIZE_MINIMIZED)
                iconified = true;
            else if (wParam == SIZE_RESTORED)
                iconified = false;
            return 0;
        }
        case WM_LBUTTONDOWN: {
            if (instance != nullptr && instance->renderer != nullptr && instance->renderer->getContext() != nullptr) {
                if (instance->renderer->getContext()->onClick({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}, 0))
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

            if (instance != nullptr && instance->renderer != nullptr && instance->renderer->getContext() != nullptr) {
                if (instance->renderer->getContext()->onRelease({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}, 0))
                    return 0;
            }

            return 0;
        }
        case WM_RBUTTONDOWN: {
            if (instance != nullptr && instance->renderer != nullptr && instance->renderer->getContext() != nullptr) {
                if (instance->renderer->getContext()->onClick({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}, 1))
                    return 0;
            }

            break;
        }
        case WM_RBUTTONUP: {
            if (instance != nullptr && instance->renderer != nullptr && instance->renderer->getContext() != nullptr) {
                if (instance->renderer->getContext()->onRelease({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}, 1))
                    return 0;
            }

            break;
        }
        case WM_MBUTTONDOWN: {
            if (instance != nullptr && instance->renderer != nullptr && instance->renderer->getContext() != nullptr) {
                if (instance->renderer->getContext()->onClick({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}, 2))
                    return 0;
            }

            break;
        }
        case WM_MBUTTONUP: {
            if (instance != nullptr && instance->renderer != nullptr && instance->renderer->getContext() != nullptr) {
                if (instance->renderer->getContext()->onRelease({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}, 2))
                    return 0;
            }

            break;
        }
        case WM_MOUSEMOVE: {
            if (instance != nullptr && instance->renderer != nullptr && instance->renderer->getContext() != nullptr) {
                const auto context = instance->renderer->getContext();

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
            requestedQuit = true;
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}
