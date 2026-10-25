#include "Utils.h"

#include "../DXCheck.h"

void DX9::Utils::copyVertices(const void* data, const size_t dataSize, LPDIRECT3DVERTEXBUFFER9 buffer) {
    if (data == nullptr || dataSize == 0 || buffer == nullptr)
        return;

    void* buf{};
    DX_CHECK(buffer->Lock, 0, 0, &buf, 0);

    (void)memcpy(buf, data, dataSize);

    DX_CHECK(buffer->Unlock);
}

void DX9::Utils::copyIndices(const void* data, const size_t dataSize, LPDIRECT3DINDEXBUFFER9 buffer) {
    if (data == nullptr || dataSize == 0 || buffer == nullptr)
        return;

    void* buf{};
    DX_CHECK(buffer->Lock, 0, 0, &buf, 0);

    (void)memcpy(buf, data, dataSize);

    DX_CHECK(buffer->Unlock);
}

void DX9::Utils::copyTexture(const uint8_t* data, const UINT width, const UINT height, LPDIRECT3DTEXTURE9 texture) {
    if (data == nullptr || width == 0 || height == 0 || texture == nullptr)
        return;

    D3DLOCKED_RECT lock{};
    DX_CHECK(texture->LockRect, 0, &lock, nullptr, 0);

    for (int y = 0; y < height; y++) {
        const auto destRow = static_cast<uint8_t*>(lock.pBits) + y * lock.Pitch;
        const auto srcRow  = data + y * width * 4;

        (void)memcpy(destRow, srcRow, width * 4);
    }

    DX_CHECK(texture->UnlockRect, 0);
}
