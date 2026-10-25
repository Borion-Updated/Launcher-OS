#pragma once

namespace WF {
    class User32 {
    public:
        static BOOL WINAPI SetProcessDPIAware() {
            if (FSetProcessDPIAware == nullptr)
                return FALSE;

            return FSetProcessDPIAware();
        }

        static UINT WINAPI GetDpiForSystem() {
            if (FGetDpiForSystem == nullptr)
                return 0;

            return FGetDpiForSystem();
        }

        static BOOL WINAPI AdjustWindowRectEx(LPRECT lpRect, DWORD dwFlags, BOOL bMenu, DWORD dwExStyle) {
            if (FAdjustWindowRectEx == nullptr)
                return FALSE;

            return FAdjustWindowRectEx(lpRect, dwFlags, bMenu, dwExStyle);
        }

        static BOOL WINAPI AdjustWindowRectExForDpi(LPRECT lpRect, DWORD dwFlags, BOOL bMenu, DWORD dwExStyle, UINT dpi) {
            if (FAdjustWindowRectExForDpi == nullptr)
                return FALSE;

            return FAdjustWindowRectExForDpi(lpRect, dwFlags, bMenu, dwExStyle, dpi);
        }

        static BOOL WINAPI SetLayeredWindowAttributes(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags) {
            if (FSetLayeredWindowAttributes == nullptr)
                return FALSE;

            return FSetLayeredWindowAttributes(hwnd, crKey, bAlpha, dwFlags);
        }

        static void init() {
            if (initialized)
                return;

            initialized = true;

            user32Dll = LoadLibraryW(L"user32.dll");

            FSetProcessDPIAware = reinterpret_cast<PFN_SetProcessDPIAware>(GetProcAddress(user32Dll, "SetProcessDPIAware"));
            FGetDpiForSystem = reinterpret_cast<PFN_GetDpiForSystem>(GetProcAddress(user32Dll, "GetDpiForSystem"));
            FAdjustWindowRectEx = reinterpret_cast<PFN_AdjustWindowRectEx>(GetProcAddress(user32Dll, "AdjustWindowRectEx"));
            FAdjustWindowRectExForDpi = reinterpret_cast<PFN_AdjustWindowRectExForDpi>(GetProcAddress(user32Dll, "AdjustWindowRectExForDpi"));
            FSetLayeredWindowAttributes = reinterpret_cast<PFN_SetLayeredWindowAttributes>(GetProcAddress(user32Dll, "SetLayeredWindowAttributes"));
        }

        static void exit() {
            if (!initialized)
                return;

            initialized = false;

            if (user32Dll != nullptr)
                (void)FreeLibrary(user32Dll);
        }

    private:
        static inline bool initialized = false;
        static inline HMODULE user32Dll = nullptr;

        using PFN_SetProcessDPIAware = BOOL(WINAPI*)();
        static inline PFN_SetProcessDPIAware FSetProcessDPIAware = nullptr;

        using PFN_GetDpiForSystem = UINT(WINAPI*)();
        static inline PFN_GetDpiForSystem FGetDpiForSystem = nullptr;

        using PFN_AdjustWindowRectEx = BOOL(WINAPI*)(LPRECT, DWORD, BOOL, DWORD);
        static inline PFN_AdjustWindowRectEx FAdjustWindowRectEx = nullptr;

        using PFN_AdjustWindowRectExForDpi = BOOL(WINAPI*)(LPRECT, DWORD, BOOL, DWORD, UINT);
        static inline PFN_AdjustWindowRectExForDpi FAdjustWindowRectExForDpi = nullptr;

        using PFN_SetLayeredWindowAttributes = BOOL(WINAPI*)(HWND, COLORREF, BYTE, DWORD);
        static inline PFN_SetLayeredWindowAttributes FSetLayeredWindowAttributes = nullptr;
    };
}
