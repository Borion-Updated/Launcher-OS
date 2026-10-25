#pragma once

namespace DX9 {
    class Utils {
    public:
        static void copyVertices(const void* data, size_t dataSize, LPDIRECT3DVERTEXBUFFER9 buffer);
        static void copyIndices(const void* data, size_t dataSize, LPDIRECT3DINDEXBUFFER9 buffer);
        static void copyTexture(const uint8_t* data, UINT width, UINT height, LPDIRECT3DTEXTURE9 texture);
    };
}
