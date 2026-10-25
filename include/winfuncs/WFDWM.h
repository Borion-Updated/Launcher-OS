#pragma once

namespace WF {
    class DWM {
    public:
        static HRESULT DwmExtendFrameIntoClientArea(HWND hwnd, const MARGINS* pMarInset) {
            if (FDwmExtendFrameIntoClientArea == nullptr)
                return 0;

            return FDwmExtendFrameIntoClientArea(hwnd, pMarInset);
        }

        static void init() {
            if (initialized)
                return;

            initialized = true;

            dwmapiDll = LoadLibraryW(L"dwmapi.dll");

            if (dwmapiDll == nullptr)
                return;

            FDwmExtendFrameIntoClientArea = reinterpret_cast<PFN_DwmExtendFrameIntoClientArea>(GetProcAddress(dwmapiDll, "DwmExtendFrameIntoClientArea"));
        }

        static void exit() {
            if (!initialized)
                return;

            initialized = false;

            if (dwmapiDll != nullptr)
                (void)FreeLibrary(dwmapiDll);
        }

    private:
        static inline bool initialized = false;
        static inline HMODULE dwmapiDll = nullptr;

        using PFN_DwmExtendFrameIntoClientArea = HRESULT(*)(HWND, const MARGINS*);
        static inline PFN_DwmExtendFrameIntoClientArea FDwmExtendFrameIntoClientArea = nullptr;
    };
}
