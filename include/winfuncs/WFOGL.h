#pragma once

#include "WFGDI.h"

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

namespace WF {
    class OGL {
    public:
        static void* WINAPI wglGetProcAddress(const char* procName) {
            if (FwglGetProcAddress == nullptr)
                return nullptr;

            return FwglGetProcAddress(procName);
        }

        static HGLRC WINAPI wglCreateContext(HDC hdc) {
            if (FwglCreateContext == nullptr)
                return nullptr;

            return FwglCreateContext(hdc);
        }

        static BOOL WINAPI wglDeleteContext(HGLRC hglrc) {
            if (FwglDeleteContext == nullptr)
                return FALSE;

            return FwglDeleteContext(hglrc);
        }

        static BOOL WINAPI wglMakeCurrent(HDC hdc, HGLRC hglrc) {
            if (FwglMakeCurrent == nullptr)
                return FALSE;

            return FwglMakeCurrent(hdc, hglrc);
        }

        static BOOL WINAPI wglChoosePixelFormatARB(HDC hdc, const int* pIAttributes, const FLOAT* pFAttributes, UINT attributeCount, int* pPixelFormat, UINT* pNumFormats) {
            if (FwglChoosePixelFormatARB == nullptr)
                return FALSE;

            return FwglChoosePixelFormatARB(hdc, pIAttributes, pFAttributes, attributeCount, pPixelFormat, pNumFormats);
        }

        static HGLRC WINAPI wglCreateContextAttribsARB(HDC hdc, HGLRC hglrc, const int* pContextAttributes) {
            if (FwglCreateContextAttribsARB == nullptr)
                return nullptr;

            return FwglCreateContextAttribsARB(hdc, hglrc, pContextAttributes);
        }

        static BOOL WINAPI wglSwapIntervalEXT(int interval) {
            if (!FwglSwapIntervalEXT)
                return FALSE;

            return FwglSwapIntervalEXT(interval);
        }

        static void init() {
            if (initialized)
                return;

            GDI::init();

            initialized = true;

            oglDll = LoadLibraryW(L"opengl32.dll");

            FwglGetProcAddress = reinterpret_cast<wglGetProcAddress_t>(GetProcAddress(oglDll, "wglGetProcAddress"));
            FwglCreateContext = reinterpret_cast<wglCreateContext_t>(GetProcAddress(oglDll, "wglCreateContext"));
            FwglDeleteContext = reinterpret_cast<wglDeleteContext_t>(GetProcAddress(oglDll, "wglDeleteContext"));
            FwglMakeCurrent = reinterpret_cast<wglMakeCurrent_t>(GetProcAddress(oglDll, "wglMakeCurrent"));

            createFakeContext();
        }

        static void exit() {
            if (!initialized)
                return;

            GDI::exit();

            initialized = false;

            if (oglDll != nullptr)
                (void)FreeLibrary(oglDll);
        }

        static HMODULE getModule() {
            return oglDll;
        }

        static bool hasGetProcAddress() {
            return FwglGetProcAddress != nullptr;
        }

    private:
        static inline bool initialized = false;
        static inline HMODULE oglDll = nullptr;

        using wglGetProcAddress_t = void*(WINAPI*)(const char*);
        static inline wglGetProcAddress_t FwglGetProcAddress = nullptr;

        using wglCreateContext_t = HGLRC(WINAPI*)(HDC);
        static inline wglCreateContext_t FwglCreateContext = nullptr;

        using wglDeleteContext_t = BOOL(WINAPI*)(HGLRC);
        static inline wglDeleteContext_t FwglDeleteContext = nullptr;

        using wglMakeCurrent_t = BOOL(WINAPI*)(HDC, HGLRC);
        static inline wglMakeCurrent_t FwglMakeCurrent = nullptr;

        using wglChoosePixelFormatARB_t = BOOL(WINAPI*)(HDC, const int*, const FLOAT*, UINT, int*, UINT*);
        static inline wglChoosePixelFormatARB_t FwglChoosePixelFormatARB = nullptr;

        using wglCreateContextAttribsARB_t = HGLRC(WINAPI*)(HDC, HGLRC, const int*);
        static inline wglCreateContextAttribsARB_t FwglCreateContextAttribsARB = nullptr;

        using wglSwapIntervalEXT_t = BOOL(WINAPI*)(int);
        static inline wglSwapIntervalEXT_t FwglSwapIntervalEXT = nullptr;

        static LRESULT WINAPI emptyCallback(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
            return DefWindowProcW(window, msg, wParam, lParam);
        }

        static void createFakeContext() {
            WNDCLASSEXW wndClass{};
            wndClass.cbSize = sizeof(WNDCLASSEXW);
            wndClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
            wndClass.lpfnWndProc = emptyCallback;
            wndClass.cbClsExtra = 0;
            wndClass.cbWndExtra = 0;
            wndClass.hInstance = GetModuleHandleW(nullptr);
            wndClass.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
            wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
            wndClass.hbrBackground = nullptr;
            wndClass.lpszMenuName = nullptr;
            wndClass.lpszClassName = L"OGLFakeWindow";
            wndClass.hIconSm = nullptr;

            if (RegisterClassExW(&wndClass) == 0) {
                const auto lastError = GetLastError();

                std::cerr << "Failed to register fake class: " << lastError << std::endl;
                return;
            }

            const auto fakeWindow = CreateWindowExW(WS_EX_LAYERED, wndClass.lpszClassName, L"Fake Window", WS_POPUP,
                CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, wndClass.hInstance, nullptr);

            const auto fakeHDC = GetDC(fakeWindow);

            PIXELFORMATDESCRIPTOR pfd{};
            pfd.nSize = sizeof(pfd);
            pfd.nVersion = 1;
            pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
            pfd.iPixelType = PFD_TYPE_RGBA;
            pfd.cColorBits = 32;
            pfd.cAlphaBits = 8;
            pfd.cDepthBits = 24;

            const auto pf = GDI::ChoosePixelFormat(fakeHDC, &pfd);

            if (pf == 0) {
                const auto lastError = GetLastError();

                std::cerr << "Failed to choose fake pixel format: " << lastError << std::endl;

                (void)ReleaseDC(fakeWindow, fakeHDC);
                (void)DestroyWindow(fakeWindow);
                (void)UnregisterClassW(wndClass.lpszClassName, wndClass.hInstance);
                return;
            }

            if (GDI::SetPixelFormat(fakeHDC, pf, &pfd) == FALSE) {
                const auto lastError = GetLastError();

                std::cerr << "Failed to set fake pixel format: " << lastError << std::endl;

                (void)ReleaseDC(fakeWindow, fakeHDC);
                (void)DestroyWindow(fakeWindow);
                (void)UnregisterClassW(wndClass.lpszClassName, wndClass.hInstance);
                return;
            }

            const auto fakeHGLRC = wglCreateContext(fakeHDC);
            (void)wglMakeCurrent(fakeHDC, fakeHGLRC);

            FwglChoosePixelFormatARB = reinterpret_cast<wglChoosePixelFormatARB_t>(FwglGetProcAddress("wglChoosePixelFormatARB"));
            FwglCreateContextAttribsARB = reinterpret_cast<wglCreateContextAttribsARB_t>(FwglGetProcAddress("wglCreateContextAttribsARB"));
            FwglSwapIntervalEXT = reinterpret_cast<wglSwapIntervalEXT_t>(FwglGetProcAddress("wglSwapIntervalEXT"));

            (void)wglMakeCurrent(fakeHDC, nullptr);
            (void)wglDeleteContext(fakeHGLRC);

            (void)ReleaseDC(fakeWindow, fakeHDC);
            (void)DestroyWindow(fakeWindow);
            (void)UnregisterClassW(wndClass.lpszClassName, wndClass.hInstance);
        }
    };
}
