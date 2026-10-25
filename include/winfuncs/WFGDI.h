#pragma once

namespace WF {
    class GDI {
    public:
        static int WINAPI ChoosePixelFormat(HDC hdc, const PIXELFORMATDESCRIPTOR *pfd) {
            if (FChoosePixelFormat == nullptr)
                return 0;

            return FChoosePixelFormat(hdc, pfd);
        }

        static BOOL WINAPI SetPixelFormat(HDC hdc, int format, const PIXELFORMATDESCRIPTOR *pfd) {
            if (FSetPixelFormat == nullptr)
                return FALSE;

            return FSetPixelFormat(hdc, format, pfd);
        }

        static int WINAPI DescribePixelFormat(HDC hdc, int iPixelFormat, UINT nBytes, LPPIXELFORMATDESCRIPTOR ppfd) {
            if (FDescribePixelFormat == nullptr)
                return 0;

            return FDescribePixelFormat(hdc, iPixelFormat, nBytes, ppfd);
        }

        static BOOL WINAPI SwapBuffers(HDC hdc) {
            if (FSwapBuffers == nullptr)
                return FALSE;

            return FSwapBuffers(hdc);
        }

        static void init() {
            if (initialized)
                return;

            initialized = true;

            gdi32Dll = LoadLibraryW(L"gdi32.dll");

            FChoosePixelFormat = reinterpret_cast<ChoosePixelFormat_t>(GetProcAddress(gdi32Dll, "ChoosePixelFormat"));
            FSetPixelFormat = reinterpret_cast<SetPixelFormat_t>(GetProcAddress(gdi32Dll, "SetPixelFormat"));
            FDescribePixelFormat = reinterpret_cast<DescribePixelFormat_t>(GetProcAddress(gdi32Dll, "DescribePixelFormat"));
            FSwapBuffers = reinterpret_cast<SwapBuffers_t>(GetProcAddress(gdi32Dll, "SwapBuffers"));
        }

        static void exit() {
            if (!initialized)
                return;

            initialized = false;

            if (gdi32Dll != nullptr)
                (void)FreeLibrary(gdi32Dll);
        }

    private:
        static inline bool initialized = false;
        static inline HMODULE gdi32Dll = nullptr;

        using ChoosePixelFormat_t = int(WINAPI*)(HDC, const PIXELFORMATDESCRIPTOR*);
        static inline ChoosePixelFormat_t FChoosePixelFormat = nullptr;

        using SetPixelFormat_t = BOOL(WINAPI*)(HDC, int, const PIXELFORMATDESCRIPTOR*);
        static inline SetPixelFormat_t FSetPixelFormat = nullptr;

        using DescribePixelFormat_t = int(WINAPI*)(HDC, int, UINT, LPPIXELFORMATDESCRIPTOR);
        static inline DescribePixelFormat_t FDescribePixelFormat = nullptr;

        using SwapBuffers_t = BOOL(WINAPI*)(HDC);
        static inline SwapBuffers_t FSwapBuffers = nullptr;
    };
}
