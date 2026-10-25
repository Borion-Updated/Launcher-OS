#pragma once

namespace DX9 {
    struct Image {
        LPDIRECT3DTEXTURE9 texture{};
        int width{};
        int height{};
        D3DFORMAT format{};
    };
}
