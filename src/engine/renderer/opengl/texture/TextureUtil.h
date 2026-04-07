#pragma once

namespace OGL {
    class TextureUtil {
    public:
        static GLuint loadTexture(const void* data, GLsizei width, GLsizei height);
    };
}
