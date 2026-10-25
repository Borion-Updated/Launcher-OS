#pragma once

namespace WF {
    class DX9 {
    public:
        static IDirect3D9* WINAPI Direct3DCreate9(UINT SDKVersion) {
            return FDirect3DCreate9(SDKVersion);
        }

        static void init() {
            if (initialized)
                return;

            initialized = true;

            dx9xDll = LoadLibraryW(L"d3d9.dll");

            FDirect3DCreate9 = reinterpret_cast<PFN_Direct3DCreate9>(GetProcAddress(dx9xDll, "Direct3DCreate9"));
        }

        static void exit() {
            if (!initialized)
                return;

            initialized = false;

            if (dx9xDll != nullptr)
                (void)FreeLibrary(dx9xDll);
        }

    private:
        static inline bool initialized = false;
        static inline HMODULE dx9xDll = nullptr;

        using PFN_Direct3DCreate9 = IDirect3D9*(WINAPI*)(UINT);
        static inline PFN_Direct3DCreate9 FDirect3DCreate9 = nullptr;
    };
}
